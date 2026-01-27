#ifndef ByteReader_hpp
#define ByteReader_hpp

#include <cstring>
#include <span>
#include <stdexcept>

class ByteReader {
public:
    explicit ByteReader(std::span<const std::byte> buffer):
        m_data(buffer),
        m_position(0) {
    }

    template<typename T>
    T read() {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");

        if(m_position + sizeof(T) > m_data.size()) {
            throw std::runtime_error("Not enough data to read.");
        }

        T result;
        std::memcpy(&result, m_data.data() + m_position, sizeof(T));
        m_position += sizeof(T);
        return result;
    }

    std::span<const std::byte> read_bytes(size_t count) {
        if(m_position + count > m_data.size()) {
            throw std::runtime_error("Not enough data to read bytes");
        }

        auto result = m_data.subspan(m_position, count);
        m_position += count;
        return result;
    }

    size_t remaining_size() const {
        return m_data.size() - m_position;
    }

private:
    std::span<const std::byte> m_data;
    size_t m_position;
};

#endif
