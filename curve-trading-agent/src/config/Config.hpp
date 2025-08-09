#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

namespace CurveTrading {

class Config {
public:
    static Config& getInstance();
    
    // Load configuration from environment variables and command line
    void load(int argc = 0, char* argv[] = nullptr);
    
    // Getters for configuration values
    std::string getRpcUrl() const { return m_rpcUrl; }
    uint32_t getChainId() const { return m_chainId; }
    std::string getWalletPrivateKey() const { return m_walletPrivateKey; }
    std::string getWalletAddress() const { return m_walletAddress; }
    
    std::string getCurvePoolAddress() const { return m_curvePoolAddress; }
    std::string getTokenSell() const { return m_tokenSell; }
    std::string getTokenBuy() const { return m_tokenBuy; }
    int32_t getTokenIndexSell() const { return m_tokenIndexSell; }
    int32_t getTokenIndexBuy() const { return m_tokenIndexBuy; }
    
    uint32_t getPollIntervalMs() const { return m_pollIntervalMs; }
    uint16_t getMaxSlippageBps() const { return m_maxSlippageBps; }
    double getGasPriorityFeeGwei() const { return m_gasPriorityFeeGwei; }
    uint16_t getGasLimitBufferPercent() const { return m_gasLimitBufferPercent; }
    
    bool isDryRun() const { return m_dryRun; }
    bool isTestnet() const { return m_chainId != 1; }
    
    // Setters (for programmatic configuration)
    void setDryRun(bool dryRun) { m_dryRun = dryRun; }
    void setWalletAddress(const std::string& address) { m_walletAddress = address; }
    
    // Validation
    bool isValid() const;
    std::string getValidationErrors() const;
    
    // Display
    void printConfig() const;
    
private:
    Config() = default;
    
    // Configuration values
    std::string m_rpcUrl;
    uint32_t m_chainId = 1; // mainnet
    std::string m_walletPrivateKey;
    std::string m_walletAddress;
    
    std::string m_curvePoolAddress;
    std::string m_tokenSell;
    std::string m_tokenBuy;
    int32_t m_tokenIndexSell = 0;
    int32_t m_tokenIndexBuy = 1;
    
    uint32_t m_pollIntervalMs = 1500;
    uint16_t m_maxSlippageBps = 50; // 0.5%
    double m_gasPriorityFeeGwei = 1.5;
    uint16_t m_gasLimitBufferPercent = 15;
    
    bool m_dryRun = false;
    
    // Helper methods
    void loadFromEnvironment();
    void loadFromCommandLine(int argc, char* argv[]);
    std::string getEnvVar(const std::string& name, const std::string& defaultValue = "");
    bool validateAddress(const std::string& address) const;
};

} // namespace CurveTrading
