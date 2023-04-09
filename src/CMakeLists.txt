include(CMakePrintHelpers)

add_subdirectory(auxiliary)

add_lib(project_lib
        SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/TaskScheduler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/TaskExecutor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/CommandLineTaskHandler.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RemoteHostListenerService.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/RemoteHostConnectionController.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ClientConnectionController.cpp
        DEPENDS
        auxiliary
        )