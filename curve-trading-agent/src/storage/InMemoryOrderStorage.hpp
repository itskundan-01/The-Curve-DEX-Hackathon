#pragma once

#include "../interfaces/IOrderStorage.hpp"
#include <unordered_map>
#include <fstream>
#include <mutex>

namespace CurveTrading {

class InMemoryOrderStorage : public IOrderStorage {
public:
    InMemoryOrderStorage(const std::string& logFilePath = "orders.json");
    ~InMemoryOrderStorage() = default;
    
    // IOrderStorage implementation
    void saveOrder(const Order& order) override;
    std::optional<Order> getOrder(const std::string& id) override;
    void updateOrder(const Order& order) override;
    void deleteOrder(const std::string& id) override;
    
    std::vector<Order> getAllOrders() override;
    std::vector<Order> getOrdersByStatus(OrderStatus status) override;
    std::vector<Order> getWorkingOrders() override;
    std::vector<Order> getExpiredOrders() override;
    
    void flush() override;
    void clear() override;
    size_t size() override;
    
private:
    std::unordered_map<std::string, Order> m_orders;
    std::string m_logFilePath;
    std::ofstream m_logFile;
    std::mutex m_mutex;
    
    void logOrderEvent(const Order& order, const std::string& event);
    void openLogFile();
};

} // namespace CurveTrading
