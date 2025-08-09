cmake_minimum_required(VERSION 3.15)
project(CurveTradingAgent)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Include directories
include_directories(src)

# Core library
set(CORE_SOURCES
    src/core/Order.cpp
    src/core/OrderPolicies.cpp
    src/utils/PriceUtils.cpp
)

add_library(curve_trading_core ${CORE_SOURCES})
target_link_libraries(curve_trading_core nlohmann_json::nlohmann_json)

# Unit tests
enable_testing()

# Individual test executables
add_executable(test_order tests/test_order.cpp)
target_link_libraries(test_order curve_trading_core)

add_executable(test_price_utils tests/test_price_utils.cpp)
target_link_libraries(test_price_utils curve_trading_core)

add_executable(test_policies tests/test_policies.cpp)
target_link_libraries(test_policies curve_trading_core)

# Add individual tests
add_test(NAME order_tests COMMAND test_order)
add_test(NAME price_utils_tests COMMAND test_price_utils)
add_test(NAME policies_tests COMMAND test_policies)

# Main executable (placeholder)
add_executable(trading_agent src/main.cpp)
target_link_libraries(trading_agent curve_trading_core)
