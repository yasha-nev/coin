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
    include/Crypto
    include/net
    include/db
    
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
    src/Mainer.cpp
    src/db/LeveldbDataBase.cpp
    src/crypto/RSACryptor.cpp
    src/crypto/publickey.cpp
    src/crypto/privatekey.cpp
    src/crypto/CryptoppImpl.cpp
    src/crypto/base58.cpp
    src/net/Message.cpp
    src/net/Network.cpp
    src/net/Server.cpp
    )

set(HEADERS
    include/Wallet.hpp
    include/Block.hpp
    include/BlockChain.hpp
    include/Transaction.hpp
    include/Mainer.hpp
    include/db/IDataBase.hpp
    include/db/LeveldbDataBase.hpp
    include/Crypto/ICrypto.hpp
    include/Crypto/CryptoppImpl.hpp
    include/Crypto/RSACryptor.hpp
    include/Crypto/publickey.hpp
    include/Crypto/privatekey.hpp
    include/Crypto/base58.hpp
    include/net/Message.hpp
    include/net/Network.hpp
    include/net/Server.hpp
    )


add_compile_options(
    #-Wall
    #-Wextra
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
add_executable(block_test test/block_test.cpp)
target_link_libraries(block_test coin)
add_test(NAME coin-block COMMAND block_test)

add_executable(transaction_test test/transaction_test.cpp)
target_link_libraries(transaction_test coin)
add_test(NAME coin-transaction COMMAND transaction_test)

add_executable(leveldbDataBase_test test/LeveldbDataBase_test.cpp)
target_link_libraries(leveldbDataBase_test coin)
add_test(NAME coin-leveldbDatabase COMMAND leveldbDataBase_test)

add_executable(message_test test/message_test.cpp)
target_link_libraries(message_test coin)
add_test(NAME coin-message COMMAND message_test)

install(TARGETS block_test transaction_test leveldbDataBase_test message_test
        RUNTIME DESTINATION coin-tests)




