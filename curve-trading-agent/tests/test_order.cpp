#include "../src/core/Order.hpp"
#include "../src/utils/PriceUtils.hpp"
#include "../src/core/OrderPolicies.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace CurveTrading;

void test_order_creation() {
    std::cout << "Testing order creation..." << std::endl;
    
    Order order("0x123", "0x456", "0x789", 0, 1, 1000000, 1.05, OrderPolicy::GTC, 50);
    
    assert(order.sellToken == "0x123");
    assert(order.buyToken == "0x456");
    assert(order.pool == "0x789");
    assert(order.i == 0);
    assert(order.j == 1);
    assert(order.amountIn == 1000000);
    assert(std::abs(order.targetPrice - 1.05) < 0.0001);
    assert(order.policy == OrderPolicy::GTC);
    assert(order.maxSlippageBps == 50);
    assert(order.status == OrderStatus::NEW);
    assert(!order.id.empty());
    
    std::cout << "✓ Order creation test passed" << std::endl;
}

void test_order_validation() {
    std::cout << "Testing order validation..." << std::endl;
    
    // Valid order
    Order validOrder("0x123", "0x456", "0x789", 0, 1, 1000000, 1.05, OrderPolicy::GTC);
    assert(validOrder.isValid());
    
    // Invalid orders
    Order invalidSellToken("", "0x456", "0x789", 0, 1, 1000000, 1.05, OrderPolicy::GTC);
    assert(!invalidSellToken.isValid());
    
    Order invalidAmount("0x123", "0x456", "0x789", 0, 1, 0, 1.05, OrderPolicy::GTC);
    assert(!invalidAmount.isValid());
    
    Order invalidPrice("0x123", "0x456", "0x789", 0, 1, 1000000, 0.0, OrderPolicy::GTC);
    assert(!invalidPrice.isValid());
    
    Order sameIndices("0x123", "0x456", "0x789", 1, 1, 1000000, 1.05, OrderPolicy::GTC);
    assert(!sameIndices.isValid());
    
    std::cout << "✓ Order validation test passed" << std::endl;
}

void test_order_status_transitions() {
    std::cout << "Testing order status transitions..." << std::endl;
    
    Order order("0x123", "0x456", "0x789", 0, 1, 1000000, 1.05, OrderPolicy::GTC);
    
    assert(order.status == OrderStatus::NEW);
    assert(!order.canExecute());
    
    order.markWorking();
    assert(order.status == OrderStatus::WORKING);
    assert(order.canExecute());
    
    order.markFilled("0xabcdef");
    assert(order.status == OrderStatus::FILLED);
    assert(!order.canExecute());
    assert(order.txHash.has_value());
    assert(order.txHash.value() == "0xabcdef");
    
    std::cout << "✓ Order status transition test passed" << std::endl;
}

void test_order_serialization() {
    std::cout << "Testing order serialization..." << std::endl;
    
    Order original("0x123", "0x456", "0x789", 0, 1, 1000000, 1.05, OrderPolicy::IOC, 100);
    original.note = "Test order";
    original.markWorking();
    
    std::string json = original.toJson();
    assert(!json.empty());
    
    // Basic JSON structure check
    assert(json.find("\"id\"") != std::string::npos);
    assert(json.find("\"sellToken\"") != std::string::npos);
    assert(json.find("\"policy\":\"IOC\"") != std::string::npos);
    assert(json.find("\"status\":\"WORKING\"") != std::string::npos);
    
    std::cout << "✓ Order serialization test passed" << std::endl;
}

int main() {
    std::cout << "Running Order tests..." << std::endl;
    
    try {
        test_order_creation();
        test_order_validation();
        test_order_status_transitions();
        test_order_serialization();
        
        std::cout << "\n✅ All Order tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
