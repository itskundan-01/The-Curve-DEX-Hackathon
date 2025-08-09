#include "../src/utils/PriceUtils.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace CurveTrading;

void test_decimal_conversions() {
    std::cout << "Testing decimal conversions..." << std::endl;
    
    // Test 18 decimals (like ETH)
    uint64_t oneEth = 1000000000000000000ULL; // 1e18
    double humanEth = PriceUtils::toHumanUnits(oneEth, 18);
    assert(std::abs(humanEth - 1.0) < 0.0001);
    
    uint64_t backToBase = PriceUtils::toBaseUnits(humanEth, 18);
    assert(backToBase == oneEth);
    
    // Test 6 decimals (like USDC)
    uint64_t oneUsdc = 1000000ULL; // 1e6
    double humanUsdc = PriceUtils::toHumanUnits(oneUsdc, 6);
    assert(std::abs(humanUsdc - 1.0) < 0.0001);
    
    backToBase = PriceUtils::toBaseUnits(humanUsdc, 6);
    assert(backToBase == oneUsdc);
    
    std::cout << "✓ Decimal conversion test passed" << std::endl;
}

void test_price_calculations() {
    std::cout << "Testing price calculations..." << std::endl;
    
    // 1 ETH (18 decimals) = 2000 USDC (6 decimals)
    uint64_t ethAmount = 1000000000000000000ULL; // 1 ETH
    uint64_t usdcAmount = 2000000000ULL; // 2000 USDC
    
    double price = PriceUtils::calculatePrice(usdcAmount, ethAmount, 6, 18);
    assert(std::abs(price - 2000.0) < 0.1);
    
    // Normalized price should be same
    double normalizedPrice = PriceUtils::normalizePrice(usdcAmount, ethAmount, 6, 18);
    assert(std::abs(normalizedPrice - 2000.0) < 0.1);
    
    std::cout << "✓ Price calculation test passed" << std::endl;
}

void test_slippage_calculations() {
    std::cout << "Testing slippage calculations..." << std::endl;
    
    uint64_t amount = 1000000; // 1 USDC
    uint16_t slippageBps = 50; // 0.5%
    
    uint64_t withSlippage = PriceUtils::applySlippage(amount, slippageBps);
    uint64_t expected = 995000; // 0.995 USDC
    assert(withSlippage == expected);
    
    // Test min out calculation
    uint64_t minOut = PriceUtils::calculateMinOut(amount, slippageBps);
    assert(minOut == expected);
    
    // Test slippage percentage calculation
    double slippagePercent = PriceUtils::calculateSlippagePercent(1000000, 995000);
    assert(std::abs(slippagePercent - 0.5) < 0.01);
    
    std::cout << "✓ Slippage calculation test passed" << std::endl;
}

void test_price_target_validation() {
    std::cout << "Testing price target validation..." << std::endl;
    
    double currentPrice = 1.05;
    double targetPrice = 1.04;
    
    // Current price meets target
    assert(PriceUtils::isPriceTarget(currentPrice, targetPrice));
    
    // Current price below target
    currentPrice = 1.03;
    assert(!PriceUtils::isPriceTarget(currentPrice, targetPrice));
    
    // Test slippage validation
    uint64_t expected = 1000000;
    uint64_t actual = 995000;
    uint16_t maxSlippage = 100; // 1%
    
    assert(PriceUtils::isWithinSlippage(expected, actual, maxSlippage));
    
    // Too much slippage
    actual = 980000; // 2% slippage
    assert(!PriceUtils::isWithinSlippage(expected, actual, maxSlippage));
    
    std::cout << "✓ Price target validation test passed" << std::endl;
}

void test_formatting() {
    std::cout << "Testing formatting..." << std::endl;
    
    // Price formatting
    std::string priceStr = PriceUtils::formatPrice(1.234567, 4);
    assert(priceStr == "1.2346");
    
    // Amount formatting
    std::string amountStr = PriceUtils::formatAmount(1234567000000000000ULL, 18, 2);
    assert(amountStr == "1.23");
    
    // Percentage formatting
    std::string percentStr = PriceUtils::formatPercentage(50);
    assert(percentStr == "0.50%");
    
    std::cout << "✓ Formatting test passed" << std::endl;
}

int main() {
    std::cout << "Running PriceUtils tests..." << std::endl;
    
    try {
        test_decimal_conversions();
        test_price_calculations();
        test_slippage_calculations();
        test_price_target_validation();
        test_formatting();
        
        std::cout << "\n✅ All PriceUtils tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
