# Curve DEX Limit Order Agent

A sophisticated C++ limit order agent for Curve DEX implementing all Time-in-Force (TIF) policies with real-time price monitoring and blockchain integration.

## Features

### Core Requirements
- **Real-time Price Monitoring**: Live CoinGecko API integration
- **Complete TIF Policy Support**: GTC, GTT, IOC, FOK fully implemented
- **Order Management**: Professional submission, listing, and cancellation
- **Blockchain Integration**: Live Sepolia testnet connectivity

### TIF Policies Implemented
- **GTC (Good Till Canceled)**: Monitors indefinitely until filled or canceled
- **GTT (Good Till Time)**: Monitors until specified expiry time
- **IOC (Immediate or Cancel)**: Executes immediately or cancels (partial fills allowed)
- **FOK (Fill or Kill)**: Executes full amount immediately or cancels (no partial fills)

## Technical Stack
- **Language**: C++17
- **Build System**: CMake
- **Dependencies**: curl, nlohmann-json
- **Network**: Sepolia Testnet
- **APIs**: CoinGecko for real-time pricing

## Build Instructions

### Prerequisites
```bash
# macOS
brew install cmake curl nlohmann-json

# Ubuntu/Debian
sudo apt install cmake libcurl4-openssl-dev nlohmann-json3-dev
```

### Compilation
```bash
mkdir build && cd build
cmake ..
make -j4
```

## Usage

### Start the Application
```bash
./curve_limit_order_agent
```

### Available Commands
- `submit` - Submit a new limit order
- `list` - List all active orders
- `cancel` - Cancel an order
- `price` - Check current market price
- `balance` - Check wallet balances
- `config` - Show configuration
- `help` - Show all commands
- `quit` - Exit application

### Order Submission Example
```
curve-agent> submit
Sell token: USDC
Buy token: ETH
Amount to sell: 100
Target price: 0.00023
Policy (GTC/GTT/IOC/FOK): GTC
Slippage tolerance %: 0.5
Order submitted successfully!
```

## Architecture

### Core Components
- **LimitOrderEngine**: Main execution engine
- **Order Management**: Order structure and validation
- **OrderPolicies**: TIF policy evaluation
- **PriceFetcher**: Real-time market data
- **CLI**: Interactive user interface
- **BlockchainServices**: Ethereum integration

### Configuration
- **RPC**: Sepolia testnet endpoint
- **Wallet**: Dedicated testnet wallet
- **Price Source**: CoinGecko API
- **Update Interval**: 2-second monitoring

## Testing Results
- All TIF policies working
- Real-time price fetching functional
- Order management complete
- Blockchain integration verified
- Error handling comprehensive

## Security
- Testnet-only implementation
- Dedicated hackathon wallet
- DEMO mode by default
- No mainnet exposure

---
*C++ implementation demonstrating advanced trading logic with real blockchain integration.*
