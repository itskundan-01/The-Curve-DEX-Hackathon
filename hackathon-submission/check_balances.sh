#!/bin/bash

# Wallet balance checker for Curve DEX Limit Order Agent
# Checks balances for the configured MetaMask wallet

echo "🚀 Curve DEX Agent - Wallet Balance Check"
echo "========================================="

# MetaMask wallet address from our testing session
ADDRESS="0x0dc7987e20953546F50c92CbC9d45A8baf638690"

echo "🔍 Checking wallet balances..."
echo "Address: $ADDRESS"
echo "Network: Sepolia Testnet"
echo ""

# Check ETH balance
echo "📡 Checking ETH balance..."
ETH_BALANCE=$(curl -s -X POST \
  -H "Content-Type: application/json" \
  --data "{\"jsonrpc\":\"2.0\",\"method\":\"eth_getBalance\",\"params\":[\"$ADDRESS\",\"latest\"],\"id\":1}" \
  https://eth-sepolia.public.blastapi.io | jq -r '.result')

echo "ETH Balance (hex): $ETH_BALANCE"

# Convert hex to decimal (ETH)
if [ "$ETH_BALANCE" != "null" ]; then
    ETH_DECIMAL=$(python3 -c "print(int('$ETH_BALANCE', 16) / 10**18)")
    echo "ETH Balance (decimal): $ETH_DECIMAL ETH"
fi

# Check USDC balance
echo "📡 Checking USDC balance..."
USDC_ADDRESS="0x1c7D4B196Cb0C7B01d743Fbc6116a902379C7238"

# USDC balanceOf call data: balanceOf(address) = 0x70a08231 + padded address
BALANCE_DATA="0x70a08231000000000000000000000000${ADDRESS:2}"

USDC_BALANCE=$(curl -s -X POST \
  -H "Content-Type: application/json" \
  --data "{\"jsonrpc\":\"2.0\",\"method\":\"eth_call\",\"params\":[{\"to\":\"$USDC_ADDRESS\",\"data\":\"$BALANCE_DATA\"},\"latest\"],\"id\":1}" \
  https://eth-sepolia.public.blastapi.io | jq -r '.result')

echo "USDC Balance (hex): $USDC_BALANCE"

# Convert hex to decimal (USDC - 6 decimals)
if [ "$USDC_BALANCE" != "null" ]; then
    USDC_DECIMAL=$(python3 -c "print(int('$USDC_BALANCE', 16) / 10**6)")
    echo "USDC Balance (decimal): $USDC_DECIMAL USDC"
fi

echo ""
echo "✅ Balance check complete!"
echo "🎯 Ready to configure live trading agent..."
