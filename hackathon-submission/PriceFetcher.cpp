#include "PriceFetcher.hpp"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace CurveTrading {

PriceFetcher::PriceFetcher() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

PriceFetcher::~PriceFetcher() {
    curl_global_cleanup();
}

size_t PriceFetcher::WriteCallback(void* contents, size_t size, size_t nmemb, HTTPResponse* response) {
    size_t totalSize = size * nmemb;
    response->data.append((char*)contents, totalSize);
    return totalSize;
}

PriceFetcher::HTTPResponse PriceFetcher::makeHttpRequest(const std::string& url, const std::map<std::string, std::string>& headers) {
    HTTPResponse response;
    response.response_code = 0;
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        return response;
    }
    
    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    
    // Set callback function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    // Set timeout
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    
    // Set User-Agent
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "CurveDEX-LimitOrderAgent/1.0");
    
    // Add custom headers
    struct curl_slist* headerList = nullptr;
    for (const auto& header : headers) {
        std::string headerStr = header.first + ": " + header.second;
        headerList = curl_slist_append(headerList, headerStr.c_str());
    }
    if (headerList) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    }
    
    // Perform request
    CURLcode res = curl_easy_perform(curl);
    
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.response_code);
    }
    
    // Cleanup
    if (headerList) {
        curl_slist_free_all(headerList);
    }
    curl_easy_cleanup(curl);
    
    return response;
}

