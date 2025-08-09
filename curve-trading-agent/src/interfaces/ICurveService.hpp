#pragma once

#include <string>
#include <cstdint>

namespace CurveTrading {

struct CurveQuote {
    uint64_t dy;           // Amount out
    double impliedPrice;   // Price implied by this quote (normalized)
    uint64_t virtualPrice; // Pool virtual price (optional)
    bool valid;
};

class ICurveService {
public:
    virtual ~ICurveService() = default;
    
    // Pool information
    virtual bool isValidPool(const std::string& pool) = 0;
    virtual std::string getToken(const std::string& pool, int32_t index) = 0;
    virtual uint8_t getTokenDecimals(const std::string& pool, int32_t index) = 0;
    virtual uint64_t getVirtualPrice(const std::string& pool) = 0;
    
    // Quotes
    virtual CurveQuote getQuote(const std::string& pool, int32_t i, int32_t j, uint64_t dx) = 0;
    virtual uint64_t getDy(const std::string& pool, int32_t i, int32_t j, uint64_t dx) = 0;
    
    // Swaps
    virtual std::string exchange(const std::string& pool, int32_t i, int32_t j, 
                               uint64_t dx, uint64_t minDy) = 0;
    virtual std::string exchangeUnderlying(const std::string& pool, int32_t i, int32_t j,
                                         uint64_t dx, uint64_t minDy) = 0;
    
    // Validation
    virtual bool validateIndices(const std::string& pool, int32_t i, int32_t j) = 0;
    virtual uint64_t calculateMinDy(uint64_t dy, uint16_t slippageBps) = 0;
};

} // namespace CurveTrading
