#pragma once

#include "Order.hpp"
#include <cstdint>

namespace CurveTrading {

// Result of policy evaluation
struct PolicyResult {
    bool shouldExecute;
    bool shouldCancel;
    std::string reason;
    
    PolicyResult(bool execute = false, bool cancel = false, const std::string& reason = "")
        : shouldExecute(execute), shouldCancel(cancel), reason(reason) {}
};

class OrderPolicies {
public:
    // Main policy evaluation function
    static PolicyResult evaluateOrder(const Order& order, double currentPrice, uint64_t availableLiquidity = 0);
    
    // Individual policy evaluations
    static PolicyResult evaluateGTC(const Order& order, double currentPrice);
    static PolicyResult evaluateGTT(const Order& order, double currentPrice);
    static PolicyResult evaluateIOC(const Order& order, double currentPrice, uint64_t availableLiquidity = 0);
    static PolicyResult evaluateFOK(const Order& order, double currentPrice, uint64_t availableLiquidity);
    
    // Helper functions
    static bool isPriceAcceptable(const Order& order, double currentPrice);
    static bool hasEnoughLiquidity(const Order& order, uint64_t availableLiquidity);
    static std::string getPolicyDescription(OrderPolicy policy);
};

} // namespace CurveTrading
