set(Target_Name TestVtkEnv)

project(${Target_Name})

# Header files
set(HDRS_FILES
)

# Source files
set(SRCS_FILES
    main.cxx
)

add_executable(${Target_Name}
    ${SRCS_FILES}
    ${HDRS_FILES}
)
target_link_libraries(${Target_Name} ${VTK_LIBRARIES})
install(TARGETS ${Target_Name} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})