#include "OrderPolicies.hpp"
#include <sstream>

namespace CurveTrading {

PolicyEvaluation OrderPolicies::evaluate(const Order& order, const CurveQuote& quote,
                                        uint8_t sellDecimals, uint8_t buyDecimals) {
    // Check expiry first
    if (order.isExpired()) {
        return {PolicyResult::EXPIRE, "Order has expired", quote, false, 0};
    }
    
    // Delegate to policy-specific evaluation
    switch (order.policy) {
        case OrderPolicy::GTC:
            return evaluateGTC(order, quote, sellDecimals, buyDecimals);
        case OrderPolicy::IOC:
            return evaluateIOC(order, quote, sellDecimals, buyDecimals);
        case OrderPolicy::FOK:
            return evaluateFOK(order, quote, sellDecimals, buyDecimals);
        default:
            return {PolicyResult::CANCEL, "Unknown policy", quote, false, 0};
    }
}

PolicyEvaluation OrderPolicies::evaluateGTC(const Order& order, const CurveQuote& quote,
                                           uint8_t sellDecimals, uint8_t buyDecimals) {
    PolicyEvaluation result;
    result.quote = quote;
    result.minDy = calculateMinDy(order, quote.dy);
    
    if (!quote.valid) {
        result.result = PolicyResult::WAIT;
        result.reason = "Invalid quote, will retry";
        result.shouldExecute = false;
        return result;
    }
    
    bool acceptable = isQuoteAcceptable(order, quote, sellDecimals, buyDecimals);
    
    if (acceptable) {
        result.result = PolicyResult::EXECUTE;
        result.reason = "Target price reached";
        result.shouldExecute = true;
    } else {
        result.result = PolicyResult::WAIT;
        result.reason = "Waiting for target price";
        result.shouldExecute = false;
    }
    
    return result;
}

PolicyEvaluation OrderPolicies::evaluateIOC(const Order& order, const CurveQuote& quote,
                                           uint8_t sellDecimals, uint8_t buyDecimals) {
    PolicyEvaluation result;
    result.quote = quote;
    result.minDy = calculateMinDy(order, quote.dy);
    
    if (!quote.valid) {
        result.result = PolicyResult::CANCEL;
        result.reason = "IOC: Invalid quote, canceling";
        result.shouldExecute = false;
        return result;
    }
    
    bool acceptable = isQuoteAcceptable(order, quote, sellDecimals, buyDecimals);
    
    if (acceptable) {
        result.result = PolicyResult::EXECUTE;
        result.reason = "IOC: Executing immediately";
        result.shouldExecute = true;
    } else {
        result.result = PolicyResult::CANCEL;
        result.reason = "IOC: Target price not met, canceling";
        result.shouldExecute = false;
    }
    
    return result;
}

PolicyEvaluation OrderPolicies::evaluateFOK(const Order& order, const CurveQuote& quote,
                                           uint8_t sellDecimals, uint8_t buyDecimals) {
    PolicyEvaluation result;
    result.quote = quote;
    result.minDy = calculateMinDy(order, quote.dy);
    
    if (!quote.valid) {
        result.result = PolicyResult::CANCEL;
        result.reason = "FOK: Invalid quote, canceling";
        result.shouldExecute = false;
        return result;
    }
    
    bool acceptable = isQuoteAcceptable(order, quote, sellDecimals, buyDecimals);
    
    if (acceptable) {
        // FOK requires full execution - we assume single trade can fill the order
        // In a more complex system, we might check order book depth
        result.result = PolicyResult::EXECUTE;
        result.reason = "FOK: Full execution possible";
        result.shouldExecute = true;
    } else {
        result.result = PolicyResult::CANCEL;
        result.reason = "FOK: Cannot fill completely at target price";
        result.shouldExecute = false;
    }
    
    return result;
}

bool OrderPolicies::isQuoteAcceptable(const Order& order, const CurveQuote& quote,
                                     uint8_t sellDecimals, uint8_t buyDecimals) {
    if (!quote.valid || quote.dy == 0) {
        return false;
    }
    
    // Calculate the implied price from the quote
    double impliedPrice = PriceUtils::calculatePrice(quote.dy, order.amountIn, 
                                                   buyDecimals, sellDecimals);
    
    // Check if it meets our target price (allowing small tolerance for floating point comparison)
    return PriceUtils::isPriceTarget(impliedPrice, order.targetPrice);
}

uint64_t OrderPolicies::calculateMinDy(const Order& order, uint64_t dy) {
    return PriceUtils::calculateMinOut(dy, order.maxSlippageBps);
}

std::string OrderPolicies::getPolicyDescription(OrderPolicy policy) {
    switch (policy) {
        case OrderPolicy::GTC:
            return "Good Till Canceled - Keep trying until filled, canceled, or expired";
        case OrderPolicy::IOC:
            return "Immediate or Cancel - Execute immediately if possible, otherwise cancel";
        case OrderPolicy::FOK:
            return "Fill or Kill - Execute completely or cancel immediately";
        default:
            return "Unknown policy";
    }
}

} // namespace CurveTrading
