#include "CLI.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace CurveTrading {

CLI::CLI() {
    initializeTokens();
    setupEngine();
}

CLI::~CLI() {
    stop();
}

void CLI::run() {
    printHeader();
    
    running_ = true;
    engine_->start();
    
    std::cout << "\nCurve DEX Limit Order Agent is ready!" << std::endl;
    std::cout << "Type 'help' for available commands, 'quit' to exit." << std::endl;
    
    interactiveMode();
}

void CLI::stop() {
    running_ = false;
    if (engine_) {
        engine_->stop();
    }
}

void CLI::interactiveMode() {
    std::string input;
    
    while (running_) {
        std::cout << "\ncurve-agent> ";
        std::getline(std::cin, input);
        
        if (input.empty()) continue;
        
        // Convert to lowercase for comparison
        std::string command = input;
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);
        
        if (command == "quit" || command == "exit") {
            break;
        } else if (command == "help" || command == "?") {
            handleShowHelp();
        } else if (command == "submit" || command == "order") {
            handleSubmitOrder();
        } else if (command == "list" || command == "orders") {
            handleListOrders();
        } else if (command == "cancel") {
            handleCancelOrder();
        } else if (command == "monitor" || command == "status") {
            handleMonitorOrders();
        } else if (command == "price" || command == "check") {
            handleCheckPrice();
        } else if (command == "config") {
            handleShowConfig();
        } else {
            std::cout << "Unknown command: " << input << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
    
    std::cout << "\nGoodbye!" << std::endl;
}

void CLI::handleSubmitOrder() {
    try {
        std::cout << "\n=== Submit New Limit Order ===" << std::endl;
        
        // Get order parameters from user
        std::string sellTokenSymbol, buyTokenSymbol;
        double amount, targetPrice;
        std::string policyStr;
        double slippagePct = 0.5; // Default 0.5%
        
        std::cout << "Sell token (USDC/WETH/DAI): ";
        std::cin >> sellTokenSymbol;
        
        std::cout << "Buy token (USDC/WETH/DAI): ";
        std::cin >> buyTokenSymbol;
        
        std::cout << "Amount to sell: ";
        std::cin >> amount;
        
        std::cout << "Target price (buy per sell): ";
        std::cin >> targetPrice;
        
        std::cout << "Policy (GTC/GTT/IOC/FOK): ";
        std::cin >> policyStr;
        
        OrderPolicy policy = parsePolicy(policyStr);
        
        // For GTT policy, get expiry time
        uint64_t expiryTime = 0;
        if (policy == OrderPolicy::GTT) {
            std::cout << "Expiry time in seconds from now (e.g., 3600 for 1 hour): ";
            uint64_t secondsFromNow;
            std::cin >> secondsFromNow;
            expiryTime = std::time(nullptr) + secondsFromNow;
        }
        
        std::cout << "Slippage tolerance % (default 0.5): ";
        std::string slippageInput;
        std::cin >> slippageInput;
        if (!slippageInput.empty()) {
            slippagePct = std::stod(slippageInput);
        }
        
        // Look up token addresses
        auto sellTokenIt = tokens_.find(sellTokenSymbol);
        auto buyTokenIt = tokens_.find(buyTokenSymbol);
        
        if (sellTokenIt == tokens_.end() || buyTokenIt == tokens_.end()) {
            std::cout << "Error: Unknown token symbol" << std::endl;
            return;
        }
        
        // Find pool for this pair
        std::string poolKey = sellTokenSymbol + "-" + buyTokenSymbol;
        auto poolIt = poolMap_.find(poolKey);
        if (poolIt == poolMap_.end()) {
            poolKey = buyTokenSymbol + "-" + sellTokenSymbol;
            poolIt = poolMap_.find(poolKey);
        }
        
        if (poolIt == poolMap_.end()) {
            std::cout << "Error: No pool found for token pair " << sellTokenSymbol << "/" << buyTokenSymbol << std::endl;
            return;
        }
        
        // Convert amounts
        uint64_t amountIn = PriceUtils::fromHumanReadable(amount, sellTokenIt->second.decimals);
        uint16_t slippageBps = static_cast<uint16_t>(slippagePct * 100); // Convert to basis points
        
        // Prepare expiry time for GTT orders
        std::optional<std::chrono::system_clock::time_point> expiryTimePoint = std::nullopt;
        if (parsePolicy(policyStr) == OrderPolicy::GTT && expiryTime > 0) {
            expiryTimePoint = std::chrono::system_clock::from_time_t(expiryTime);
        }
        
        // Create order
        Order order(
            sellTokenIt->second.address,
            buyTokenIt->second.address,
            poolIt->second,
            0, 1, // Simplified indices
            amountIn,
            targetPrice,
            parsePolicy(policyStr),
            slippageBps,
            expiryTimePoint
        );
        
        // Submit order
        std::string orderId = engine_->submitOrder(order);
        
        std::cout << "\n✓ Order submitted successfully!" << std::endl;
        std::cout << "Order ID: " << orderId << std::endl;
        std::cout << "Policy: " << OrderPolicies::getPolicyDescription(order.policy) << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error submitting order: " << e.what() << std::endl;
    }
    
    // Clear input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void CLI::handleListOrders() {
    std::cout << "\n=== Active Orders ===" << std::endl;
    
    auto orders = engine_->getActiveOrders();
    
    if (orders.empty()) {
        std::cout << "No active orders." << std::endl;
        return;
    }
    
    printSeparator();
    std::cout << std::left
              << std::setw(12) << "Order ID"
              << std::setw(10) << "Policy"
              << std::setw(15) << "Sell/Buy"
              << std::setw(12) << "Amount"
              << std::setw(12) << "Target Price"
              << std::setw(10) << "Status"
              << std::endl;
    printSeparator();
    
    for (const auto& order : orders) {
        displayOrder(order);
    }
    
    printSeparator();
}

void CLI::handleCancelOrder() {
    std::cout << "\nEnter Order ID to cancel: ";
    std::string orderId;
    std::cin >> orderId;
    
    if (engine_->cancelOrder(orderId)) {
        std::cout << "✓ Order " << orderId << " canceled successfully." << std::endl;
    } else {
        std::cout << "✗ Failed to cancel order " << orderId << std::endl;
    }
}

void CLI::handleMonitorOrders() {
    std::cout << "\n=== Order Status Monitor ===" << std::endl;
    
    auto orders = engine_->getActiveOrders();
    
    if (orders.empty()) {
        std::cout << "No active orders to monitor." << std::endl;
        return;
    }
    
    for (const auto& order : orders) {
        std::cout << "\nOrder " << order.id.substr(0, 8) << "..." << std::endl;
        std::cout << "  Status: " << orderStatusToString(order.status) << std::endl;
        std::cout << "  Policy: " << orderPolicyToString(order.policy) << std::endl;
        
        // Show execution result if available
        auto result = engine_->getExecutionResult(order.id);
        if (result.success) {
            std::cout << "  ✓ Executed - TxHash: " << result.txHash.substr(0, 10) << "..." << std::endl;
            std::cout << "  Actual Price: " << result.actualPrice << std::endl;
        } else if (!result.errorMessage.empty() && result.errorMessage != "No execution result found") {
            std::cout << "  ✗ Failed: " << result.errorMessage << std::endl;
        }
    }
}

void CLI::handleCheckPrice() {
    std::cout << "\n=== Check Current Price ===" << std::endl;
    
    std::string sellToken, buyToken;
    double amount;
    
    std::cout << "Sell token: ";
    std::cin >> sellToken;
    
    std::cout << "Buy token: ";
    std::cin >> buyToken;
    
    std::cout << "Amount: ";
    std::cin >> amount;
    
    // Look up tokens and pool
    auto sellIt = tokens_.find(sellToken);
    auto buyIt = tokens_.find(buyToken);
    
    if (sellIt == tokens_.end() || buyIt == tokens_.end()) {
        std::cout << "Error: Unknown token" << std::endl;
        return;
    }
    
    // Find pool
    std::string poolKey = sellToken + "-" + buyToken;
    auto poolIt = poolMap_.find(poolKey);
    if (poolIt == poolMap_.end()) {
        poolKey = buyToken + "-" + sellToken;
        poolIt = poolMap_.find(poolKey);
    }
    
    if (poolIt == poolMap_.end()) {
        std::cout << "Error: No pool found for pair" << std::endl;
        return;
    }
    
    try {
        uint64_t amountIn = PriceUtils::fromHumanReadable(amount, sellIt->second.decimals);
        double currentPrice = engine_->getCurrentPrice(poolIt->second, 0, 1, amountIn);
        
        std::cout << "\nCurrent price: " << currentPrice << " " << buyToken << " per " << sellToken << std::endl;
        std::cout << "Expected output: " << (amount * currentPrice) << " " << buyToken << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error checking price: " << e.what() << std::endl;
    }
}

void CLI::handleShowConfig() {
    std::cout << "\n=== Configuration ===" << std::endl;
    std::cout << "Engine running: " << (engine_->isRunning() ? "Yes" : "No") << std::endl;
    std::cout << "Available tokens:" << std::endl;
    
    for (const auto& pair : tokens_) {
        std::cout << "  " << pair.first << ": " << pair.second.address.substr(0, 10) 
                  << "... (" << static_cast<int>(pair.second.decimals) << " decimals)" << std::endl;
    }
    
    std::cout << "Available pools:" << std::endl;
    for (const auto& pair : poolMap_) {
        std::cout << "  " << pair.first << ": " << pair.second.substr(0, 10) << "..." << std::endl;
    }
}

void CLI::handleShowHelp() {
    std::cout << "\n=== Available Commands ===" << std::endl;
    std::cout << "  submit/order    - Submit a new limit order" << std::endl;
    std::cout << "  list/orders     - List all active orders" << std::endl;
    std::cout << "  cancel          - Cancel an order by ID" << std::endl;
    std::cout << "  monitor/status  - Show order execution status" << std::endl;
    std::cout << "  price/check     - Check current market price" << std::endl;
    std::cout << "  config          - Show engine configuration" << std::endl;
    std::cout << "  help/?          - Show this help message" << std::endl;
    std::cout << "  quit/exit       - Exit the application" << std::endl;
    
    std::cout << "\n=== Order Policies ===" << std::endl;
    std::cout << "  GTC - Good Till Canceled (default)" << std::endl;
    std::cout << "  GTT - Good Till Time (with expiry)" << std::endl;
    std::cout << "  IOC - Immediate or Cancel (partial fills OK)" << std::endl;
    std::cout << "  FOK - Fill or Kill (full fill or nothing)" << std::endl;
}

void CLI::displayOrder(const Order& order) {
    // Extract token symbols from addresses (simplified)
    std::string sellSymbol = "???", buySymbol = "???";
    for (const auto& pair : tokens_) {
        if (pair.second.address == order.sellToken) sellSymbol = pair.first;
        if (pair.second.address == order.buyToken) buySymbol = pair.first;
    }
    
    std::cout << std::left
              << std::setw(12) << order.id.substr(0, 8) + "..."
              << std::setw(10) << orderPolicyToString(order.policy)
              << std::setw(15) << (sellSymbol + "/" + buySymbol)
              << std::setw(12) << PriceUtils::formatAmount(order.amountIn, 18, 2)
              << std::setw(12) << PriceUtils::formatPrice(order.targetPrice, 4)
              << std::setw(10) << orderStatusToString(order.status)
              << std::endl;
}

std::string CLI::parseTokenSymbol(const std::string& input) {
    std::string upper = input;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    return upper;
}

double CLI::parseAmount(const std::string& input) {
    return std::stod(input);
}

OrderPolicy CLI::parsePolicy(const std::string& input) {
    std::string upper = input;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    if (upper == "GTC") return OrderPolicy::GTC;
    if (upper == "GTT") return OrderPolicy::GTT;
    if (upper == "IOC") return OrderPolicy::IOC;
    if (upper == "FOK") return OrderPolicy::FOK;
    
    throw std::invalid_argument("Unknown policy: " + input);
}

void CLI::initializeTokens() {
    // Sepolia testnet token addresses - REAL ADDRESSES RESEARCHED  
    tokens_["USDC"] = {"0x1c7D4B196Cb0C7B01d743Fbc6116a902379C7238", "USDC", 6};   // Circle testnet USDC
    tokens_["WETH"] = {"0xfff9976782d46cc05630d1f6ebab18b2324d6b14", "WETH", 18};  // Official Sepolia WETH
    tokens_["DAI"] = {"0x3e622317f8C93f7328350cF0B56d9eD4C620C5d6", "DAI", 18};    // DAI testnet token
    
    // Pool mappings - Using Curve Factory on Sepolia (pools need to be discovered)
    poolMap_["USDC-WETH"] = "0xfb37b8D939FFa77114005e61CFc2e543d6F49A81";  // Factory address - pools discovered dynamically
    poolMap_["USDC-DAI"] = "0xfb37b8D939FFa77114005e61CFc2e543d6F49A81";   // Factory address - pools discovered dynamically  
    poolMap_["WETH-DAI"] = "0xfb37b8D939FFa77114005e61CFc2e543d6F49A81";   // Factory address - pools discovered dynamically
}

void CLI::setupEngine() {
    EngineConfig config;
    
    // Sepolia testnet RPC URL - REAL ENDPOINT 
    config.rpcUrl = "https://eth-sepolia.public.blastapi.io"; // Public Sepolia RPC
    config.userAddress = "0x0000000000000000000000000000000000000000"; // Will be configured later
    config.privateKey = "0x0000000000000000000000000000000000000000000000000000000000000000"; // Will be configured later
    config.priceCheckIntervalMs = 2000; // 2 seconds for demo
    config.dryRun = true; // Start in dry run mode for safety
    
    // Configure token decimals
    for (const auto& pair : tokens_) {
        config.tokenDecimals[pair.second.address] = pair.second.decimals;
    }
    
    engine_ = std::make_unique<LimitOrderEngine>(config);
}

void CLI::printHeader() {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "           CURVE DEX LIMIT ORDER AGENT" << std::endl;
    std::cout << "         Hackathon Challenge Solution" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void CLI::printSeparator() {
    std::cout << std::string(80, '-') << std::endl;
}

std::vector<std::string> CLI::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

} // namespace CurveTrading
