#include "../src/core/OrderPolicies.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace CurveTrading;

// Helper to create a mock quote
CurveQuote createMockQuote(uint64_t dy, bool valid = true) {
    CurveQuote quote;
    quote.dy = dy;
    quote.valid = valid;
    quote.virtualPrice = 1000000000000000000ULL; // 1.0
    quote.impliedPrice = 0.0; // Will be calculated by policy
    return quote;
}

void test_policy_descriptions() {
    std::cout << "Testing policy descriptions..." << std::endl;
    
    std::string gtcDesc = OrderPolicies::getPolicyDescription(OrderPolicy::GTC);
    assert(gtcDesc.find("Good Till Canceled") != std::string::npos);
    
    std::string iocDesc = OrderPolicies::getPolicyDescription(OrderPolicy::IOC);
    assert(iocDesc.find("Immediate or Cancel") != std::string::npos);
    
    std::string fokDesc = OrderPolicies::getPolicyDescription(OrderPolicy::FOK);
    assert(fokDesc.find("Fill or Kill") != std::string::npos);
    
    std::cout << "✓ Policy descriptions test passed" << std::endl;
}

void test_quote_acceptability() {
    std::cout << "Testing quote acceptability..." << std::endl;
    
    // Order: sell 1 USDC (6 decimals) for 0.0005 ETH (18 decimals)
    // Target price: 0.0005 ETH per USDC
    Order order("0x123", "0x456", "0x789", 0, 1, 1000000, 0.0005, OrderPolicy::GTC);
    
    // Quote that meets target: 500000000000000 wei (0.0005 ETH)
    CurveQuote goodQuote = createMockQuote(500000000000000ULL);
    assert(OrderPolicies::isQuoteAcceptable(order, goodQuote, 6, 18));
    
    // Quote that doesn't meet target: 400000000000000 wei (0.0004 ETH)
    CurveQuote badQuote = createMockQuote(400000000000000ULL);
    assert(!OrderPolicies::isQuoteAcceptable(order, badQuote, 6, 18));
    
    // Invalid quote
    CurveQuote invalidQuote = createMockQuote(500000000000000ULL, false);
    assert(!OrderPolicies::isQuoteAcceptable(order, invalidQuote, 6, 18));
    
    std::cout << "✓ Quote acceptability test passed" << std::endl;
}

void test_gtc_policy() {
    std::cout << "Testing GTC policy..." << std::endl;
    
    Order order("0x123", "0x456", "0x789", 0, 1, 1000000, 0.0005, OrderPolicy::GTC);
    
    // Good quote - should execute
    CurveQuote goodQuote = createMockQuote(500000000000000ULL);
    PolicyEvaluation eval = OrderPolicies::evaluateGTC(order, goodQuote, 6, 18);
    assert(eval.result == PolicyResult::EXECUTE);
    assert(eval.shouldExecute);
    
    // Bad quote - should wait
    CurveQuote badQuote = createMockQuote(400000000000000ULL);
    eval = OrderPolicies::evaluateGTC(order, badQuote, 6, 18);
    assert(eval.result == PolicyResult::WAIT);
    assert(!eval.shouldExecute);
    
    // Invalid quote - should wait
    CurveQuote invalidQuote = createMockQuote(500000000000000ULL, false);
    eval = OrderPolicies::evaluateGTC(order, invalidQuote, 6, 18);
    assert(eval.result == PolicyResult::WAIT);
    assert(!eval.shouldExecute);
    
    std::cout << "✓ GTC policy test passed" << std::endl;
}

void test_ioc_policy() {
    std::cout << "Testing IOC policy..." << std::endl;
    
    Order order("0x123", "0x456", "0x789", 0, 1, 1000000, 0.0005, OrderPolicy::IOC);
    
    // Good quote - should execute
    CurveQuote goodQuote = createMockQuote(500000000000000ULL);
    PolicyEvaluation eval = OrderPolicies::evaluateIOC(order, goodQuote, 6, 18);
    assert(eval.result == PolicyResult::EXECUTE);
    assert(eval.shouldExecute);
    
    // Bad quote - should cancel
    CurveQuote badQuote = createMockQuote(400000000000000ULL);
    eval = OrderPolicies::evaluateIOC(order, badQuote, 6, 18);
    assert(eval.result == PolicyResult::CANCEL);
    assert(!eval.shouldExecute);
    
    // Invalid quote - should cancel
    CurveQuote invalidQuote = createMockQuote(500000000000000ULL, false);
    eval = OrderPolicies::evaluateIOC(order, invalidQuote, 6, 18);
    assert(eval.result == PolicyResult::CANCEL);
    assert(!eval.shouldExecute);
    
    std::cout << "✓ IOC policy test passed" << std::endl;
}

void test_fok_policy() {
    std::cout << "Testing FOK policy..." << std::endl;
    
    Order order("0x123", "0x456", "0x789", 0, 1, 1000000, 0.0005, OrderPolicy::FOK);
    
    // Good quote - should execute
    CurveQuote goodQuote = createMockQuote(500000000000000ULL);
    PolicyEvaluation eval = OrderPolicies::evaluateFOK(order, goodQuote, 6, 18);
    assert(eval.result == PolicyResult::EXECUTE);
    assert(eval.shouldExecute);
    
    // Bad quote - should cancel
    CurveQuote badQuote = createMockQuote(400000000000000ULL);
    eval = OrderPolicies::evaluateFOK(order, badQuote, 6, 18);
    assert(eval.result == PolicyResult::CANCEL);
    assert(!eval.shouldExecute);
    
    // Invalid quote - should cancel
    CurveQuote invalidQuote = createMockQuote(500000000000000ULL, false);
    eval = OrderPolicies::evaluateFOK(order, invalidQuote, 6, 18);
    assert(eval.result == PolicyResult::CANCEL);
    assert(!eval.shouldExecute);
    
    std::cout << "✓ FOK policy test passed" << std::endl;
}

void test_expired_order() {
    std::cout << "Testing expired order handling..." << std::endl;
    
    Order order("0x123", "0x456", "0x789", 0, 1, 1000000, 0.0005, OrderPolicy::GTC);
    
    // Set expiry to past
    auto past = std::chrono::system_clock::now() - std::chrono::hours(1);
    order.expiry = past;
    
    CurveQuote goodQuote = createMockQuote(500000000000000ULL);
    PolicyEvaluation eval = OrderPolicies::evaluate(order, goodQuote, 6, 18);
    assert(eval.result == PolicyResult::EXPIRE);
    assert(!eval.shouldExecute);
    
    std::cout << "✓ Expired order test passed" << std::endl;
}

void test_min_dy_calculation() {
    std::cout << "Testing minDy calculation..." << std::endl;
    
    Order order("0x123", "0x456", "0x789", 0, 1, 1000000, 0.0005, OrderPolicy::GTC, 50); // 0.5% slippage
    
    uint64_t dy = 1000000; // 1 unit
    uint64_t minDy = OrderPolicies::calculateMinDy(order, dy);
    uint64_t expected = 995000; // 0.995 units (0.5% slippage)
    
    assert(minDy == expected);
    
    std::cout << "✓ MinDy calculation test passed" << std::endl;
}

int main() {
    std::cout << "Running OrderPolicies tests..." << std::endl;
    
    try {
        test_policy_descriptions();
        test_quote_acceptability();
        test_gtc_policy();
        test_ioc_policy();
        test_fok_policy();
        test_expired_order();
        test_min_dy_calculation();
        
        std::cout << "\n✅ All OrderPolicies tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
