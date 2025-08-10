#include "CLI.hpp"
#include <iostream>
#include <exception>

int main(int argc, char* argv[]) {
    (void)argc; // Suppress unused parameter warning
    (void)argv; // Suppress unused parameter warning
    try {
        std::cout << "Curve DEX Limit Order Agent - Hackathon Solution" << std::endl;
        std::cout << "Initializing..." << std::endl;
        
        CurveTrading::CLI cli;
        cli.run();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
