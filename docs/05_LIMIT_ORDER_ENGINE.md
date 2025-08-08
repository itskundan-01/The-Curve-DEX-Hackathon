# Limit Order Engine

This defines the core execution logic, state machine, and policy semantics.

## Order Schema
- id: string (uuid)
- side: enum { SELL }
- sellToken: address
- buyToken: address
- pool: address
- i, j: int (Curve indices for sell->buy)
- amountIn: uint256 (base units)
- targetPrice: rational (buy per 1 sell)
- policy: enum { GTC, IOC, FOK }
- maxSlippageBps: uint16
- expiry: optional timestamp
- createdAt, updatedAt: timestamp
- status: enum { NEW, WORKING, FILLED, CANCELED, EXPIRED, FAILED }
- meta: { note?: string }

## State Machine
- NEW -> WORKING (accepted)
- WORKING -> FILLED (swap success)
- WORKING -> FAILED (revert or permanent error)
- WORKING -> CANCELED (user cancel)
- WORKING -> EXPIRED (now > expiry)

## Evaluation Loop
Pseudocode:
1) For each WORKING order:
   - If expiry && now > expiry: EXPIRED
   - Quote dy = Curve.get_dy(i,j,dx)
   - Optional: vp = Curve.get_virtual_price() for telemetry/sanity
   - impliedPrice = dy / dx (normalized to decimals)
   - If policy==GTC and impliedPrice >= targetPrice: attemptSwap(dy)
   - If policy==IOC: attempt once; if fail or quote < target, cancel
   - If policy==FOK: proceed only if full dx satisfies target and swap succeeds; else cancel

attemptSwap(dy):
- minDy = applySlippage(dy, maxSlippageBps)
- ensureAllowance(pool, amountIn)
- tx = Curve.exchange(i,j,dx,minDy)
- wait receipt; success -> FILLED; else FAILED

## Slippage & Price Normalization
- Use token decimals to normalize dx and dy for price comparison
- TargetPrice uses human units; convert both sides to common 18-decimal fixed or double with care

## Persistence
- In-memory store keyed by id
- Append-only JSON log of events for audit and demo

## Concurrency & Scheduling
- Single-threaded tick loop (simple and safe for 24h hackathon)
- Batch RPC where possible (optional optimization)

## Cancellation Semantics
- Cancel moves order to CANCELED; no further actions
- For IOC/FOK, auto-cancel on first failure/insufficient-quote

## Telemetry
- Counters: ordersSubmitted, filled, canceled, expired, failed
- Timers: quoteLatencyMs, txLatencyMs
 - Gauges: lastVirtualPrice (per pool) [optional]

## Not Supported by Design
- Post Only: not meaningful for AMM swaps; excluded to avoid confusion.
