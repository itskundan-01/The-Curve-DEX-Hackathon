#pragma once

#include "../core/Order.hpp"
#include <vector>
#include <optional>

namespace CurveTrading {

class IOrderStorage {
public:
    virtual ~IOrderStorage() = default;
    
    // CRUD operations
    virtual void saveOrder(const Order& order) = 0;
    virtual std::optional<Order> getOrder(const std::string& id) = 0;
    virtual void updateOrder(const Order& order) = 0;
    virtual void deleteOrder(const std::string& id) = 0;
    
    // Queries
    virtual std::vector<Order> getAllOrders() = 0;
    virtual std::vector<Order> getOrdersByStatus(OrderStatus status) = 0;
    virtual std::vector<Order> getWorkingOrders() = 0;
    virtual std::vector<Order> getExpiredOrders() = 0;
    
    // Persistence
    virtual void flush() = 0;
    virtual void clear() = 0;
    virtual size_t size() = 0;
};

} // namespace CurveTrading
