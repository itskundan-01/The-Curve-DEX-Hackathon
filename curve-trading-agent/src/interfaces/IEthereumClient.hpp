#pragma once

#include <string>
#include <vector>
#include <future>
#include <cstdint>

namespace CurveTrading {

struct TransactionReceipt {
    std::string txHash;
    bool success;
    uint64_t gasUsed;
    std::string failureReason;
};

struct GasEstimate {
    uint64_t gasLimit;
    uint64_t maxFeePerGas;
    uint64_t maxPriorityFeePerGas;
};

class IEthereumClient {
public:
    virtual ~IEthereumClient() = default;
    
    // Network info
    virtual std::string getChainId() = 0;
    virtual uint64_t getBlockNumber() = 0;
    virtual uint64_t getNonce(const std::string& address) = 0;
    
    // Contract calls
    virtual std::string call(const std::string& to, const std::string& data) = 0;
    virtual std::string call(const std::string& to, const std::string& data, const std::string& blockTag) = 0;
    
    // Gas estimation
    virtual GasEstimate estimateGas(const std::string& from, const std::string& to, const std::string& data) = 0;
    virtual uint64_t getBaseFee() = 0;
    virtual uint64_t getPriorityFee() = 0;
    
    // Transaction management
    virtual std::string sendRawTransaction(const std::string& signedTx) = 0;
    virtual TransactionReceipt getTransactionReceipt(const std::string& txHash) = 0;
    virtual bool waitForTransaction(const std::string& txHash, uint32_t timeoutSeconds = 300) = 0;
    
    // Utility
    virtual bool isConnected() = 0;
    virtual std::string getNetworkName() = 0;
};

} // namespace CurveTrading
