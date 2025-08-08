# Requirements Checklist

Use this as a live tracker while implementing.

## Functional
- [ ] CLI or service to submit/track limit orders
- [ ] Price/quote retrieval from Curve pool(s) (get_dy or router quote)
- [ ] Execution policies: IOC, FOK, GTC with expiry
- [ ] Slippage controls and minOut enforcement
- [ ] ERC-20 approvals (idempotent) and allowance checks
- [ ] EIP-1559 transaction building/signing/broadcasting
- [ ] Retry with backoff for transient RPC errors
- [ ] Persist order state (in-memory + JSON log)

## Non-Functional
- [ ] C++17+ compatible, builds in GoCode environment
- [ ] Unit and integration tests with mocks
- [ ] Structured logging and metrics counters
- [ ] Config via env/flags without hardcoded secrets
- [ ] Clear documentation and demo script

## Curve/Ethereum Specific
- [ ] ABI handling for Curve pools and ERC-20
- [ ] Optional: read get_virtual_price for sanity checks (primary pricing via get_dy)
- [ ] Gas estimation and priority fee strategy
- [ ] Chain selection (mainnet vs testnet) via config
- [ ] Wallet key management (env-variable or keystore)

## Explicit Non-Requirements (for AMMs like Curve)
- [ ] Post Only orders (N/A on AMM; kept out to avoid confusion)

## Deliverables
- [ ] Source code and tests
- [ ] README with run instructions
- [ ] Recording (narration + demo) - 5-8 minutes with audio narration
- [ ] Submission packaging per `challenge/` rules
- [ ] Integration with provided starter C++ code from `challenge/` directory
- [ ] Demonstration of functional application with live trading
