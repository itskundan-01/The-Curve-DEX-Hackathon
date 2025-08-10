#pragma once

#include <cstdint>
#include <string>

namespace CurveTrading {

class PriceUtils {
public:
    // Convert between different decimal representations
    static double toHumanReadable(uint64_t amount, uint8_t decimals);
    static uint64_t fromHumanReadable(double amount, uint8_t decimals);
    
    // Calculate price from get_dy result
    static double calculatePrice(uint64_t inputAmount, uint64_t outputAmount, 
                                uint8_t inputDecimals, uint8_t outputDecimals);
    
    // Apply slippage to amount
    static uint64_t applySlippage(uint64_t amount, uint16_t slippageBps);
    
    // Calculate minimum output with slippage protection
    static uint64_t calculateMinOutput(uint64_t expectedOutput, uint16_t slippageBps);
    
    // Price comparison with tolerance
    static bool isPriceBetter(double current, double target, double tolerancePct = 0.0);
    
    // Format amounts for display
    static std::string formatAmount(uint64_t amount, uint8_t decimals, uint8_t displayDecimals = 6);
    static std::string formatPrice(double price, uint8_t displayDecimals = 6);
};

} // namespace CurveTrading
