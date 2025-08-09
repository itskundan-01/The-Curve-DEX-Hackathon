#pragma once

#include <string>
#include <cstdint>

namespace CurveTrading {

class IERC20Service {
public:
    virtual ~IERC20Service() = default;
    
    // Token info
    virtual uint8_t decimals(const std::string& token) = 0;
    virtual std::string symbol(const std::string& token) = 0;
    virtual std::string name(const std::string& token) = 0;
    
    // Balance and allowance
    virtual uint64_t balanceOf(const std::string& token, const std::string& owner) = 0;
    virtual uint64_t allowance(const std::string& token, const std::string& owner, const std::string& spender) = 0;
    
    // Approval
    virtual std::string approve(const std::string& token, const std::string& spender, uint64_t amount) = 0;
    virtual std::string approveMax(const std::string& token, const std::string& spender) = 0;
    
    // Utility
    virtual bool hasMinimumAllowance(const std::string& token, const std::string& owner, 
                                   const std::string& spender, uint64_t required) = 0;
    virtual std::string ensureAllowance(const std::string& token, const std::string& spender, uint64_t required) = 0;
};

} // namespace CurveTrading
