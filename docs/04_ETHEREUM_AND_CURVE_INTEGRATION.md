# Ethereum & Curve Integration Notes

This document captures all on-chain integration details needed to implement and test the trading agent in C++.

## JSON-RPC Endpoints Used
- eth_chainId
- eth_blockNumber
- eth_call
- eth_estimateGas
- eth_gasPrice or eth_feeHistory (preferred)
- eth_maxPriorityFeePerGas (fallback if feeHistory not available)
- eth_getTransactionCount (nonce)
- eth_sendRawTransaction
- eth_getTransactionReceipt

RPC strategy:
- Prefer EIP-1559: baseFee from blocks + priority fee strategy
- Retry transient errors with exponential backoff (e.g., 200ms, 500ms, 1s, 2s)
- Timeout per request: 5s default (configurable)

## Transaction Model (EIP-1559)
- Fields: chainId, nonce, maxPriorityFeePerGas, maxFeePerGas, gasLimit, to, value, data, accessList (optional)
- Signing: secp256k1 over keccak256(rlp(typedData))
- Serialization: EIP-2718 typed transaction (type 0x02)

Recommended defaults:
- gasLimit: estimateGas + 15% buffer
- priority fee: configurable, default 1.5 gwei
- max fee: baseFee * 2 + priority fee

## ABI & Encoding
We only need a small subset of ERC-20 and Curve pool ABIs. Implement a minimal ABI encoder/decoder for:

ERC-20
- function decimals() view returns (uint8)
- function allowance(address owner, address spender) view returns (uint256)
- function approve(address spender, uint256 amount) returns (bool)
- function balanceOf(address owner) view returns (uint256) [optional]

Curve Pool (varies by pool; parameterize at runtime)
- function get_dy(int128 i, int128 j, uint256 dx) view returns (uint256)
- function exchange(int128 i, int128 j, uint256 dx, uint256 min_dy) returns (uint256)
- function exchange_underlying(int128 i, int128 j, uint256 dx, uint256 min_dy) returns (uint256) [if underlying assets]
 - function get_virtual_price() view returns (uint256) [optional sanity/telemetry]

Notes:
- Some pools use uint256 for i/j; make indices configurable and support both ABI shapes (documented via a config switch).
- If the provided starter includes specific pool bindings, adopt those first.

## Addressing & Indices
- pool: Curve pool contract address
- token i/j: pool-specific index mapping; verify off-chain before placing orders
- receiver: typically msg.sender; we’ll set to our wallet address (no separate receiver parameter in standard exchange)

## Quotes vs Execution
- Quote: call get_dy(i,j,dx)
- Execution: call exchange(i,j,dx,min_dy)
- Slippage: min_dy = dy * (1 - slippageBps/10_000)
 - Optional telemetry: compare trends in get_virtual_price for pool health; do not use as sole execution signal.

## Allowances
- Spender: pool address
- Ensure allowance >= amountIn prior to swap
- If insufficient, send approve(spender, maxUint) or exact amount (configurable)
- Wait for approval receipt before swap (or optimistic with nonce sequencing; for simplicity, wait)

## Chain & Tokens
- Chain: mainnet by default; allow testnet via config
- Token decimals: fetch via decimals() and cache
- Amount conversions: human -> base units using decimals

## Safety Checks
- Reject orders if expiry passed
- Validate pool supports requested i/j pair via a dry-run eth_call
- After swap, read receipt status; if failed, mark order FAILED

## Logging & Tracing
- Log raw tx hash and linkable explorer URL (based on chain)
- Persist JSON event: { orderId, action, txHash, gas, quote, minDy, status }

## Signing Options
- Primary: in-process C++ secp256k1 signer (preferred for self-contained demo)
- Alternative: external signer (e.g., offline tool or helper script) producing raw signed tx bytes that the C++ agent can broadcast. Use if cryptographic deps are constrained in GoCode.

## Policy Note
- Post Only orders are DEX orderbook semantics and not applicable to AMM swaps on Curve; omitted by design.
