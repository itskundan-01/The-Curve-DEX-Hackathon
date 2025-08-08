# Test Strategy & Cases

Primary goal: deterministic tests that validate engine logic without requiring a live chain, plus an optional integration smoke test.

## Test Pyramid
- Unit (heavy):
  - AbiCodec encode/decode
  - Price normalization, slippage math
  - Engine policy transitions (GTC/IOC/FOK)
  - Nonce manager behavior
  - Gas calc given baseFee/priority
- Integration (medium):
  - Approve then swap using mocked EthereumClient receipts
  - Failure paths: revert, insufficient allowance, slippage breach
- E2E (light/optional):
  - Testnet or local fork: one small trade on a known pool

## Mocking
- Provide a MockEthereumClient with scripted responses for eth_call, estimateGas, sendRawTransaction, getTransactionReceipt
- Fixtures for ERC-20 decimals, allowance responses, get_dy quotes

## Key Cases
1) GTC fills when quote >= target
2) IOC cancels when first quote < target
3) FOK cancels if swap fails or quote slips under minDy
4) Approval path: missing allowance triggers approve then swap
5) Expiry kicks in before a quote meets target -> EXPIRED
6) RPC transient error -> retried and then success
7) Nonce gap: send approve and swap sequentially with incremented nonce
8) Gas estimation + buffer applied
9) Decimals: 6->18 token normalization
10) Revert on swap -> FAILED with reason persisted

## Test Data & Determinism
- Fixed decimals (e.g., USDC=6, WETH=18)
- Fixed amounts: dx = 1_000_000 (1 USDC), quotes deterministic
- Disable wall-clock dependence via injected time provider

## How to Run (placeholder)
- Build tests via CMake and run ctest
- Commands will be in main README once code scaffolded
