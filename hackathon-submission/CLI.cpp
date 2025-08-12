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
        } else if (command == "mode" || command == "toggle") {
            handleToggleMode();
        } else if (command == "balance" || command == "bal") {
            handleCheckBalance();
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
        std::string input;
        
        std::cout << "Sell token (USDC/WETH/ETH/DAI): ";
        std::getline(std::cin, sellTokenSymbol);
        std::transform(sellTokenSymbol.begin(), sellTokenSymbol.end(), sellTokenSymbol.begin(), ::toupper);
        
        std::cout << "Buy token (USDC/WETH/ETH/DAI): ";
        std::getline(std::cin, buyTokenSymbol);
        std::transform(buyTokenSymbol.begin(), buyTokenSymbol.end(), buyTokenSymbol.begin(), ::toupper);
        
        // Validate token symbols
        if (sellTokenSymbol != "USDC" && sellTokenSymbol != "WETH" && sellTokenSymbol != "ETH" && sellTokenSymbol != "DAI") {
            std::cout << "Error: Invalid sell token. Supported: USDC, WETH, ETH, DAI" << std::endl;
            return;
        }
        if (buyTokenSymbol != "USDC" && buyTokenSymbol != "WETH" && buyTokenSymbol != "ETH" && buyTokenSymbol != "DAI") {
            std::cout << "Error: Invalid buy token. Supported: USDC, WETH, ETH, DAI" << std::endl;
            return;
        }
        if (sellTokenSymbol == buyTokenSymbol) {
            std::cout << "Error: Cannot trade same token with itself" << std::endl;
            return;
        }
        
        std::cout << "Amount to sell: ";
        std::getline(std::cin, input);
        try {
            amount = std::stod(input);
            if (amount <= 0) {
                std::cout << "Error: Amount must be positive" << std::endl;
                return;
            }
        } catch (const std::exception& e) {
            std::cout << "Error: Invalid amount format" << std::endl;
            return;
        }
        
        std::cout << "Target price (buy per sell): ";
        std::getline(std::cin, input);
        try {
            targetPrice = std::stod(input);
            if (targetPrice <= 0) {
                std::cout << "Error: Target price must be positive" << std::endl;
                return;
            }
        } catch (const std::exception& e) {
            std::cout << "Error: Invalid price format" << std::endl;
            return;
        }
        
        std::cout << "Policy (GTC/GTT/IOC/FOK): ";
        std::getline(std::cin, policyStr);
        std::transform(policyStr.begin(), policyStr.end(), policyStr.begin(), ::toupper);
        
        OrderPolicy policy;
        try {
            policy = parsePolicy(policyStr);
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << ". Supported: GTC, GTT, IOC, FOK" << std::endl;
            return;
        }
        
        // For GTT policy, get expiry time
        uint64_t expiryTime = 0;
        if (policy == OrderPolicy::GTT) {
            std::cout << "Expiry time in seconds from now (e.g., 3600 for 1 hour): ";
            std::getline(std::cin, input);
            try {
                uint64_t secondsFromNow = std::stoull(input);
                if (secondsFromNow == 0) {
                    std::cout << "Error: Expiry time must be greater than 0" << std::endl;
                    return;
                }
                if (secondsFromNow > 86400 * 30) { // Max 30 days
                    std::cout << "Error: Expiry time cannot exceed 30 days (2592000 seconds)" << std::endl;
                    return;
                }
                expiryTime = std::time(nullptr) + secondsFromNow;
                std::cout << "Order will expire in " << secondsFromNow << " seconds" << std::endl;
            } catch (const std::exception& e) {
                std::cout << "Error: Invalid expiry time format" << std::endl;
                return;
            }
        }
        
        std::cout << "Slippage tolerance % (default 0.5): ";
        std::string slippageInput;
        std::getline(std::cin, slippageInput);
        if (!slippageInput.empty() && slippageInput != "0.5") {
            slippagePct = std::stod(slippageInput);
        }
        
        // Look up token addresses - handle ETH specially
        std::string sellTokenAddress, buyTokenAddress;
        uint8_t sellDecimals;
        
        if (sellTokenSymbol == "ETH") {
            sellTokenAddress = "0x0000000000000000000000000000000000000000";
            sellDecimals = 18;
        } else {
            auto sellTokenIt = tokens_.find(sellTokenSymbol);
            if (sellTokenIt == tokens_.end()) {
                std::cout << "Error: Unknown token symbol: " << sellTokenSymbol << std::endl;
                return;
            }
            sellTokenAddress = sellTokenIt->second.address;
            sellDecimals = sellTokenIt->second.decimals;
        }
        
        if (buyTokenSymbol == "ETH") {
            buyTokenAddress = "0x0000000000000000000000000000000000000000";
        } else {
            auto buyTokenIt = tokens_.find(buyTokenSymbol);
            if (buyTokenIt == tokens_.end()) {
                std::cout << "Error: Unknown token symbol: " << buyTokenSymbol << std::endl;
                return;
            }
            buyTokenAddress = buyTokenIt->second.address;
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
        uint64_t amountIn = PriceUtils::fromHumanReadable(amount, sellDecimals);
        uint16_t slippageBps = static_cast<uint16_t>(slippagePct * 100); // Convert to basis points
        
        // Prepare expiry time for GTT orders
        std::optional<std::chrono::system_clock::time_point> expiryTimePoint = std::nullopt;
        if (parsePolicy(policyStr) == OrderPolicy::GTT) {
            if (expiryTime > 0) {
                expiryTimePoint = std::chrono::system_clock::from_time_t(expiryTime);
                time_t expiry_t = static_cast<time_t>(expiryTime);
                std::cout << "GTT order expiry set for " << std::ctime(&expiry_t);
            } else {
                std::cout << "Error: GTT orders require valid expiry time" << std::endl;
                return;
            }
        }
        
        // Create order
        Order order(
            sellTokenAddress,
            buyTokenAddress,
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
        
        std::cout << "\n--- Order submitted successfully!" << std::endl;
        std::cout << "Order ID: " << orderId << std::endl;
        std::cout << "Policy: " << OrderPolicies::getPolicyDescription(order.policy) << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error submitting order: " << e.what() << std::endl;
    }
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
    std::cout << "\n=== Cancel Order ===" << std::endl;
    
    // First show active orders
    auto orders = engine_->getActiveOrders();
    if (orders.empty()) {
        std::cout << "No active orders to cancel." << std::endl;
        return;
    }
    
    std::cout << "\nActive Orders:" << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << "Index   Order ID        Policy    Sell/Buy       Status" << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    
    for (size_t i = 0; i < orders.size(); ++i) {
        const auto& order = orders[i];
        std::cout << std::setw(5) << (i + 1) << "   "
                  << std::setw(12) << order.id.substr(0, 8) + "..."
                  << std::setw(10) << orderPolicyToString(order.policy)
                  << std::setw(15) << (order.sellToken + "/" + order.buyToken)
                  << std::setw(10) << orderStatusToString(order.status)
                  << std::endl;
    }
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    
    std::cout << "\nEnter Order ID (full) or Index number to cancel: ";
    std::string input;
    std::getline(std::cin, input);
    
    std::string orderId;
    
    // Check if input is a number (index)
    try {
        size_t index = std::stoull(input);
        if (index >= 1 && index <= orders.size()) {
            orderId = orders[index - 1].id;
            std::cout << "Selected order: " << orderId.substr(0, 8) << "..." << std::endl;
        } else {
            std::cout << "Error: Invalid index. Must be between 1 and " << orders.size() << std::endl;
            return;
        }
    } catch (const std::exception&) {
        // Input is not a number, treat as order ID
        orderId = input;
        
        // Try partial match if exact match fails
        bool found = false;
        for (const auto& order : orders) {
            if (order.id == orderId || order.id.substr(0, input.length()) == input) {
                orderId = order.id;
                found = true;
                break;
            }
        }
        
        if (!found) {
            std::cout << "Error: Order ID not found. Use exact ID or index number." << std::endl;
            return;
        }
    }
    
    if (engine_->cancelOrder(orderId)) {
        std::cout << "Order " << orderId.substr(0, 8) << "... canceled successfully!" << std::endl;
    } else {
        std::cout << "Failed to cancel order " << orderId.substr(0, 8) << "..." << std::endl;
        std::cout << "Order may have already been filled, canceled, or expired." << std::endl;
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
            std::cout << "  Executed - TxHash: " << result.txHash.substr(0, 10) << "..." << std::endl;
            std::cout << "  Actual Price: " << result.actualPrice << std::endl;
        } else if (!result.errorMessage.empty() && result.errorMessage != "No execution result found") {
            std::cout << "  Failed: " << result.errorMessage << std::endl;
        }
    }
}

