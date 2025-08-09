#include "PriceUtils.hpp"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace CurveTrading {

double PriceUtils::toHumanUnits(uint64_t amount, uint8_t decimals) {
    if (decimals > 18) {
        throw std::invalid_argument("Decimals cannot exceed 18");
    }
    return static_cast<double>(amount) / static_cast<double>(POWER_OF_10[decimals]);
}

uint64_t PriceUtils::toBaseUnits(double amount, uint8_t decimals) {
    if (decimals > 18) {
        throw std::invalid_argument("Decimals cannot exceed 18");
    }
    if (amount < 0) {
        throw std::invalid_argument("Amount cannot be negative");
    }
    return static_cast<uint64_t>(amount * static_cast<double>(POWER_OF_10[decimals]));
}

double PriceUtils::calculatePrice(uint64_t amountOut, uint64_t amountIn,
                                uint8_t decimalsOut, uint8_t decimalsIn) {
    if (amountIn == 0) {
        throw std::invalid_argument("Amount in cannot be zero");
    }
    
    double humanOut = toHumanUnits(amountOut, decimalsOut);
    double humanIn = toHumanUnits(amountIn, decimalsIn);
    
    return humanOut / humanIn;
}

double PriceUtils::normalizePrice(uint64_t dy, uint64_t dx,
                                uint8_t decimalsY, uint8_t decimalsX) {
    return calculatePrice(dy, dx, decimalsY, decimalsX);
}

uint64_t PriceUtils::applySlippage(uint64_t amount, uint16_t slippageBps) {
    if (slippageBps > 10000) {
        throw std::invalid_argument("Slippage cannot exceed 100%");
    }
    
    // Calculate: amount * (1 - slippageBps/10000)
    uint64_t slippageAmount = (amount * slippageBps) / 10000;
    return amount - slippageAmount;
}

uint64_t PriceUtils::calculateMinOut(uint64_t expectedOut, uint16_t slippageBps) {
    return applySlippage(expectedOut, slippageBps);
}

double PriceUtils::calculateSlippagePercent(uint64_t expected, uint64_t actual) {
    if (expected == 0) return 0.0;
    if (actual > expected) return 0.0; // No slippage if we got more than expected
    
    double slippage = (static_cast<double>(expected - actual) / static_cast<double>(expected)) * 100.0;
    return slippage;
}

bool PriceUtils::isPriceTarget(double currentPrice, double targetPrice, double tolerance) {
    return currentPrice >= (targetPrice - tolerance);
}

bool PriceUtils::isWithinSlippage(uint64_t expected, uint64_t actual, uint16_t maxSlippageBps) {
    uint64_t minAcceptable = calculateMinOut(expected, maxSlippageBps);
    return actual >= minAcceptable;
}

std::string PriceUtils::formatPrice(double price, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << price;
    return oss.str();
}

std::string PriceUtils::formatAmount(uint64_t amount, uint8_t decimals, int precision) {
    double human = toHumanUnits(amount, decimals);
    return formatPrice(human, precision);
}

std::string PriceUtils::formatPercentage(uint16_t bps) {
    double percent = static_cast<double>(bps) / 100.0;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << percent << "%";
    return oss.str();
}

} // namespace CurveTrading
