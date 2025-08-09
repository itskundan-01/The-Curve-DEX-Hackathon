#pragma once

#include "Order.hpp"
#include "OrderPolicies.hpp"
#include "../interfaces/IOrderStorage.hpp"
#include "../interfaces/ICurveService.hpp"
#include "../interfaces/IERC20Service.hpp"
#include "../interfaces/IEthereumClient.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <chrono>

namespace CurveTrading {

struct EngineStats {
    uint32_t ordersSubmitted;
    uint32_t ordersFilled;
    uint32_t ordersCanceled;
    uint32_t ordersExpired;
    uint32_t ordersFailed;
    uint32_t ticksProcessed;
    std::chrono::system_clock::time_point lastTick;
};

struct ExecutionResult {
    bool success;
    std::string txHash;
    std::string reason;
    TransactionReceipt receipt;
};

class LimitOrderEngine {
public:
    using OrderUpdateCallback = std::function<void(const Order&, const std::string&)>;
    using ExecutionCallback = std::function<void(const Order&, const ExecutionResult&)>;
    
    LimitOrderEngine(
        std::shared_ptr<IOrderStorage> storage,
        std::shared_ptr<ICurveService> curveService,
        std::shared_ptr<IERC20Service> erc20Service,
        std::shared_ptr<IEthereumClient> ethClient
    );
    
    ~LimitOrderEngine() = default;
    
    // Order management
    std::string submitOrder(const Order& order);
    bool cancelOrder(const std::string& orderId);
    std::optional<Order> getOrder(const std::string& orderId);
    std::vector<Order> getAllOrders();
    std::vector<Order> getOrdersByStatus(OrderStatus status);
    
    // Engine control
    void start();
    void stop();
    bool isRunning() const { return m_running; }
    void tick();
    
    // Configuration
    void setTickInterval(std::chrono::milliseconds interval) { m_tickInterval = interval; }
    void setDryRun(bool dryRun) { m_dryRun = dryRun; }
    void setWalletAddress(const std::string& address) { m_walletAddress = address; }
    
    // Callbacks
    void setOrderUpdateCallback(OrderUpdateCallback callback) { m_orderUpdateCallback = callback; }
    void setExecutionCallback(ExecutionCallback callback) { m_executionCallback = callback; }
    
    // Statistics
    EngineStats getStats() const { return m_stats; }
    void resetStats();
    
private:
    // Core services
    std::shared_ptr<IOrderStorage> m_storage;
    std::shared_ptr<ICurveService> m_curveService;
    std::shared_ptr<IERC20Service> m_erc20Service;
    std::shared_ptr<IEthereumClient> m_ethClient;
    
    // Engine state
    bool m_running;
    bool m_dryRun;
    std::string m_walletAddress;
    std::chrono::milliseconds m_tickInterval;
    EngineStats m_stats;
    
    // Callbacks
    OrderUpdateCallback m_orderUpdateCallback;
    ExecutionCallback m_executionCallback;
    
    // Decimal cache (token -> decimals)
    std::unordered_map<std::string, uint8_t> m_decimalsCache;
    
    // Internal methods
    void processWorkingOrders();
    void processOrder(Order& order);
    ExecutionResult executeOrder(const Order& order, const PolicyEvaluation& eval);
    void updateOrderStatus(Order& order, OrderStatus newStatus, const std::string& reason = "");
    
    // Token utilities
    uint8_t getTokenDecimals(const std::string& token);
    void cacheTokenDecimals(const std::string& token);
    
    // Validation
    bool validateOrder(const Order& order);
    bool validateOrderExecution(const Order& order);
    
    // Approval management
    bool ensureApproval(const Order& order);
    
    // Logging helpers
    void logOrderUpdate(const Order& order, const std::string& message);
    void logExecution(const Order& order, const ExecutionResult& result);
};

} // namespace CurveTrading
