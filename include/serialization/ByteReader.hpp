#ifndef ByteReader_hpp
#define ByteReader_hpp

#include <vector>
#include <stdexcept>
#include <cstring>

class ByteReader {
public:
    explicit ByteReader(const std::vector<std::byte>& buffer)
        : m_data(buffer),
          m_position(0) {}
    
    template<typename T>
    T read() {
        static_assert(std::is_trivial_v<T>, "Type must be trivial");
        
        if (m_position + sizeof(T) > m_data.size()) {
            throw std::runtime_error("Not enough data to read.");
        }
        
        T result;
        std::memcpy(&result, m_data.data() + m_position, sizeof(T));
        m_position += sizeof(T);
        return result;
    }
    
    std::vector<std::byte> read_bytes(size_t count) {
        if (m_position + count > m_data.size()) {
            throw std::runtime_error("Not enough data to read.");
        }

        std::vector<std::byte> bytes(count);
        
        std::memcpy(bytes.data(), m_data.data() + m_position, count);
        m_position += count;

        return bytes;
    }
    
    size_t remaining_size() const {
        return m_data.size() - m_position;
    }

private:
    std::vector<std::byte> m_data;
    size_t m_position;
};

#endif