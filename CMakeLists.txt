cmake_minimum_required(VERSION 3.13)

project(coin VERSION 1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

add_subdirectory(leveldb)

include_directories(
    include
    include/Crypto
    include/net
    
    src
    src/crypto
    src/net
    leveldb/include
    )

set(SRCS
    src/Wallet.cpp
    src/Block.cpp
    src/DB.cpp
    src/BlockChain.cpp
    src/Transaction.cpp
    src/Mainer.cpp
    src/crypto/sha256.cpp
    src/crypto/rsa.cpp
    src/crypto/publickey.cpp
    src/crypto/privatekey.cpp
    src/crypto/ripemd160.cpp
    src/crypto/base58.cpp
    src/net/Message.cpp
    src/net/Network.cpp
    src/net/Server.cpp
    )

set(HEADERS
    include/Wallet.hpp
    include/Block.hpp
    include/DB.hpp
    include/BlockChain.hpp
    include/Transaction.hpp
    include/Mainer.hpp
    include/Crypto/sha256.hpp
    include/Crypto/rsa.hpp
    include/Crypto/publickey.hpp
    include/Crypto/privatekey.hpp
    include/Crypto/ripemd160.hpp
    include/Crypto/base58.hpp
    include/net/Message.hpp
    include/net/Network.hpp
    include/net/Server.hpp
    )


add_compile_options(
    #-Wall
    #-Wextra
    )

add_library(coin STATIC ${SRCS} ${HEADERS})

add_executable(main main.cpp)

target_link_libraries(main coin leveldb)

