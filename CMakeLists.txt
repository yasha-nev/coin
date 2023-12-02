cmake_minimum_required(VERSION 3.13)

project(coin VERSION 1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

add_subdirectory(leveldb)

include_directories(
    include
    include/crypto
    src
    src/crypto
    leveldb/include
    )

set(SRCS
    src/Wallet.cpp
    src/Block.cpp
    src/DB.cpp
    src/BlockChain.cpp
    src/Transaction.cpp
    src/crypto/sha256.cpp
    src/crypto/rsa.cpp
    src/crypto/publickey.cpp
    src/crypto/privatekey.cpp
    src/crypto/ripemd160.cpp
    src/crypto/base58.cpp
    )

set(HEADERS
    include/Wallet.hpp
    include/Block.hpp
    include/DB.hpp
    include/BlockChain.hpp
    include/Transaction.hpp
    include/crypto/sha256.hpp
    include/crypto/rsa.hpp
    include/crypto/publickey.hpp
    include/crypto/privatekey.hpp
    include/crypto/ripemd160.hpp
    include/crypto/base58.hpp
    )


add_compile_options(
    #-Wall
    #-Wextra
    )

add_library(coin STATIC ${SRCS} ${HEADERS})

add_executable(main main.cpp)

target_link_libraries(main coin leveldb)

