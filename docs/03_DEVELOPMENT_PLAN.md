# Development Plan

## Assumptions
- Starter C++ sample provides a skeleton we can extend
- Network RPC endpoint available in GoCode env
- We target Ethereum mainnet by default, with an option for testnet
- Must integrate with and extend the provided C++ starter code in `challenge/` directory
- Submission must demonstrate functional application with live trading capabilities

## Milestones
1) Project scaffolding
- Build system (CMake), deps (cURL/Boost/asio, nlohmann/json, secp256k1)
- Folder layout: src/, include/, tests/, tools/

2) Core plumbing
- Config loader (env + flags)
- EthereumClient (JSON-RPC over HTTP)
- AbiCodec minimal (ERC20, Curve functions we need)

3) Services
- ERC20Service (allowance/approve/decimals)
- CurveService (quote, swap)

4) Tx pipeline
- Nonce mgr, EIP-1559 builder, signer
- Gas estimation + safety buffer

5) LimitOrderEngine
- Order model, storage, policies (GTC/IOC/FOK)
- Price monitor + tick loop

6) CLI
- submit/list/cancel; run engine loop

7) Tests
- Unit tests with mocked EthereumClient
- Integration: happy-path approve+swap (dry-run or testnet)
- Edge cases: slippage breach, expiry, RPC failures

8) Docs + Recording
- README, runbook
- Demo script and recording

## Definition of Done
- All checkboxes in requirements are ticked
- Tests pass; demo narrated recording produced

## Out-of-Scope Risk Controls
- If ABI lib is heavy, implement minimal encoder/decoder for used methods
- If on-chain tx blocked, demonstrate full flow with mocked send and signed tx artifact
