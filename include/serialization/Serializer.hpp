#ifndef Serializer_hpp
#define Serializer_hpp

#include <vector>

class Serializer {
public:
    virtual ~Serializer() = default;

    virtual std::vector<std::byte> encode() const = 0;
    virtual void decode(const std::vector<std::byte>& data) = 0;
};

#endif
