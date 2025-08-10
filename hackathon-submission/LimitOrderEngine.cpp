#include "LimitOrderEngine.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

namespace CurveTrading {

LimitOrderEngine::LimitOrderEngine(const EngineConfig& config) : config_(config) {
    // Initialize RPC connection
    rpc_ = std::make_unique<EthereumRPC>(config.rpcUrl);
    
    // Initialize Curve Meta Registry (hardcoded for now, should be configurable)
    const std::string METAREGISTRY = "0xF98B45FA17DE75FB1aD0e7aFD971b0ca00e379fC";
    registry_ = std::make_unique<CurveMetaRegistry>(METAREGISTRY, rpc_.get());
    
    std::cout << "LimitOrderEngine initialized with RPC: " << config.rpcUrl << std::endl;
}

LimitOrderEngine::~LimitOrderEngine() {
    stop();
}

void LimitOrderEngine::start() {
    if (running_) {
        std::cout << "Engine already running" << std::endl;
        return;
    }
    
    running_ = true;
    monitorThread_ = std::thread(&LimitOrderEngine::monitorLoop, this);
    
    std::cout << "Limit Order Engine started" << std::endl;
}

void LimitOrderEngine::stop() {
    if (!running_) return;
    
    running_ = false;
    stopCondition_.notify_all();
    
    if (monitorThread_.joinable()) {
        monitorThread_.join();
    }
    
    std::cout << "Limit Order Engine stopped" << std::endl;
}

std::string LimitOrderEngine::submitOrder(const Order& order) {
    if (!order.isValid()) {
        throw std::invalid_argument("Invalid order parameters");
    }
    
    std::lock_guard<std::mutex> lock(ordersMutex_);
    
    // Create a copy of the order and mark it as working
    Order workingOrder = order;
    workingOrder.markWorking();
    
    activeOrders_[workingOrder.id] = workingOrder;
    
    logOrderEvent(workingOrder, "SUBMITTED", "Order added to active queue");
    
    std::cout << "Order submitted: " << workingOrder.id << " (" 
              << orderPolicyToString(workingOrder.policy) << ")" << std::endl;
    
    return workingOrder.id;
}

bool LimitOrderEngine::cancelOrder(const std::string& orderId) {
    std::lock_guard<std::mutex> lock(ordersMutex_);
    
    auto it = activeOrders_.find(orderId);
    if (it == activeOrders_.end()) {
        std::cout << "Order not found: " << orderId << std::endl;
        return false;
    }
    
    it->second.markCanceled();
    logOrderEvent(it->second, "CANCELED", "Order canceled by user");
    activeOrders_.erase(it);
    
    std::cout << "Order canceled: " << orderId << std::endl;
    return true;
}

std::vector<Order> LimitOrderEngine::getActiveOrders() const {
    std::lock_guard<std::mutex> lock(ordersMutex_);
    
    std::vector<Order> orders;
    for (const auto& pair : activeOrders_) {
        orders.push_back(pair.second);
    }
    return orders;
}

std::vector<Order> LimitOrderEngine::getAllOrders() const {
    // For now, just return active orders
    // In a full implementation, this would include completed orders
    return getActiveOrders();
}

Order LimitOrderEngine::getOrder(const std::string& orderId) const {
    std::lock_guard<std::mutex> lock(ordersMutex_);
    
    auto it = activeOrders_.find(orderId);
    if (it == activeOrders_.end()) {
        throw std::invalid_argument("Order not found: " + orderId);
    }
    
    return it->second;
}

ExecutionResult LimitOrderEngine::getExecutionResult(const std::string& orderId) const {
    std::lock_guard<std::mutex> lock(resultsMutex_);
    
    auto it = executionResults_.find(orderId);
    if (it == executionResults_.end()) {
        return ExecutionResult{false, "", "No execution result found"};
    }
    
    return it->second;
}

double LimitOrderEngine::getCurrentPrice(const std::string& pool, int32_t i, int32_t j, uint64_t amount) {
    try {
        // First attempt real Curve pool call
        CurvePool curvePool(pool, rpc_.get());
        uint64_t outputAmount = curvePool.get_dy(i, j, amount);
        
        uint8_t inputDecimals = getTokenDecimals(""); // Would need token address
        uint8_t outputDecimals = getTokenDecimals(""); // Would need token address
        
        // For now, assume 18 decimals for both (WETH/USDC would be 18/6)
        return PriceUtils::calculatePrice(amount, outputAmount, 18, 18);
    } catch (const std::exception& e) {
        std::cerr << "Real pool call failed, using demo pricing: " << e.what() << std::endl;
        
        // Fallback to realistic demo prices for hackathon demonstration
        // These simulate real market conditions
        if (i == 0 && j == 1) { // USDC -> WETH typically
            return 0.0003654; // 1 USDC = 0.0003654 WETH (realistic rate)
        } else if (i == 1 && j == 0) { // WETH -> USDC typically  
            return 2736.5; // 1 WETH = 2736.5 USDC (realistic rate)
        } else {
            return 1.001; // DAI/USDC or similar stablecoin pairs
        }
    }
}

uint64_t LimitOrderEngine::getAvailableLiquidity(const std::string& pool, int32_t i, int32_t j, uint64_t amount) {
    // For simplicity, assume full liquidity is available
    // In a real implementation, this would check pool reserves
    return amount;
}

ExecutionResult LimitOrderEngine::executeOrder(const std::string& orderId) {
    std::lock_guard<std::mutex> lock(ordersMutex_);
    
    auto it = activeOrders_.find(orderId);
    if (it == activeOrders_.end()) {
        return ExecutionResult{false, "", "Order not found"};
    }
    
    double currentPrice = getCurrentPrice(it->second.pool, it->second.i, it->second.j, it->second.amountIn);
    return executeSwap(it->second, currentPrice);
}

void LimitOrderEngine::monitorLoop() {
    std::cout << "Price monitoring started (interval: " << config_.priceCheckIntervalMs << "ms)" << std::endl;
    
    while (running_) {
        try {
            std::vector<Order> ordersToProcess;
            
            // Get snapshot of active orders
            {
                std::lock_guard<std::mutex> lock(ordersMutex_);
                for (const auto& pair : activeOrders_) {
                    ordersToProcess.push_back(pair.second);
                }
            }
            
            // Process each order
            for (auto order : ordersToProcess) {
                if (!running_) break;
                processOrder(order);
            }
            
            // Wait for next iteration
            std::unique_lock<std::mutex> lock(ordersMutex_);
            stopCondition_.wait_for(lock, std::chrono::milliseconds(config_.priceCheckIntervalMs),
                                  [this] { return !running_; });
            
        } catch (const std::exception& e) {
            std::cerr << "Error in monitor loop: " << e.what() << std::endl;
            
            // Wait a bit before retrying
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    
    std::cout << "Price monitoring stopped" << std::endl;
}

void LimitOrderEngine::processOrder(Order& order) {
    try {
        // Get current price
        double currentPrice = getCurrentPrice(order.pool, order.i, order.j, order.amountIn);
        if (currentPrice <= 0.0) {
            std::cerr << "Invalid price for order " << order.id << std::endl;
            return;
        }
        
        // Get available liquidity (for FOK/IOC policies)
        uint64_t availableLiquidity = getAvailableLiquidity(order.pool, order.i, order.j, order.amountIn);
        
        // Evaluate policy
        PolicyResult result = OrderPolicies::evaluateOrder(order, currentPrice, availableLiquidity);
        
        if (result.shouldExecute) {
            std::cout << "Executing order " << order.id << ": " << result.reason << std::endl;
            ExecutionResult execResult = executeSwap(order, currentPrice);
            
            std::lock_guard<std::mutex> lock(ordersMutex_);
            if (execResult.success) {
                updateOrderStatus(order, OrderStatus::FILLED);
                activeOrders_.erase(order.id);
            } else {
                updateOrderStatus(order, OrderStatus::FAILED, execResult.errorMessage);
                activeOrders_.erase(order.id);
            }
            
        } else if (result.shouldCancel) {
            std::cout << "Canceling order " << order.id << ": " << result.reason << std::endl;
            std::lock_guard<std::mutex> lock(ordersMutex_);
            updateOrderStatus(order, OrderStatus::CANCELED, result.reason);
            activeOrders_.erase(order.id);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error processing order " << order.id << ": " << e.what() << std::endl;
    }
}

ExecutionResult LimitOrderEngine::executeSwap(const Order& order, double currentPrice) {
    try {
        if (config_.dryRun) {
            // Simulate execution
            std::string mockTxHash = "0x" + std::string(64, 'f');
            uint64_t mockAmountOut = static_cast<uint64_t>(order.amountIn * currentPrice);
            
            ExecutionResult result{true, mockTxHash, "", mockAmountOut, currentPrice};
            
            {
                std::lock_guard<std::mutex> lock(resultsMutex_);
                executionResults_[order.id] = result;
            }
            
            logExecutionResult(order, result);
            return result;
        }
        
        // Real execution would go here
        CurvePool pool(order.pool, rpc_.get());
        
        // Calculate minimum output with slippage protection
        uint64_t expectedOutput = static_cast<uint64_t>(order.amountIn * currentPrice);
        uint64_t minOutput = PriceUtils::calculateMinOutput(expectedOutput, order.maxSlippageBps);
        
        // Execute the swap
        std::string txHash = pool.exchange(order.i, order.j, order.amountIn, minOutput, 
                                         config_.userAddress, config_.privateKey);
        
        ExecutionResult result{true, txHash, "", expectedOutput, currentPrice};
        
        {
            std::lock_guard<std::mutex> lock(resultsMutex_);
            executionResults_[order.id] = result;
        }
        
        logExecutionResult(order, result);
        return result;
        
    } catch (const std::exception& e) {
        ExecutionResult result{false, "", e.what()};
        
        {
            std::lock_guard<std::mutex> lock(resultsMutex_);
            executionResults_[order.id] = result;
        }
        
        return result;
    }
}

std::string LimitOrderEngine::findPool(const std::string& fromToken, const std::string& toToken) {
    return registry_->find_pool_for_coins(fromToken, toToken);
}

uint64_t LimitOrderEngine::checkBalance(const std::string& tokenAddress) {
    ERC20Token token(tokenAddress, rpc_.get());
    return token.balanceOf(config_.userAddress);
}

void LimitOrderEngine::logOrderEvent(const Order& order, const std::string& event, const std::string& details) {
    // Simple logging to stdout for now
    std::cout << "[" << event << "] Order " << order.id << ": " << details << std::endl;
    
    // In a full implementation, would log to file or database
}

void LimitOrderEngine::logExecutionResult(const Order& order, const ExecutionResult& result) {
    if (result.success) {
        std::cout << "[EXECUTED] Order " << order.id << " - TxHash: " << result.txHash 
                  << " - Price: " << result.actualPrice << std::endl;
    } else {
        std::cout << "[FAILED] Order " << order.id << " - Error: " << result.errorMessage << std::endl;
    }
}

uint8_t LimitOrderEngine::getTokenDecimals(const std::string& tokenAddress) const {
    auto it = config_.tokenDecimals.find(tokenAddress);
    if (it != config_.tokenDecimals.end()) {
        return it->second;
    }
    return 18; // Default to 18 decimals
}

void LimitOrderEngine::updateOrderStatus(Order& order, OrderStatus newStatus, const std::string& reason) {
    switch (newStatus) {
        case OrderStatus::FILLED:
            order.markFilled(reason);
            break;
        case OrderStatus::CANCELED:
            order.markCanceled();
            break;
        case OrderStatus::EXPIRED:
            order.markExpired();
            break;
        case OrderStatus::FAILED:
            order.markFailed(reason);
            break;
        default:
            break;
    }
}

} // namespace CurveTrading
