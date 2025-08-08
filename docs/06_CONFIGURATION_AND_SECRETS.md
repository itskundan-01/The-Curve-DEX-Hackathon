# Configuration & Secrets

Use env vars or CLI flags; never hardcode secrets in code or Git.

## Environment Variables
- ETH_RPC_URL: HTTPS URL to Ethereum node
- CHAIN_ID: 1 (mainnet) or testnet id
- WALLET_PRIVATE_KEY: hex string without 0x
- CURVE_POOL_ADDRESS: address
- TOKEN_SELL: address
- TOKEN_BUY: address
- TOKEN_INDEX_SELL: integer index i
- TOKEN_INDEX_BUY: integer index j
- POLL_INTERVAL_MS: default 1500
- MAX_SLIPPAGE_BPS: default 50 (0.5%)
- GAS_PRIORITY_FEE_GWEI: default 1.5
- GAS_LIMIT_BUFFER_PERCENT: default 15
- APPROVAL_MODE: exact|max (default max)
- ABI_VARIANT: curve_pool|curve_underlying (choose exchange vs exchange_underlying)

## Sample .env (example)
ETH_RPC_URL=https://mainnet.infura.io/v3/<project>
CHAIN_ID=1
WALLET_PRIVATE_KEY=...redacted...
CURVE_POOL_ADDRESS=0x...
TOKEN_SELL=0x...
TOKEN_BUY=0x...
TOKEN_INDEX_SELL=0
TOKEN_INDEX_BUY=1
POLL_INTERVAL_MS=1500
MAX_SLIPPAGE_BPS=50
GAS_PRIORITY_FEE_GWEI=1.5
GAS_LIMIT_BUFFER_PERCENT=15
APPROVAL_MODE=max
ABI_VARIANT=curve_pool

## Secrets Handling
- Provide secrets via environment only in GoCode
- Do not commit or echo the private key
- Consider using a keystore file if provided by starter (optional)

## Configuration Precedence
1) CLI flags
2) Environment variables
3) Defaults in code
