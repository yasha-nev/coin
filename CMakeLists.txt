cmake_minimum_required(VERSION 3.15)

project(coin VERSION 1.0 LANGUAGES CXX)

include(FetchContent)

#===================GoogleTest=========================

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.15.2
)

FetchContent_MakeAvailable(googletest)

#=====================LevelDB===========================

set(LEVELDB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LEVELDB_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    leveldb
    GIT_REPOSITORY https://github.com/google/leveldb.git
    GIT_TAG 1.23
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(leveldb)

#=====================Cryptopp===========================

set(CRYPTOPP_BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    cryptopp-cmake
    GIT_REPOSITORY https://github.com/abdes/cryptopp-cmake.git
    GIT_TAG v8.8.0
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(cryptopp-cmake )

#======================General============================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    include
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
    src/crypto/CryptoppHashEncoder.cpp
    src/crypto/CryptoppRsaProcessor.cpp
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
    include/crypto/IHashEncoder.hpp
    include/crypto/CryptoppHashEncoder.hpp
    include/crypto/IRsaProcessor.hpp
    include/crypto/CryptoppRsaProcessor.hpp
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

#======================libcoin============================

add_library(libcoin STATIC ${SRCS} ${HEADERS})
target_include_directories(libcoin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(libcoin PUBLIC leveldb cryptopp)

#=======================coin==============================

add_executable(coin main.cpp)
target_link_libraries(coin PRIVATE libcoin)

#=======================tests==============================

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
enable_testing()
include(GoogleTest)

set(ALL_TESTS
    "hash_test"           "test/hash_test.cpp"
    "txinput_test"        "test/txinput_test.cpp"
    "txoutput_test"       "test/txoutput_test.cpp"
    "transaction_test"    "test/transaction_test.cpp"
    "block_test"          "test/block_test.cpp"
    "leveldbDataBase_test" "test/LeveldbDataBase_test.cpp"
    "message_test"        "test/message_test.cpp"
)

while(ALL_TESTS)
    list(POP_FRONT ALL_TESTS TEST_NAME TEST_SOURCE)

    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    target_link_libraries(${TEST_NAME} PRIVATE libcoin GTest::gtest_main)
    
    gtest_discover_tests(${TEST_NAME})
    
    install(TARGETS ${TEST_NAME} RUNTIME DESTINATION coin-tests)
endwhile()




