#pragma once

#include "../core/Order.hpp"
#include "../core/LimitOrderEngine.hpp"
#include <memory>
#include <string>
#include <vector>

namespace CurveTrading {

class CLI {
public:
    CLI(std::shared_ptr<LimitOrderEngine> engine);
    ~CLI() = default;
    
    // Main CLI loop
    void run();
    void runInteractive();
    
    // Command handlers
    void handleSubmitOrder();
    void handleListOrders();
    void handleCancelOrder();
    void handleOrderStatus();
    void handleStartEngine();
    void handleStopEngine();
    void handleStats();
    void handleHelp();
    void handleQuit();
    
    // Non-interactive commands (for scripting)
    int submitOrder(const std::string& sellToken, const std::string& buyToken,
                   const std::string& pool, int32_t i, int32_t j,
                   double amountIn, double targetPrice, const std::string& policy,
                   uint16_t slippageBps = 50, uint32_t expiryMinutes = 0);
    
    int listOrders();
    int cancelOrder(const std::string& orderId);
    int getOrderStatus(const std::string& orderId);
    int showStats();
    
    // Configuration
    void setWalletAddress(const std::string& address) { m_walletAddress = address; }
    void setDryRun(bool dryRun) { m_dryRun = dryRun; }
    
private:
    std::shared_ptr<LimitOrderEngine> m_engine;
    bool m_running;
    bool m_dryRun;
    std::string m_walletAddress;
    
    // Helper methods
    void printWelcome();
    void printHelp();
    void printOrder(const Order& order);
    void printOrderSummary(const Order& order);
    std::string readLine(const std::string& prompt);
    double readDouble(const std::string& prompt);
    uint64_t readUint64(const std::string& prompt);
    int32_t readInt32(const std::string& prompt);
    uint16_t readUint16(const std::string& prompt);
    OrderPolicy parsePolicy(const std::string& policyStr);
    
    // Validation helpers
    bool validateAddress(const std::string& address);
    bool validateAmount(double amount);
    bool validatePrice(double price);
};

} // namespace CurveTrading
