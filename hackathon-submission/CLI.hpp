#pragma once

#include "LimitOrderEngine.hpp"
#include <string>
#include <vector>
#include <memory>

namespace CurveTrading {

class CLI {
private:
    std::unique_ptr<LimitOrderEngine> engine_;
    bool running_ = false;
    bool trackingActive_ = false;  // New: tracks if price monitoring is active
    
    // Token configuration for Sepolia testnet
    struct TokenInfo {
        std::string address;
        std::string symbol;
        uint8_t decimals;
    };
    
    std::map<std::string, TokenInfo> tokens_;
    std::map<std::string, std::string> poolMap_; // token1-token2 -> pool address

public:
    CLI();
    ~CLI();
    
    // Main CLI functions
    void run();
    void stop();
    
    // Command handlers
    void handleSubmitOrder();
    void handleListOrders();
    void handleCancelOrder();
    void handleMonitorOrders();
    void handleCheckPrice();
    void handleShowConfig();
    void handleToggleMode();
    void handleCheckBalance();
    void handleShowHelp();
    void handleTrackLogs();    // New: start tracking
    void handleCloseLogs();    // New: stop tracking
    
    // Interactive mode
    void interactiveMode();
    
    // Utility functions
    void displayOrder(const Order& order);
    void displayExecutionResult(const std::string& orderId);
    std::string parseTokenSymbol(const std::string& input);
    double parseAmount(const std::string& input);
    OrderPolicy parsePolicy(const std::string& input);
    bool isTrackingActive() const { return trackingActive_; }  // New: check tracking state
    
private:
    void initializeTokens();
    void setupEngine();
    std::vector<std::string> splitString(const std::string& str, char delimiter);
    void printHeader();
    void printSeparator();
};

} // namespace CurveTrading
