# Risks & Mitigations

## Technical
- RPC instability or rate limits
  - Use backoff; reduce poll interval; switch endpoint if possible
- ABI mismatch between pools
  - Feature flag for exchange vs exchange_underlying; quick probe via eth_call
- Nonce drift (multiple tx)
  - Single process; track pending nonces; re-sync on error
- Gas spikes
  - Cap maxFee; retry later; allow user override
- Slippage miscalc due to decimals
  - Centralized decimals cache; unit tests covering 6/8/18 decimals

## Product/Scope
- Starter code assumptions differ
  - Adapt interfaces; keep our modules thin and composable
- No testnet access
  - Full mocked flow; record dry-run with signed tx
- Time overrun
  - Follow 24h plan; cut scope via optional features list

## Security
- Key handling in browser env
  - Env-only; never commit; consider ephemeral key for demo
- Approval amount risk
  - Default exact approval; allow max via flag
