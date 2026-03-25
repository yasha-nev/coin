#ifndef ByteWriter_hpp
#define ByteWriter_hpp

#include <cassert>
#include <cstddef>
#include <cstring>
#include <span>
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

    void write_bytes(std::span<const std::byte> bytes) {
        auto old = data.size();
        data.resize(old + bytes.size());
        std::memcpy(data.data() + old, bytes.data(), bytes.size());
    }

    const std::vector<std::byte>& bytes() const {
        return data;
    }

private:
    std::vector<std::byte> data;
};

inline std::span<const std::byte> as_bytes(const std::byte* ptr, size_t size) {
    return { ptr, size };
}

template<typename T>
inline std::span<const std::byte> as_bytes(const T* ptr, size_t size)
    requires(sizeof(T) == 1)
{
    return { reinterpret_cast<const std::byte*>(ptr), size };
}

template<typename T>
inline std::span<const std::byte> as_bytes(const std::vector<T>& arr)
    requires(sizeof(T) == 1)
{
    return std::as_bytes(std::span(arr));
}

#endif
