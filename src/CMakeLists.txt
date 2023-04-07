include(CMakePrintHelpers)

add_subdirectory(auxiliary)

add_lib(project_lib
        SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/TaskScheduler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/TaskExecutor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CommandLineTaskCreator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ClientService.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PeerToPeerConnection.cpp
        DEPENDS
        auxiliary
        )