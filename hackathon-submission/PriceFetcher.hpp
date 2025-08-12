#pragma once

#include <string>
#include <map>
#include <memory>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace CurveTrading {

struct PriceData {
    double price;
    uint64_t timestamp;
    std::string source;
    bool isValid;
};

class PriceFetcher {
private:
    std::map<std::string, PriceData> priceCache_;
    const uint64_t CACHE_EXPIRY_SECONDS = 30; // Cache prices for 30 seconds
    bool verboseLogging_ = false;  // New: control output verbosity
    
    // HTTP response structure
    struct HTTPResponse {
        std::string data;
        long response_code;
    };
    
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, HTTPResponse* response);
    HTTPResponse makeHttpRequest(const std::string& url, const std::map<std::string, std::string>& headers = {});
    
    // Price fetching methods for different sources
    PriceData fetchFromCoinGecko(const std::string& tokenPair);
    PriceData fetchFromUniswapV3(const std::string& token0, const std::string& token1);
    PriceData fetchFromChainlink(const std::string& tokenPair);
    PriceData fetchFrom1inch(const std::string& sellToken, const std::string& buyToken);
    
    // Helper methods
    std::string getTokenGeckoId(const std::string& symbol);
    std::string getChainlinkFeedAddress(const std::string& tokenPair);
    bool isCacheValid(const PriceData& data);

public:
    PriceFetcher();
    ~PriceFetcher();
    
    // Main price fetching interface
    PriceData getRealTimePrice(const std::string& sellToken, const std::string& buyToken);
    
    // Fallback to multiple sources
    PriceData getPriceWithFallback(const std::string& sellToken, const std::string& buyToken);
    
    // Cache management
    void clearCache();
    void setCacheExpiry(uint64_t seconds);
    
    // Verbose logging control
    void setVerboseLogging(bool verbose) { verboseLogging_ = verbose; }
    bool isVerboseLogging() const { return verboseLogging_; }
    
    // Utility methods
    static double calculateInversePrice(double price);
    static std::string formatPairKey(const std::string& token0, const std::string& token1);
};

} // namespace CurveTrading
