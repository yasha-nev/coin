#ifndef ByteWriter_hpp
#define ByteWriter_hpp

#include <cassert>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <vector>

class ByteWriter {
public:
    template<typename T>
    void write(const T& value) {
        static_assert(std::is_trivially_copyable_v<T>);
        auto old = data.size();
        data.resize(old + sizeof(T));
        std::memcpy(data.data() + old, &value, sizeof(T));
    }

    void write_bytes(const std::byte* ptr, size_t size) {
        auto old = data.size();
        data.resize(old + size);
        std::memcpy(data.data() + old, ptr, size);
    }

    std::vector<std::byte> bytes() const {
        return data;
    }

private:
    std::vector<std::byte> data;
};

#endif
