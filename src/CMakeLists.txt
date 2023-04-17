include(CMakePrintHelpers)

add_subdirectory(auxiliary)

add_lib(project_lib
        SOURCES
        remote/RemoteHostListenerService.cpp
        remote/RemoteHostConnectionController.cpp
        client/ClientConnectionController.cpp
        client/ServiceConnector.cpp
        PeerFactory.cpp
        ShellHandler.cpp

        DEPENDS
        auxiliary
        )