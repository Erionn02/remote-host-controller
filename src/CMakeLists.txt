include(CMakePrintHelpers)

add_subdirectory(auxiliary)

add_lib(project_lib
        SOURCES
        remote/RemoteHostListenerService.cpp
        remote/RemoteHostConnectionController.cpp
        remote/ShellHandler.cpp
        remote/StreamReader.cpp
        client/ClientConnectionController.cpp
        client/ServiceConnector.cpp
        PeerFactory.cpp

        DEPENDS
        auxiliary
        )