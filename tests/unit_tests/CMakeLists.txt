set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -w -O0 -g3 -no-pie")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_DEBUG} -w -O0 -g3 -no-pie")

add_test(CryptoUnitTests
        SOURCES
        AESCryptographerTest.cpp
        RSACryptographerTest.cpp
        ZMQSocketTest.cpp
        DEPENDS auxiliary
        )


