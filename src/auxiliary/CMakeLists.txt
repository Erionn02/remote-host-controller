include(CMakePrintHelpers)


add_lib(auxiliary
        SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/socket/ZMQSocket.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/socket/SecureSocket.cpp
        )
