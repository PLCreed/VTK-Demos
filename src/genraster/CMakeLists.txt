set(Target_Name genraster)

# Header files
set(HDRS_FILES
    colormap.h
    interval.h
    point3d.h
    scalemap.h
    rasterdata.h
    plotrasteritembase.h
    plotspectrogram.h
)

# Source files
set(SRCS_FILES
    main.cpp
    colormap.cpp
    interval.cpp
    point3d.cpp
    scalemap.cpp
    rasterdata.cpp
    plotrasteritembase.cpp
    plotspectrogram.cpp
)

add_executable(${Target_Name}
    ${SRCS_FILES}
    ${HDRS_FILES}
)
target_link_libraries(${Target_Name} ${QT_LIBRARIES} ${VTK_LIBRARIES})

install(TARGETS ${Target_Name} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})