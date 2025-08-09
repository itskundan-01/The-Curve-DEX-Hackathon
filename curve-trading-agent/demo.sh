#!/bin/bash

# Curve Trading Agent Demo Script
# This demonstrates the interactive capabilities expected by the hackathon

echo "🚀 Curve Trading Agent Demo"
echo "==============================="
echo ""

# Set demo environment variables (using testnet for safety)
export ETH_RPC_URL="https://sepolia.infura.io/v3/your-project-id"
export CHAIN_ID="11155111"  # Sepolia testnet
export WALLET_PRIVATE_KEY="your-private-key-here"
export CURVE_POOL_ADDRESS="0x7f90122BF0700F9E7e1F688fe926940E8839F353"  # Example testnet pool
export TOKEN_SELL="0x11fE4B6AE13d2a6055C8D9cF65c55bac32B5d844"       # Example DAI testnet
export TOKEN_BUY="0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2"        # WETH
export TOKEN_INDEX_SELL="0"
export TOKEN_INDEX_BUY="1"
export POLL_INTERVAL_MS="2000"
export MAX_SLIPPAGE_BPS="100"  # 1% for demo
export DRY_RUN="true"  # Safety first!

echo "📋 Demo Configuration:"
echo "  Network: Sepolia Testnet"
echo "  Pool: $CURVE_POOL_ADDRESS"
echo "  Sell Token: DAI (index $TOKEN_INDEX_SELL)"
echo "  Buy Token: WETH (index $TOKEN_INDEX_BUY)"
echo "  Poll Interval: ${POLL_INTERVAL_MS}ms"
echo "  Max Slippage: $(echo "scale=2; $MAX_SLIPPAGE_BPS/100" | bc)%"
echo "  Dry Run: $DRY_RUN"
echo ""

echo "🔧 Building application..."
cd build && make -j4

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo ""
    
    echo "🧪 Running core tests..."
    ./test_price_utils
    echo ""
    
    echo "🎯 Demo Commands Available:"
    echo ""
    echo "1️⃣  Interactive Mode:"
    echo "   ./trading_agent --interactive"
    echo ""
    echo "2️⃣  Submit Order (Command Line):"
    echo "   ./trading_agent submit \\"
    echo "     --sell-token $TOKEN_SELL \\"
    echo "     --buy-token $TOKEN_BUY \\"
    echo "     --pool $CURVE_POOL_ADDRESS \\"
    echo "     --amount 1.0 \\"
    echo "     --target-price 0.0005 \\"
    echo "     --policy GTC \\"
    echo "     --slippage 100"
    echo ""
    echo "3️⃣  List Orders:"
    echo "   ./trading_agent list"
    echo ""
    echo "4️⃣  Start Engine:"
    echo "   ./trading_agent engine --start"
    echo ""
    echo "5️⃣  Monitor and Execute:"
    echo "   ./trading_agent monitor"
    echo ""
    
    echo "📊 Expected Live Demo Flow:"
    echo "1. Start engine in background"
    echo "2. Submit a limit order via CLI"
    echo "3. Show order monitoring and price checking"
    echo "4. Demonstrate order execution when target met"
    echo "5. Show transaction hash and blockchain explorer"
    echo "6. Display order logs and final status"
    echo ""
    
    echo "🎬 For hackathon recording:"
    echo "- Use small amounts (0.01 ETH equivalent)"
    echo "- Set realistic price targets that will trigger"
    echo "- Show both successful and cancelled orders"
    echo "- Demonstrate all three policies (GTC, IOC, FOK)"
    echo "- Show transaction on Etherscan/explorer"
    echo ""
    
else
    echo "❌ Build failed! Check compilation errors above."
    exit 1
fi

echo "🚀 Ready for live demo! Good luck with the hackathon! 🏆"
