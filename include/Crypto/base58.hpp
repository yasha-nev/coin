#ifndef base58_hpp
#define base58_hpp

#include <string>

std::string EncodeBase58(const std::string &source);

std::string DecodeBase58(const std::string &str);

#endif /* base58_hpp */