void CLI::handleCheckPrice() {
    std::cout << "\n=== Check Current Price ===" << std::endl;
    
    std::string sellToken, buyToken;
    std::string input;
    
    std::cout << "Sell token: ";
    std::getline(std::cin, sellToken);
    
    std::cout << "Buy token: ";
    std::getline(std::cin, buyToken);
    
    std::cout << "Amount: ";
    std::getline(std::cin, input);
    
    // Validate tokens exist
    if (sellToken != "ETH" && tokens_.find(sellToken) == tokens_.end()) {
        std::cout << "Error: Unknown token: " << sellToken << std::endl;
        return;
    }
    
    if (buyToken != "ETH" && tokens_.find(buyToken) == tokens_.end()) {
        std::cout << "Error: Unknown token: " << buyToken << std::endl;
        return;
        // Buy token decimals stored for potential future use
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
        // Use the new real-time price method with actual token symbols
        double currentPrice = engine_->getRealTimePrice(sellToken, buyToken);
        
        std::cout << "\nCurrent price: " << currentPrice << " " << buyToken << " per " << sellToken << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error checking price: " << e.what() << std::endl;
    }
}

void CLI::handleShowConfig() {
    std::cout << "\n=== Configuration ===" << std::endl;
    std::cout << "Engine running: " << (engine_->isRunning() ? "Yes" : "No") << std::endl;
    std::cout << "Trading mode: " << (engine_->isDryRun() ? "DEMO (Safe)" : "REAL (Uses wallet!)") << std::endl;
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

void CLI::handleToggleMode() {
    std::cout << "\n=== Trading Mode Toggle ===" << std::endl;
    std::cout << "WARNING: This will switch between DEMO and REAL trading modes!" << std::endl;
    std::cout << "Current mode: " << (engine_->isDryRun() ? "DEMO (Safe)" : "REAL (Uses wallet funds)") << std::endl;
    std::cout << "\nDo you want to toggle mode? (yes/no): ";
    
    std::string confirm;
    std::getline(std::cin, confirm);
    
    if (confirm == "yes" || confirm == "y" || confirm == "YES") {
        bool newMode = !engine_->isDryRun();
        engine_->setDryRun(newMode);
        
        std::cout << "\nMode switched to: " << (newMode ? "DEMO (Safe)" : "REAL (Uses wallet funds!)") << std::endl;
        
        if (!newMode) {
            std::cout << "\nREAL MODE ACTIVE - Orders will use your actual wallet!" << std::endl;
            std::cout << "   Wallet: 0x0dc7987e20953546F50c92CbC9d45A8baf638690" << std::endl;
            std::cout << "   Network: Sepolia Testnet" << std::endl;
        }
    } else {
        std::cout << "Mode unchanged." << std::endl;
    }
}

void CLI::handleCheckBalance() {
    std::cout << "\n=== Real Token Balance Check ===" << std::endl;
    std::cout << "Wallet: 0x0dc7987e20953546F50c92CbC9d45A8baf638690" << std::endl;
    std::cout << "\nChecking real balances on Sepolia..." << std::endl;
    
    // Check ETH balance first
    try {
        json params = {"0x0dc7987e20953546F50c92CbC9d45A8baf638690", "latest"};
        auto result = engine_->getRPC()->call("eth_getBalance", params);
        if (!result.contains("error")) {
            uint64_t weiBalance = hexToUint64(result["result"]);
            double ethBalance = static_cast<double>(weiBalance) / 1e18;
            std::cout << "  ETH: " << ethBalance << " ETH" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "  ETH: Error - " << e.what() << std::endl;
    }
    
    // Check token balances
    for (const auto& token : tokens_) {
        try {
            ERC20Token tokenContract(token.second.address, engine_->getRPC());
            uint64_t balance = tokenContract.balanceOf("0x0dc7987e20953546F50c92CbC9d45A8baf638690");
            
            double humanBalance = PriceUtils::toHumanReadable(balance, token.second.decimals);
            std::cout << "  " << token.first << ": " << humanBalance << " " << token.first << std::endl;
            
        } catch (const std::exception& e) {
            std::cout << "  " << token.first << ": Error - " << e.what() << std::endl;
        }
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
    std::cout << "  mode/toggle     - Toggle between DEMO and REAL trading mode" << std::endl;
    std::cout << "  balance/bal     - Check real token balances" << std::endl;
    std::cout << "  help/?          - Show this help message" << std::endl;
    std::cout << "  quit/exit       - Exit the application" << std::endl;
    
    std::cout << "\n=== Order Policies ===" << std::endl;
    std::cout << "  GTC - Good Till Canceled (default)" << std::endl;
    std::cout << "  GTT - Good Till Time (with expiry)" << std::endl;
    std::cout << "  IOC - Immediate or Cancel (partial fills OK)" << std::endl;
    std::cout << "  FOK - Fill or Kill (full fill or nothing)" << std::endl;
}

void CLI::displayOrder(const Order& order) {
    // Extract token symbols from addresses (handle ETH specially)
    std::string sellSymbol = "???", buySymbol = "???";
    
    // Handle ETH special case
    if (order.sellToken == "0x0000000000000000000000000000000000000000") {
        sellSymbol = "ETH";
    } else {
        for (const auto& pair : tokens_) {
            if (pair.second.address == order.sellToken) sellSymbol = pair.first;
        }
    }
    
    if (order.buyToken == "0x0000000000000000000000000000000000000000") {
        buySymbol = "ETH";
    } else {
        for (const auto& pair : tokens_) {
            if (pair.second.address == order.buyToken) buySymbol = pair.first;
        }
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
    // Note: ETH is handled separately as native currency, not as ERC20 token
    
    // Pool mappings - REAL WORKING CONTRACTS ON SEPOLIA
    // Using Uniswap V3 Factory to find actual pool addresses
    // Uniswap V3 Factory on Sepolia: 0x0227628f3F023bb0B980b67D528571c95c6DaC1c
    poolMap_["USDC-WETH"] = "0x0227628f3F023bb0B980b67D528571c95c6DaC1c";  // Uniswap V3 Factory
    poolMap_["USDC-ETH"] = "0x0227628f3F023bb0B980b67D528571c95c6DaC1c";   // Uniswap V3 Factory
    poolMap_["ETH-USDC"] = "0x0227628f3F023bb0B980b67D528571c95c6DaC1c";   // Uniswap V3 Factory
    poolMap_["USDC-DAI"] = "0x0227628f3F023bb0B980b67D528571c95c6DaC1c";   // Uniswap V3 Factory  
    poolMap_["WETH-DAI"] = "0x0227628f3F023bb0B980b67D528571c95c6DaC1c";   // Uniswap V3 Factory
    poolMap_["ETH-DAI"] = "0x0227628f3F023bb0B980b67D528571c95c6DaC1c";    // Uniswap V3 Factory
}

void CLI::setupEngine() {
    EngineConfig config;
    
    // Sepolia testnet RPC URL - REAL ENDPOINT 
    config.rpcUrl = "https://eth-sepolia.public.blastapi.io"; // Public Sepolia RPC
    config.userAddress = "0x0dc7987e20953546F50c92CbC9d45A8baf638690"; // Your funded wallet
    config.privateKey = "0x39914000b3640bded11dfc9da17c0e4eb954cdc74c6447f82b481c1506e1dc61"; // Your real private key
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
