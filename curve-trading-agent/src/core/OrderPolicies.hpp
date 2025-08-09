#pragma once

#include "../core/Order.hpp"
#include "../interfaces/ICurveService.hpp"
#include "../utils/PriceUtils.hpp"

namespace CurveTrading {

enum class PolicyResult {
    WAIT,           // Continue monitoring
    EXECUTE,        // Execute the order
    CANCEL,         // Cancel the order (IOC/FOK)
    EXPIRE          // Order has expired
};

struct PolicyEvaluation {
    PolicyResult result;
    std::string reason;
    CurveQuote quote;
    bool shouldExecute;
    uint64_t minDy;
};

class OrderPolicies {
public:
    // Evaluate order against its policy
    static PolicyEvaluation evaluate(const Order& order, const CurveQuote& quote, 
                                    uint8_t sellDecimals, uint8_t buyDecimals);
    
    // Policy-specific evaluations
    static PolicyEvaluation evaluateGTC(const Order& order, const CurveQuote& quote,
                                       uint8_t sellDecimals, uint8_t buyDecimals);
    static PolicyEvaluation evaluateIOC(const Order& order, const CurveQuote& quote,
                                       uint8_t sellDecimals, uint8_t buyDecimals);
    static PolicyEvaluation evaluateFOK(const Order& order, const CurveQuote& quote,
                                       uint8_t sellDecimals, uint8_t buyDecimals);
    
    // Utility functions
    static bool isQuoteAcceptable(const Order& order, const CurveQuote& quote,
                                uint8_t sellDecimals, uint8_t buyDecimals);
    static uint64_t calculateMinDy(const Order& order, uint64_t dy);
    static std::string getPolicyDescription(OrderPolicy policy);
};

} // namespace CurveTrading
