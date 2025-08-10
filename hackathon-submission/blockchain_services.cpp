#include "blockchain_services.hpp"

// Utility function implementations
std::string encodeUint256(uint64_t value) {
  std::stringstream ss;
  ss << std::hex << std::setfill('0') << std::setw(64) << value;
  return ss.str();
}

std::string encodeAddress(const std::string& address) {
  std::string clean_addr = address;
  if (clean_addr.substr(0, 2) == "0x") {
    clean_addr = clean_addr.substr(2);
  }
  return std::string(24, '0') + clean_addr;  // Pad to 32 bytes
}

uint64_t hexToUint64(const std::string& hex) {
  std::string cleanHex = hex;
  if (cleanHex.substr(0, 2) == "0x") {
    cleanHex = cleanHex.substr(2);
  }
  if (cleanHex.empty() || cleanHex == "0" ||
      cleanHex.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos) {
    return 0;  // Return 0 for invalid or empty hex
  }
  try {
    return std::stoull(cleanHex, nullptr, 16);
  } catch (const std::exception& e) {
    std::cerr << "Warning: Failed to parse hex value '" << hex << "': " << e.what() << std::endl;
    return 0;
  }
}
