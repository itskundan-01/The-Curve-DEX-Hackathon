# Runtime Operations (Runbook)

Operational guidance for running and observing the agent during the hackathon.

## Modes
- CLI submit/list/cancel
- Engine run loop with polling

## Typical Flow
1) Export env vars (RPC, key, pool, tokens)
2) Start engine in a shell
3) Submit a limit order via CLI
4) Observe logs; on fill, note tx hash

## Logging
- Structured logs: time, level, component, message, key fields
- Persist JSON events to logs/orders.log

## Troubleshooting
- No quotes: check pool address and indices
- Revert: inspect minDy and slippage; increase slippage temporarily
- Nonce errors: ensure single engine instance; reset nonce from chain
- RPC 429: backoff and reduce poll interval

## Safety & Controls
- Dry-run flag to avoid sending transactions (for demo)
- Max gas guard: abort if estimated gas > threshold
- Global stop: SIGINT gracefully stops engine and flushes logs
