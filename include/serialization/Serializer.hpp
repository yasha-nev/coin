#pragma once
#include "ByteReader.hpp"
#include "ByteWriter.hpp"

#include <span>
#include <vector>

class Serializer {
public:
    virtual ~Serializer() = default;

    virtual void encode(ByteWriter& writer) const = 0;

    virtual void decode(ByteReader& reader) = 0;
};
