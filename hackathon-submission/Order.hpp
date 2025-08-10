#pragma once

#include <string>
#include <chrono>
#include <optional>
#include <cstdint>

namespace CurveTrading {

enum class OrderStatus {
    NEW,
    WORKING,
    FILLED,
    CANCELED,
    EXPIRED,
    FAILED
};

enum class OrderPolicy {
    GTC,  // Good Till Canceled
    GTT,  // Good Till Time (added for hackathon)
    IOC,  // Immediate or Cancel
    FOK   // Fill or Kill
};

struct Order {
    std::string id;
    
    // Token and pool information
    std::string sellToken;
    std::string buyToken;
    std::string pool;
    int32_t i;  // Curve index for sell token
    int32_t j;  // Curve index for buy token
    
    // Order parameters
    uint64_t amountIn;  // Amount to sell (in base units)
    double targetPrice; // Target price (buy per 1 sell, normalized to human units)
    OrderPolicy policy;
    uint16_t maxSlippageBps; // Max slippage in basis points (1 = 0.01%)
    
    // Timing
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
    std::optional<std::chrono::system_clock::time_point> expiry;
    
    // State
    OrderStatus status;
    std::string failureReason;
    std::optional<std::string> txHash;
    
    // Metadata
    std::string note;
    
    Order();
    Order(const std::string& sellToken, const std::string& buyToken, 
          const std::string& pool, int32_t i, int32_t j,
          uint64_t amountIn, double targetPrice, OrderPolicy policy,
          uint16_t maxSlippageBps = 50,
          std::optional<std::chrono::system_clock::time_point> expiryTime = std::nullopt);
    
    // Validation
    bool isValid() const;
    bool isExpired() const;
    bool canExecute() const;
    
    // Serialization
    std::string toJson() const;
    static Order fromJson(const std::string& json);
    
    // Status management
    void markWorking();
    void markFilled(const std::string& txHash);
    void markCanceled();
    void markExpired();
    void markFailed(const std::string& reason);
};

// Helper functions
std::string orderStatusToString(OrderStatus status);
std::string orderPolicyToString(OrderPolicy policy);
OrderStatus orderStatusFromString(const std::string& str);
OrderPolicy orderPolicyFromString(const std::string& str);

} // namespace CurveTrading
