#include "PriceUtils.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>

namespace CurveTrading {

double PriceUtils::toHumanReadable(uint64_t amount, uint8_t decimals) {
    return static_cast<double>(amount) / std::pow(10.0, decimals);
}

uint64_t PriceUtils::fromHumanReadable(double amount, uint8_t decimals) {
    return static_cast<uint64_t>(amount * std::pow(10.0, decimals));
}

double PriceUtils::calculatePrice(uint64_t inputAmount, uint64_t outputAmount, 
                                 uint8_t inputDecimals, uint8_t outputDecimals) {
    if (inputAmount == 0) return 0.0;
    
    double inputHuman = toHumanReadable(inputAmount, inputDecimals);
    double outputHuman = toHumanReadable(outputAmount, outputDecimals);
    
    return outputHuman / inputHuman;
}

uint64_t PriceUtils::applySlippage(uint64_t amount, uint16_t slippageBps) {
    // slippageBps is in basis points (1 = 0.01%)
    // For minimum output, we reduce by slippage
    double slippageMultiplier = (10000.0 - slippageBps) / 10000.0;
    return static_cast<uint64_t>(amount * slippageMultiplier);
}

uint64_t PriceUtils::calculateMinOutput(uint64_t expectedOutput, uint16_t slippageBps) {
    return applySlippage(expectedOutput, slippageBps);
}

bool PriceUtils::isPriceBetter(double current, double target, double tolerancePct) {
    if (tolerancePct > 0.0) {
        double threshold = target * (1.0 - tolerancePct / 100.0);
        return current >= threshold;
    }
    return current >= target;
}

std::string PriceUtils::formatAmount(uint64_t amount, uint8_t decimals, uint8_t displayDecimals) {
    double human = toHumanReadable(amount, decimals);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(displayDecimals) << human;
    return ss.str();
}

std::string PriceUtils::formatPrice(double price, uint8_t displayDecimals) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(displayDecimals) << price;
    return ss.str();
}

} // namespace CurveTrading
