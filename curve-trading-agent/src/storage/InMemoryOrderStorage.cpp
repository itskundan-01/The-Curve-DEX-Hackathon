#include "InMemoryOrderStorage.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace CurveTrading {

InMemoryOrderStorage::InMemoryOrderStorage(const std::string& logFilePath) 
    : m_logFilePath(logFilePath) {
    openLogFile();
}

void InMemoryOrderStorage::saveOrder(const Order& order) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_orders[order.id] = order;
    logOrderEvent(order, "CREATED");
}

std::optional<Order> InMemoryOrderStorage::getOrder(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_orders.find(id);
    if (it != m_orders.end()) {
        return it->second;
    }
    return std::nullopt;
}

void InMemoryOrderStorage::updateOrder(const Order& order) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_orders.find(order.id);
    if (it != m_orders.end()) {
        OrderStatus oldStatus = it->second.status;
        it->second = order;
        
        // Log status changes
        if (oldStatus != order.status) {
            std::string event = "STATUS_CHANGE_" + orderStatusToString(order.status);
            logOrderEvent(order, event);
        }
    }
}

void InMemoryOrderStorage::deleteOrder(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_orders.find(id);
    if (it != m_orders.end()) {
        logOrderEvent(it->second, "DELETED");
        m_orders.erase(it);
    }
}

std::vector<Order> InMemoryOrderStorage::getAllOrders() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<Order> orders;
    orders.reserve(m_orders.size());
    
    for (const auto& pair : m_orders) {
        orders.push_back(pair.second);
    }
    
    // Sort by creation time (newest first)
    std::sort(orders.begin(), orders.end(), 
              [](const Order& a, const Order& b) {
                  return a.createdAt > b.createdAt;
              });
    
    return orders;
}

std::vector<Order> InMemoryOrderStorage::getOrdersByStatus(OrderStatus status) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<Order> filteredOrders;
    
    for (const auto& pair : m_orders) {
        if (pair.second.status == status) {
            filteredOrders.push_back(pair.second);
        }
    }
    
    return filteredOrders;
}

std::vector<Order> InMemoryOrderStorage::getWorkingOrders() {
    return getOrdersByStatus(OrderStatus::WORKING);
}

std::vector<Order> InMemoryOrderStorage::getExpiredOrders() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<Order> expiredOrders;
    
    for (const auto& pair : m_orders) {
        if (pair.second.isExpired()) {
            expiredOrders.push_back(pair.second);
        }
    }
    
    return expiredOrders;
}

void InMemoryOrderStorage::flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logFile.is_open()) {
        m_logFile.flush();
    }
}

void InMemoryOrderStorage::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    logOrderEvent(Order(), "STORAGE_CLEARED");
    m_orders.clear();
}

size_t InMemoryOrderStorage::size() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_orders.size();
}

void InMemoryOrderStorage::logOrderEvent(const Order& order, const std::string& event) {
    if (!m_logFile.is_open()) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    json logEntry;
    logEntry["timestamp"] = std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    logEntry["event"] = event;
    
    if (!order.id.empty()) {
        logEntry["orderId"] = order.id;
        logEntry["status"] = orderStatusToString(order.status);
        logEntry["policy"] = orderPolicyToString(order.policy);
        logEntry["sellToken"] = order.sellToken;
        logEntry["buyToken"] = order.buyToken;
        logEntry["amountIn"] = std::to_string(order.amountIn);
        logEntry["targetPrice"] = order.targetPrice;
        
        if (order.txHash.has_value()) {
            logEntry["txHash"] = order.txHash.value();
        }
        
        if (!order.failureReason.empty()) {
            logEntry["failureReason"] = order.failureReason;
        }
    }
    
    m_logFile << logEntry.dump() << std::endl;
    m_logFile.flush();
}

void InMemoryOrderStorage::openLogFile() {
    m_logFile.open(m_logFilePath, std::ios::app);
    if (m_logFile.is_open()) {
        logOrderEvent(Order(), "STORAGE_INITIALIZED");
    }
}

} // namespace CurveTrading
