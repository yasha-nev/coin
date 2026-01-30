cmake_minimum_required(VERSION 3.15)

project(coin VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

set(LEVELDB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LEVELDB_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(CRYPTOPP_BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    leveldb
    GIT_REPOSITORY https://github.com/google/leveldb.git
    GIT_TAG 1.23
    OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
    cryptopp-cmake
    GIT_REPOSITORY https://github.com/abdes/cryptopp-cmake.git
    GIT_TAG v8.8.0
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(cryptopp-cmake leveldb)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    include
    include/crypto
    include/net
    include/db
    include/serialization
    
    src
    src/db
    src/crypto
    src/net
    )

set(SRCS
    src/Wallet.cpp
    src/Block.cpp
    src/BlockChain.cpp
    src/Transaction.cpp
    src/TXInput.cpp
    src/TXOutput.cpp
    src/Mainer.cpp
    src/db/LeveldbDataBase.cpp
    src/crypto/Hash.cpp
    src/crypto/RSACryptor.cpp
    src/crypto/publickey.cpp
    src/crypto/privatekey.cpp
    src/crypto/CryptoppImpl.cpp
    src/crypto/base58.cpp
    src/net/Client.cpp
    src/net/Message.cpp
    src/net/Network.cpp
    src/net/Server.cpp
    )

set(HEADERS
    include/Wallet.hpp
    include/Block.hpp
    include/BlockChain.hpp
    include/Transaction.hpp
    include/TXInput.hpp
    include/TXOutput.hpp
    include/Mainer.hpp
    include/db/IDataBase.hpp
    include/db/LeveldbDataBase.hpp
    include/crypto/Hash.hpp
    include/crypto/ICrypto.hpp
    include/crypto/CryptoppImpl.hpp
    include/crypto/RSACryptor.hpp
    include/crypto/publickey.hpp
    include/crypto/privatekey.hpp
    include/crypto/base58.hpp
    include/net/Client.hpp
    include/net/Message.hpp
    include/net/Network.hpp
    include/net/Server.hpp
    include/serialization/ByteReader.hpp
    include/serialization/ByteWriter.hpp
    include/serialization/Serializer.hpp
    )


add_compile_options(
    -Wall
    -Wextra
    )


#===========LIBRARY=================
add_library(coin STATIC ${SRCS} ${HEADERS})

target_include_directories(coin PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(coin PUBLIC leveldb cryptopp)

#=============COIN==================
add_executable(main main.cpp)
target_link_libraries(main PRIVATE coin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
enable_testing()

#=============TESTS=================

# Hash Test
add_executable(hash_test test/hash_test.cpp)
target_link_libraries(hash_test coin)
add_test(NAME coin-hash COMMAND hash_test)

# Transaction Input Test
add_executable(txinput_test test/txinput_test.cpp)
target_link_libraries(txinput_test coin)
add_test(NAME coin-txinput COMMAND txinput_test)

# Transaction Output Test
add_executable(txoutput_test test/txoutput_test.cpp)
target_link_libraries(txoutput_test coin)
add_test(NAME coin-txoutput COMMAND txoutput_test)

# Transaction Test
add_executable(transaction_test test/transaction_test.cpp)
target_link_libraries(transaction_test coin)
add_test(NAME coin-transaction COMMAND transaction_test)

# Block Test
add_executable(block_test test/block_test.cpp)
target_link_libraries(block_test coin)
add_test(NAME coin-block COMMAND block_test)

# Level database test
add_executable(leveldbDataBase_test test/LeveldbDataBase_test.cpp)
target_link_libraries(leveldbDataBase_test coin)
add_test(NAME coin-leveldbDatabase COMMAND leveldbDataBase_test)

# Network messages test
add_executable(message_test test/message_test.cpp)
target_link_libraries(message_test coin)
add_test(NAME coin-message COMMAND message_test)

install(TARGETS txinput_test txoutput_test transaction_test block_test leveldbDataBase_test message_test
        RUNTIME DESTINATION coin-tests)




