cmake_minimum_required(VERSION 3.16)
project(CurveDEXLimitOrderAgent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json REQUIRED)

# Find CURL
find_package(CURL REQUIRED)

# Include directories
include_directories(${CURL_INCLUDE_DIRS})

# Source files
set(SOURCES
    main.cpp
    Order.cpp
    OrderPolicies.cpp
    PriceUtils.cpp
    PriceFetcher.cpp
    LimitOrderEngine.cpp
    CLI.cpp
    blockchain_services.cpp
)

# Create executable
add_executable(curve_limit_order_agent ${SOURCES})

# Link libraries
target_link_libraries(curve_limit_order_agent
    ${CURL_LIBRARIES}
    nlohmann_json::nlohmann_json
)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(curve_limit_order_agent PRIVATE -Wall -Wextra -O2)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    target_compile_options(curve_limit_order_agent PRIVATE -Wall -Wextra -O2)
endif()

# Platform-specific configurations
if(APPLE)
    find_library(SECURITY_FRAMEWORK Security)
    target_link_libraries(curve_limit_order_agent ${SECURITY_FRAMEWORK})
endif()

# Installation
install(TARGETS curve_limit_order_agent DESTINATION bin)

# Testing (optional)
enable_testing()

# Create a simple test (remove for now due to main function conflict)
# add_executable(test_basic
#     Order.cpp
#     OrderPolicies.cpp
#     PriceUtils.cpp
# )

# target_link_libraries(test_basic nlohmann_json::nlohmann_json)

# Add a basic test
# add_test(NAME basic_functionality COMMAND test_basic)