PriceData PriceFetcher::fetchFromCoinGecko(const std::string& tokenPair) {
    PriceData result;
    result.isValid = false;
    result.source = "CoinGecko";
    result.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    try {
        // Parse token pair
        std::string sellToken, buyToken;
        size_t pos = tokenPair.find('/');
        if (pos != std::string::npos) {
            sellToken = tokenPair.substr(0, pos);
            buyToken = tokenPair.substr(pos + 1);
        } else {
            return result;
        }
        
        std::string sellId = getTokenGeckoId(sellToken);
        std::string buyId = getTokenGeckoId(buyToken);
        
        if (sellId.empty() || buyId.empty()) {
            return result;
        }
        
        // CoinGecko API URL
        std::string url = "https://api.coingecko.com/api/v3/simple/price?ids=" + 
                         sellId + "," + buyId + "&vs_currencies=usd";
        
        HTTPResponse response = makeHttpRequest(url);
        
        if (response.response_code == 200 && !response.data.empty()) {
            json j = json::parse(response.data);
            
            if (j.contains(sellId) && j.contains(buyId) &&
                j[sellId].contains("usd") && j[buyId].contains("usd")) {
                
                double sellPriceUSD = j[sellId]["usd"];
                double buyPriceUSD = j[buyId]["usd"];
                
                if (buyPriceUSD > 0) {
                    result.price = sellPriceUSD / buyPriceUSD;
                    result.isValid = true;
                    
                    if (verboseLogging_) {
                        std::cout << "[REAL PRICE] " << sellToken << "/" << buyToken << ": " 
                                  << result.price << " (via CoinGecko)" << std::endl;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        if (verboseLogging_) {
            std::cout << "[PRICE ERROR] CoinGecko fetch failed: " << e.what() << std::endl;
        }
    }
    
    return result;
}

PriceData PriceFetcher::fetchFrom1inch(const std::string& sellToken, const std::string& buyToken) {
    PriceData result;
    result.isValid = false;
    result.source = "1inch";
    result.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    try {
        // 1inch API for price quotes (Ethereum mainnet)
        // Note: This would need actual token addresses for Sepolia
        std::string url = "https://api.1inch.io/v5.0/1/quote?fromTokenSymbol=" + 
                         sellToken + "&toTokenSymbol=" + buyToken + "&amount=1000000000000000000";
        
        HTTPResponse response = makeHttpRequest(url);
        
        if (response.response_code == 200 && !response.data.empty()) {
            json j = json::parse(response.data);
            
            if (j.contains("toTokenAmount") && j.contains("fromTokenAmount")) {
                double fromAmount = std::stod(j["fromTokenAmount"].get<std::string>());
                double toAmount = std::stod(j["toTokenAmount"].get<std::string>());
                
                if (fromAmount > 0) {
                    result.price = toAmount / fromAmount;
                    result.isValid = true;
                    
                    if (verboseLogging_) {
                        std::cout << "[REAL PRICE] " << sellToken << "/" << buyToken << ": " 
                                  << result.price << " (via 1inch)" << std::endl;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << "[PRICE ERROR] 1inch fetch failed: " << e.what() << std::endl;
    }
    
    return result;
}

PriceData PriceFetcher::fetchFromUniswapV3(const std::string& token0, const std::string& token1) {
    // Suppress unused parameter warnings
    (void)token0;
    (void)token1;
    
    // This would require GraphQL queries to Uniswap subgraph
    // For now, return invalid to fall back to other sources
    PriceData result;
    result.isValid = false;
    result.source = "UniswapV3";
    result.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // TODO: Implement Uniswap V3 subgraph query
    return result;
}

PriceData PriceFetcher::fetchFromChainlink(const std::string& tokenPair) {
    // Suppress unused parameter warning
    (void)tokenPair;
    
    // This would require actual Chainlink oracle calls
    PriceData result;
    result.isValid = false;
    result.source = "Chainlink";
    result.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // TODO: Implement Chainlink price feed calls
    return result;
}

std::string PriceFetcher::getTokenGeckoId(const std::string& symbol) {
    static std::map<std::string, std::string> tokenMap = {
        {"ETH", "ethereum"},
        {"WETH", "weth"},
        {"USDC", "usd-coin"},
        {"DAI", "dai"},
        {"USDT", "tether"},
        {"BTC", "bitcoin"},
        {"WBTC", "wrapped-bitcoin"}
    };
    
    auto it = tokenMap.find(symbol);
    return (it != tokenMap.end()) ? it->second : "";
}

bool PriceFetcher::isCacheValid(const PriceData& data) {
    if (!data.isValid) return false;
    
    uint64_t currentTime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    return (currentTime - data.timestamp) < CACHE_EXPIRY_SECONDS;
}

PriceData PriceFetcher::getRealTimePrice(const std::string& sellToken, const std::string& buyToken) {
    std::string pairKey = formatPairKey(sellToken, buyToken);
    
    // Check cache first
    auto it = priceCache_.find(pairKey);
    if (it != priceCache_.end() && isCacheValid(it->second)) {
        if (verboseLogging_) {
            std::cout << "[CACHED PRICE] " << sellToken << "/" << buyToken << ": " 
                      << it->second.price << " (cached)" << std::endl;
        }
        return it->second;
    }
    
    // Try multiple sources with fallback
    return getPriceWithFallback(sellToken, buyToken);
}

PriceData PriceFetcher::getPriceWithFallback(const std::string& sellToken, const std::string& buyToken) {
    std::string pairKey = formatPairKey(sellToken, buyToken);
    
    // Try CoinGecko first (most reliable for major tokens)
    PriceData price = fetchFromCoinGecko(sellToken + "/" + buyToken);
    if (price.isValid) {
        priceCache_[pairKey] = price;
        return price;
    }
    
    // Try 1inch API
    price = fetchFrom1inch(sellToken, buyToken);
    if (price.isValid) {
        priceCache_[pairKey] = price;
        return price;
    }
    
    // Try Uniswap V3 (if implemented)
    price = fetchFromUniswapV3(sellToken, buyToken);
    if (price.isValid) {
        priceCache_[pairKey] = price;
        return price;
    }
    
    // All sources failed - return invalid
    if (verboseLogging_) {
        std::cout << "[PRICE WARNING] All real price sources failed for " 
                  << sellToken << "/" << buyToken << std::endl;
    }
    
    PriceData fallback;
    fallback.isValid = false;
    fallback.source = "Failed";
    return fallback;
}

std::string PriceFetcher::formatPairKey(const std::string& token0, const std::string& token1) {
    return token0 + "/" + token1;
}

double PriceFetcher::calculateInversePrice(double price) {
    return (price > 0) ? (1.0 / price) : 0.0;
}

void PriceFetcher::clearCache() {
    priceCache_.clear();
}

void PriceFetcher::setCacheExpiry(uint64_t seconds) {
    const_cast<uint64_t&>(CACHE_EXPIRY_SECONDS) = seconds;
}

} // namespace CurveTrading
