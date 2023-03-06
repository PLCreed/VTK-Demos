set(Target_Name extend)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Header files
set(HDRS_FILES
    vtkLogger.h
)

# Source files
set(SRCS_FILES
    vtkLogger.cxx
)

add_library(${Target_Name} ${HDRS_FILES} ${SRCS_FILES})

target_link_libraries(${Target_Name} ${VTK_LIBRARIES})
target_include_directories(${Target_Name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

install(TARGETS ${Target_Name} LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX})