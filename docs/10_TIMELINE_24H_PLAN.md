# 24-Hour Execution Plan

Time is from pressing “Start Challenge”. Adjust as needed.

- H0–H1: Read starter code in `challenge/`, understand provided interfaces, wire CMake, build skeleton
- H1–H3: EthereumClient (RPC) + Config loader, integrate with starter structure
- H3–H5: ABI minimal (ERC20, Curve get_dy/exchange), adapt to starter patterns
- H5–H6: ERC20Service + CurveService
- H6–H8: Tx builder (EIP-1559), signer, nonce mgr
- H8–H10: LimitOrderEngine (model, policies, tick)
- H10–H11: CLI submit/list/cancel
- H11–H13: Unit tests for core components
- H13–H15: Integration tests with mocks; fix edges
- H15–H16: Dry-run e2e; parameter tuning
- H16–H18: Live small trade on testnet/mainnet (optional)
- H18–H20: Hardening: retries, logging, doc polish
- H20–H22: Recording: tests + demo run
- H22–H24: Final QA, package, submission upload

Buffers: 2 hours sprinkled across blocks for surprises.
