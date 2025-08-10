#!/bin/bash

# 🚀 Order Execution Test Script
# This script demonstrates order execution in dry run mode

echo "🔬 Testing Order Execution Simulation"
echo "======================================"

cd build

echo ""
echo "📋 Test Plan:"
echo "1. Create GTC order with achievable target price"
echo "2. Monitor order status and execution"
echo "3. Verify mock transaction generation"
echo ""

echo "🚀 Starting test..."
echo ""

# Create order with low target price to trigger execution
echo -e "submit\nWETH\nUSDC\n0.1\n100.0\nGTC\n0.5\nlist\nstatus\nquit" | ./curve_limit_order_agent

echo ""
echo "✅ Test completed!"
echo ""
echo "📊 Expected Results:"
echo "- Order created with UUID"
echo "- Status shows 'WORKING'"
echo "- Mock execution may occur if price conditions met"
echo "- Mock transaction hash: 0xfff...fff"
echo ""
echo "💡 Note: Running in DRY RUN mode - no real transactions!"
