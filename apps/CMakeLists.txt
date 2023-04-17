include(CMakePrintHelpers)

add_app(${CMAKE_PROJECT_NAME}
        ssh-service.cpp
        )

add_app(test_program
        test_program.cpp
        )


add_app(process_control
        process_control.cpp
        )

