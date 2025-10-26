cmake_minimum_required(VERSION 3.13)

project(coin VERSION 1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

add_subdirectory(leveldb)

include_directories(
    include
    include/Crypto
    include/net
    include/db
    
    src
    src/db
    src/crypto
    src/net
    leveldb/include
    )

set(SRCS
    src/Wallet.cpp
    src/Block.cpp
    src/BlockChain.cpp
    src/Transaction.cpp
    src/Mainer.cpp
    src/db/IDataBase.cpp
    src/db/LeveldbDataBase.cpp
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
    include/BlockChain.hpp
    include/Transaction.hpp
    include/Mainer.hpp
    include/db/IDataBase.hpp
    include/db/LeveldbDataBase.hpp
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


#===========LIBRARY=================
add_library(coin STATIC ${SRCS} ${HEADERS})

#=============COIN==================
add_executable(main main.cpp)
target_link_libraries(main coin leveldb)



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
enable_testing()

#=============TESTS=================
add_executable(block_test test/block_test.cpp)
target_link_libraries(block_test coin)
add_test(NAME Test1 COMMAND block_test)

add_executable(transaction_test test/transaction_test.cpp)
target_link_libraries(transaction_test coin)
add_test(NAME Test2 COMMAND transaction_test)

add_executable(db_test test/db_test.cpp)
target_link_libraries(db_test coin leveldb)
add_test(NAME Test3 COMMAND db_test)

install(TARGETS block_test transaction_test db_test
        RUNTIME DESTINATION tests)




