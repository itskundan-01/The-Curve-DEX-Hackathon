#pragma once

#include <cstdint>
#include <string>

namespace CurveTrading {

class PriceUtils {
public:
    // Decimal conversions
    static double toHumanUnits(uint64_t amount, uint8_t decimals);
    static uint64_t toBaseUnits(double amount, uint8_t decimals);
    
    // Price calculations
    static double calculatePrice(uint64_t amountOut, uint64_t amountIn, 
                               uint8_t decimalsOut, uint8_t decimalsIn);
    static double normalizePrice(uint64_t dy, uint64_t dx, 
                               uint8_t decimalsY, uint8_t decimalsX);
    
    // Slippage calculations
    static uint64_t applySlippage(uint64_t amount, uint16_t slippageBps);
    static uint64_t calculateMinOut(uint64_t expectedOut, uint16_t slippageBps);
    static double calculateSlippagePercent(uint64_t expected, uint64_t actual);
    
    // Validation
    static bool isPriceTarget(double currentPrice, double targetPrice, double tolerance = 0.0001);
    static bool isWithinSlippage(uint64_t expected, uint64_t actual, uint16_t maxSlippageBps);
    
    // Formatting
    static std::string formatPrice(double price, int precision = 6);
    static std::string formatAmount(uint64_t amount, uint8_t decimals, int precision = 4);
    static std::string formatPercentage(uint16_t bps);
    
private:
    static constexpr uint64_t POWER_OF_10[] = {
        1ULL, 10ULL, 100ULL, 1000ULL, 10000ULL, 100000ULL, 1000000ULL,
        10000000ULL, 100000000ULL, 1000000000ULL, 10000000000ULL,
        100000000000ULL, 1000000000000ULL, 10000000000000ULL,
        100000000000000ULL, 1000000000000000ULL, 10000000000000000ULL,
        100000000000000000ULL, 1000000000000000000ULL
    };
};

} // namespace CurveTrading
