#include "OrderPolicies.hpp"
#include <chrono>

namespace CurveTrading {

PolicyResult OrderPolicies::evaluateOrder(const Order& order, double currentPrice, uint64_t availableLiquidity) {
    // First check if order is expired
    if (order.isExpired()) {
        return PolicyResult(false, true, "Order expired");
    }
    
    // Check if order is in valid state for execution
    if (!order.canExecute()) {
        return PolicyResult(false, false, "Order not in executable state");
    }
    
    // Evaluate based on policy
    switch (order.policy) {
        case OrderPolicy::GTC:
            return evaluateGTC(order, currentPrice);
        case OrderPolicy::GTT:
            return evaluateGTT(order, currentPrice);
        case OrderPolicy::IOC:
            return evaluateIOC(order, currentPrice, availableLiquidity);
        case OrderPolicy::FOK:
            return evaluateFOK(order, currentPrice, availableLiquidity);
        default:
            return PolicyResult(false, true, "Unknown order policy");
    }
}

PolicyResult OrderPolicies::evaluateGTC(const Order& order, double currentPrice) {
    if (isPriceAcceptable(order, currentPrice)) {
        return PolicyResult(true, false, "Target price reached - executing GTC order");
    }
    
    return PolicyResult(false, false, "Waiting for target price");
}

PolicyResult OrderPolicies::evaluateGTT(const Order& order, double currentPrice) {
    // Check if expired first (should be caught earlier but double-check)
    if (order.isExpired()) {
        return PolicyResult(false, true, "GTT order expired");
    }
    
    if (isPriceAcceptable(order, currentPrice)) {
        return PolicyResult(true, false, "Target price reached - executing GTT order");
    }
    
    return PolicyResult(false, false, "Waiting for target price before expiry");
}

PolicyResult OrderPolicies::evaluateIOC(const Order& order, double currentPrice, uint64_t availableLiquidity) {
    if (!isPriceAcceptable(order, currentPrice)) {
        return PolicyResult(false, true, "IOC: Price not acceptable - canceling immediately");
    }
    
    // For IOC, we execute whatever is available, even partial fills
    // If liquidity is provided and it's less than requested, we still execute
    if (availableLiquidity > 0 && availableLiquidity < order.amountIn) {
        return PolicyResult(true, false, "IOC: Executing partial fill");
    }
    
    return PolicyResult(true, false, "IOC: Executing full order");
}

PolicyResult OrderPolicies::evaluateFOK(const Order& order, double currentPrice, uint64_t availableLiquidity) {
    if (!isPriceAcceptable(order, currentPrice)) {
        return PolicyResult(false, true, "FOK: Price not acceptable - killing order");
    }
    
    if (!hasEnoughLiquidity(order, availableLiquidity)) {
        return PolicyResult(false, true, "FOK: Insufficient liquidity for full fill - killing order");
    }
    
    return PolicyResult(true, false, "FOK: Executing full order");
}

bool OrderPolicies::isPriceAcceptable(const Order& order, double currentPrice) {
    // For a sell order (selling sellToken for buyToken):
    // We want to get at least targetPrice of buyToken per unit of sellToken
    // So currentPrice should be >= targetPrice
    return currentPrice >= order.targetPrice;
}

bool OrderPolicies::hasEnoughLiquidity(const Order& order, uint64_t availableLiquidity) {
    return availableLiquidity >= order.amountIn;
}

std::string OrderPolicies::getPolicyDescription(OrderPolicy policy) {
    switch (policy) {
        case OrderPolicy::GTC:
            return "Good Till Canceled - Order remains active until filled or manually canceled";
        case OrderPolicy::GTT:
            return "Good Till Time - Order remains active until filled, manually canceled, or expires";
        case OrderPolicy::IOC:
            return "Immediate or Cancel - Execute immediately at current price or cancel. Partial fills allowed";
        case OrderPolicy::FOK:
            return "Fill or Kill - Execute full amount immediately at target price or cancel. No partial fills";
        default:
            return "Unknown policy";
    }
}

} // namespace CurveTrading
