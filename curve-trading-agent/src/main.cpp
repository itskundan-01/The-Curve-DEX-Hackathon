#include <iostream>
#include <string>
#include <vector>
#include "core/Order.hpp"
#include "utils/PriceUtils.hpp"
#include "core/OrderPolicies.hpp"

using namespace CurveTrading;

void printUsage(const char* programName) {
    std::cout << "Curve Trading Agent - Limit Order Engine\n";
    std::cout << "Usage: " << programName << " [command] [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  interactive          Start interactive CLI mode\n";
    std::cout << "  submit               Submit a new limit order\n";
    std::cout << "  list                 List all orders\n";
    std::cout << "  cancel <order-id>    Cancel an order\n";
    std::cout << "  status <order-id>    Show order status\n";
    std::cout << "  engine               Start/stop the trading engine\n";
    std::cout << "  monitor              Monitor prices and execute orders\n";
    std::cout << "  stats                Show engine statistics\n";
    std::cout << "  demo                 Run core library demo\n";
    std::cout << "  help                 Show this help\n\n";
}

void runCoreDemo() {
    std::cout << "Curve Trading Agent - Core Library Demo" << std::endl;
    std::cout << "=======================================" << std::endl;
    
    // Create a sample order
    Order order("0xA0b86a33E6c8C2c95A5dBB03B9f54f8ED0D0e97e", // USDC
                "0xC02aaA39b223FE8D0A0e5C4F27eAD9083C756Cc2", // WETH  
                "0xbEbc44782C7dB0a1A60Cb6fe97d0b483032FF1C7", // 3Pool
                1, 2, 1000000, 0.0005, OrderPolicy::GTC, 50);
    
    std::cout << "Created order:" << std::endl;
    std::cout << "- ID: " << order.id << std::endl;
    std::cout << "- Sell Token: " << order.sellToken << std::endl;
    std::cout << "- Buy Token: " << order.buyToken << std::endl;
    std::cout << "- Amount In: " << PriceUtils::formatAmount(order.amountIn, 6) << " USDC" << std::endl;
    std::cout << "- Target Price: " << PriceUtils::formatPrice(order.targetPrice) << " ETH per USDC" << std::endl;
    std::cout << "- Policy: " << orderPolicyToString(order.policy) << std::endl;
    std::cout << "- Max Slippage: " << PriceUtils::formatPercentage(order.maxSlippageBps) << std::endl;
    std::cout << "- Status: " << orderStatusToString(order.status) << std::endl;
    std::cout << "- Valid: " << (order.isValid() ? "Yes" : "No") << std::endl;
    
    // Test price calculations
    std::cout << "\nPrice Calculation Examples:" << std::endl;
    uint64_t ethAmount = 500000000000000ULL; // 0.0005 ETH (18 decimals)
    uint64_t usdcAmount = 1000000ULL; // 1 USDC (6 decimals)
    
    double price = PriceUtils::calculatePrice(ethAmount, usdcAmount, 18, 6);
    std::cout << "- Price of 1 USDC = " << PriceUtils::formatPrice(price, 6) << " ETH" << std::endl;
    
    // Test slippage
    uint64_t minOut = PriceUtils::calculateMinOut(ethAmount, order.maxSlippageBps);
    std::cout << "- Min ETH out (with " << PriceUtils::formatPercentage(order.maxSlippageBps) << " slippage): " 
              << PriceUtils::formatAmount(minOut, 18, 6) << " ETH" << std::endl;
    
    // Test policy description
    std::cout << "\nPolicy Description:" << std::endl;
    std::cout << "- " << OrderPolicies::getPolicyDescription(order.policy) << std::endl;
    
    std::cout << "\n✅ Core library test completed successfully!" << std::endl;
}

void runInteractiveMode() {
    std::cout << "🚀 Curve Trading Agent - Interactive Mode\n";
    std::cout << "=========================================\n\n";
    std::cout << "This demonstrates the live trading interface expected by the hackathon.\n";
    std::cout << "In the full implementation, this connects to:\n";
    std::cout << "- Ethereum blockchain (via RPC)\n";
    std::cout << "- Curve Finance pools (for quotes and swaps)\n";
    std::cout << "- Order management engine\n";
    std::cout << "- Real-time price monitoring\n\n";
    
    std::cout << "Available commands:\n";
    std::cout << "  submit    - Submit a new limit order\n";
    std::cout << "  list      - List all orders\n";  
    std::cout << "  cancel    - Cancel an order\n";
    std::cout << "  status    - Check order status\n";
    std::cout << "  engine    - Start/stop trading engine\n";
    std::cout << "  monitor   - Monitor prices (live mode)\n";
    std::cout << "  quit      - Exit program\n\n";
    
    std::string command;
    while (true) {
        std::cout << "trading-agent> ";
        std::getline(std::cin, command);
        
        if (command == "quit" || command == "exit") {
            std::cout << "Goodbye! 👋\n";
            break;
        } else if (command == "demo") {
            runCoreDemo();
        } else if (command == "submit") {
            std::cout << "📝 [DEMO] Submitting limit order...\n";
            std::cout << "   → Real implementation: Create order via LimitOrderEngine\n";
            std::cout << "   → Real implementation: Store in order storage\n";
            std::cout << "   → Real implementation: Start monitoring price targets\n";
        } else if (command == "list") {
            std::cout << "📋 [DEMO] Active orders:\n";
            std::cout << "   → Real implementation: Query order storage\n";
            std::cout << "   → Real implementation: Show order status and progress\n";
        } else if (command == "engine") {
            std::cout << "⚙️  [DEMO] Starting trading engine...\n";
            std::cout << "   → Real implementation: Start price monitoring loop\n";
            std::cout << "   → Real implementation: Connect to Curve pools\n";
            std::cout << "   → Real implementation: Execute orders when targets met\n";
        } else if (command == "monitor") {
            std::cout << "👀 [DEMO] Live monitoring mode:\n";
            std::cout << "   → Real implementation: Poll Curve get_dy() for quotes\n";
            std::cout << "   → Real implementation: Compare with order targets\n";
            std::cout << "   → Real implementation: Execute swaps when profitable\n";
            std::cout << "   → Real implementation: Log all activities\n";
        } else if (command.empty()) {
            continue;
        } else {
            std::cout << "Unknown command: " << command << ". Available: submit, list, cancel, engine, monitor, quit\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        runCoreDemo();
        return 0;
    }
    
    std::string command = argv[1];
    
    if (command == "help" || command == "--help" || command == "-h") {
        printUsage(argv[0]);
    } else if (command == "demo") {
        runCoreDemo();
    } else if (command == "interactive" || command == "-i") {
        runInteractiveMode();
    } else if (command == "submit") {
        std::cout << "📝 [CLI] Submit order command\n";
        std::cout << "Real implementation: Parse args and submit to engine\n";
    } else if (command == "list") {
        std::cout << "📋 [CLI] List orders command\n";
        std::cout << "Real implementation: Query storage and display orders\n";
    } else if (command == "engine") {
        std::cout << "⚙️  [CLI] Engine control command\n";
        std::cout << "Real implementation: Start/stop trading engine\n";
    } else {
        std::cout << "Unknown command: " << command << "\n";
        printUsage(argv[0]);
        return 1;
    }
    
    return 0;
}
