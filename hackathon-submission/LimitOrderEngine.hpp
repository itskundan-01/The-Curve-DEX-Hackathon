#pragma once

#include "Order.hpp"
#include "OrderPolicies.hpp"
#include "PriceUtils.hpp"
#include "PriceFetcher.hpp"
#include "blockchain_services.hpp"
#include <memory>
#include <vector>
#include <map>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace CurveTrading {

// Configuration for the limit order engine
struct EngineConfig {
    std::string rpcUrl;
    std::string userAddress;
    std::string privateKey;
    uint32_t priceCheckIntervalMs = 1000; // 1 second
    bool dryRun = false;
    
    // Token configurations (decimals for price calculations)
    std::map<std::string, uint8_t> tokenDecimals;
};

// Order execution result
struct ExecutionResult {
    bool success;
    std::string txHash;
    std::string errorMessage;
    uint64_t actualAmountOut = 0;
    double actualPrice = 0.0;
};

class LimitOrderEngine {
private:
    EngineConfig config_;
    std::unique_ptr<EthereumRPC> rpc_;
    std::unique_ptr<CurveMetaRegistry> registry_;
    std::unique_ptr<PriceFetcher> priceFetcher_;
    
    // Order management
    std::map<std::string, Order> activeOrders_;
    mutable std::mutex ordersMutex_;
    
    // Monitoring thread
    std::thread monitorThread_;
    std::atomic<bool> running_{false};
    std::condition_variable stopCondition_;
    
    // Execution tracking
    std::map<std::string, ExecutionResult> executionResults_;
    mutable std::mutex resultsMutex_;

public:
    explicit LimitOrderEngine(const EngineConfig& config);
    ~LimitOrderEngine();
    
    // Engine lifecycle
    void start();
    void stop();
    bool isRunning() const { return running_; }
    
    // Configuration
    bool isDryRun() const { return config_.dryRun; }
    void setDryRun(bool dryRun) { config_.dryRun = dryRun; }
    EthereumRPC* getRPC() const { return rpc_.get(); }
    
    // Order management
    std::string submitOrder(const Order& order);
    bool cancelOrder(const std::string& orderId);
    std::vector<Order> getActiveOrders() const;
    std::vector<Order> getAllOrders() const;
    Order getOrder(const std::string& orderId) const;
    
    // Execution results
    ExecutionResult getExecutionResult(const std::string& orderId) const;
    
    // Price monitoring
    double getCurrentPrice(const std::string& pool, int32_t i, int32_t j, uint64_t amount);
    double getRealTimePrice(const std::string& sellToken, const std::string& buyToken);
    uint64_t getAvailableLiquidity(const std::string& pool, int32_t i, int32_t j, uint64_t amount);
    
    // Manual execution (for testing)
    ExecutionResult executeOrder(const std::string& orderId);

private:
    // Core engine functions
    void monitorLoop();
    void processOrder(Order& order);
    ExecutionResult executeSwap(const Order& order, double currentPrice);
    
    // Blockchain interactions
    std::string findPool(const std::string& fromToken, const std::string& toToken);
    uint64_t checkBalance(const std::string& tokenAddress);
    
    // Enhanced demo pricing with market simulation
    double getEnhancedDemoPrice(int32_t i, int32_t j, uint64_t amount);
    
    // Logging
    void logOrderEvent(const Order& order, const std::string& event, const std::string& details = "");
    void logExecutionResult(const Order& order, const ExecutionResult& result);
    
    // Helper functions
    uint8_t getTokenDecimals(const std::string& tokenAddress) const;
    void updateOrderStatus(Order& order, OrderStatus newStatus, const std::string& reason = "");
};

} // namespace CurveTrading
