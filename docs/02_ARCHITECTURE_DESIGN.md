# Architecture & Design

## High-Level Diagram
Components (all C++ modules):
- Config & Secrets
- JSON-RPC EthereumClient (HTTP)
- ABI Codec (encode/decode)
- ERC20 Service (allowance, approve, decimals)
- Curve Service (quote via get_dy, swap)
- Price Monitor (poll quotes, compare target)
- LimitOrder Engine (policies, lifecycle)
- Tx Builder (EIP-1559)
- Storage (in-memory + JSON append log)
- CLI (submit/list/cancel)

Data flow: CLI -> Engine -> Services -> EthereumClient -> Node; responses -> Engine -> Storage/Logs.

## Core Interfaces (contracts)
- EthereumClient
  - call(contract, data) -> bytes
  - sendRawTransaction(txBytes) -> txHash
  - getNonce(addr), getGasPrice(), estimateGas(tx)
- AbiCodec
  - encode(function, args) -> bytes
  - decode(types, data) -> values
- ERC20Service
  - allowance(owner, spender)
  - approve(spender, amount)
  - decimals(token)
- CurveService
  - quote(pool, i, j, dx) -> dy
  - swap(pool, i, j, dx, minDy, receiver)
  - virtualPrice(pool) -> vp (optional sanity signal)
- LimitOrderEngine
  - submit(order) -> id
  - cancel(id)
  - tick(now) -> actions

## Order Model
- id: string (uuid)
- sellToken, buyToken: address
- pool: address
- i, j: int (Curve index)
- amountIn: uint256
- targetPrice: rational (buy/sell per unit)
- policy: enum { GTC, IOC, FOK }
- maxSlippageBps: uint16
- expiry: timestamp (optional)
- status: enum { NEW, WORKING, FILLED, CANCELED, EXPIRED, FAILED }

## Policy Semantics
- GTC: execute when quote >= target (for sells), persists until filled/cancel/expiry
- IOC: attempt once; fill if possible within slippage else cancel
- FOK: only execute if full size meets target+slippage in one tx; else cancel
 - Note: Post Only is not applicable to AMM swaps and is intentionally excluded.

## Error Handling
- Deterministic exceptions -> mapped to engine events
- Transient RPC errors -> retry/backoff
- Revert on swap -> mark FAILED with reason

## Performance & Polling
- Poll interval: 1-3s (configurable)
- Batch multiple orders per tick
- Debounce gas estimation if unchanged
