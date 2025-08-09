#include "Order.hpp"
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <sstream>
#include <random>
#include <iomanip>

using json = nlohmann::json;

namespace CurveTrading {

// Simple UUID generator function
std::string generateUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);
    
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-4";
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    ss << dis2(gen);
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dis(gen);
    return ss.str();
}

Order::Order() 
    : amountIn(0), targetPrice(0.0), policy(OrderPolicy::GTC), 
      maxSlippageBps(50), status(OrderStatus::NEW) {
    auto now = std::chrono::system_clock::now();
    createdAt = now;
    updatedAt = now;
    id = generateUUID();
}

Order::Order(const std::string& sellToken, const std::string& buyToken,
             const std::string& pool, int32_t i, int32_t j,
             uint64_t amountIn, double targetPrice, OrderPolicy policy,
             uint16_t maxSlippageBps)
    : sellToken(sellToken), buyToken(buyToken), pool(pool), i(i), j(j),
      amountIn(amountIn), targetPrice(targetPrice), policy(policy),
      maxSlippageBps(maxSlippageBps), status(OrderStatus::NEW) {
    
    auto now = std::chrono::system_clock::now();
    createdAt = now;
    updatedAt = now;
    id = generateUUID();
}

bool Order::isValid() const {
    if (sellToken.empty() || buyToken.empty() || pool.empty()) return false;
    if (amountIn == 0) return false;
    if (targetPrice <= 0.0) return false;
    if (i == j) return false;
    if (maxSlippageBps > 10000) return false; // Max 100%
    return true;
}

bool Order::isExpired() const {
    if (!expiry.has_value()) return false;
    return std::chrono::system_clock::now() > expiry.value();
}

bool Order::canExecute() const {
    return status == OrderStatus::WORKING && !isExpired();
}

void Order::markWorking() {
    status = OrderStatus::WORKING;
    updatedAt = std::chrono::system_clock::now();
}

void Order::markFilled(const std::string& txHash) {
    status = OrderStatus::FILLED;
    this->txHash = txHash;
    updatedAt = std::chrono::system_clock::now();
}

void Order::markCanceled() {
    status = OrderStatus::CANCELED;
    updatedAt = std::chrono::system_clock::now();
}

void Order::markExpired() {
    status = OrderStatus::EXPIRED;
    updatedAt = std::chrono::system_clock::now();
}

void Order::markFailed(const std::string& reason) {
    status = OrderStatus::FAILED;
    failureReason = reason;
    updatedAt = std::chrono::system_clock::now();
}

std::string Order::toJson() const {
    json j;
    j["id"] = id;
    j["sellToken"] = sellToken;
    j["buyToken"] = buyToken;
    j["pool"] = pool;
    j["i"] = i;
    j["j"] = this->j;
    j["amountIn"] = std::to_string(amountIn);
    j["targetPrice"] = targetPrice;
    j["policy"] = orderPolicyToString(policy);
    j["maxSlippageBps"] = maxSlippageBps;
    j["status"] = orderStatusToString(status);
    j["note"] = note;
    
    if (txHash.has_value()) {
        j["txHash"] = txHash.value();
    }
    
    if (!failureReason.empty()) {
        j["failureReason"] = failureReason;
    }
    
    // Convert timestamps to ISO strings
    auto time_t_created = std::chrono::system_clock::to_time_t(createdAt);
    auto time_t_updated = std::chrono::system_clock::to_time_t(updatedAt);
    
    std::stringstream ss_created, ss_updated;
    ss_created << std::put_time(std::gmtime(&time_t_created), "%Y-%m-%dT%H:%M:%SZ");
    ss_updated << std::put_time(std::gmtime(&time_t_updated), "%Y-%m-%dT%H:%M:%SZ");
    
    j["createdAt"] = ss_created.str();
    j["updatedAt"] = ss_updated.str();
    
    if (expiry.has_value()) {
        auto time_t_expiry = std::chrono::system_clock::to_time_t(expiry.value());
        std::stringstream ss_expiry;
        ss_expiry << std::put_time(std::gmtime(&time_t_expiry), "%Y-%m-%dT%H:%M:%SZ");
        j["expiry"] = ss_expiry.str();
    }
    
    return j.dump(2);
}

Order Order::fromJson(const std::string& jsonStr) {
    json js = json::parse(jsonStr);
    
    Order order;
    order.id = js["id"];
    order.sellToken = js["sellToken"];
    order.buyToken = js["buyToken"];
    order.pool = js["pool"];
    order.i = js["i"];
    order.j = js["j"];
    order.amountIn = std::stoull(static_cast<std::string>(js["amountIn"]));
    order.targetPrice = js["targetPrice"];
    order.policy = orderPolicyFromString(js["policy"]);
    order.maxSlippageBps = js["maxSlippageBps"];
    order.status = orderStatusFromString(js["status"]);
    order.note = js.value("note", "");
    order.failureReason = js.value("failureReason", "");
    
    if (js.contains("txHash")) {
        order.txHash = js["txHash"];
    }
    
    // Parse timestamps (simplified for now)
    // In production, would use proper ISO 8601 parsing
    
    return order;
}

std::string orderStatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::NEW: return "NEW";
        case OrderStatus::WORKING: return "WORKING";
        case OrderStatus::FILLED: return "FILLED";
        case OrderStatus::CANCELED: return "CANCELED";
        case OrderStatus::EXPIRED: return "EXPIRED";
        case OrderStatus::FAILED: return "FAILED";
        default: return "UNKNOWN";
    }
}

std::string orderPolicyToString(OrderPolicy policy) {
    switch (policy) {
        case OrderPolicy::GTC: return "GTC";
        case OrderPolicy::IOC: return "IOC";
        case OrderPolicy::FOK: return "FOK";
        default: return "UNKNOWN";
    }
}

OrderStatus orderStatusFromString(const std::string& str) {
    if (str == "NEW") return OrderStatus::NEW;
    if (str == "WORKING") return OrderStatus::WORKING;
    if (str == "FILLED") return OrderStatus::FILLED;
    if (str == "CANCELED") return OrderStatus::CANCELED;
    if (str == "EXPIRED") return OrderStatus::EXPIRED;
    if (str == "FAILED") return OrderStatus::FAILED;
    throw std::invalid_argument("Unknown order status: " + str);
}

OrderPolicy orderPolicyFromString(const std::string& str) {
    if (str == "GTC") return OrderPolicy::GTC;
    if (str == "IOC") return OrderPolicy::IOC;
    if (str == "FOK") return OrderPolicy::FOK;
    throw std::invalid_argument("Unknown order policy: " + str);
}

} // namespace CurveTrading
