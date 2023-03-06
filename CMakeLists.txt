cmake_minimum_required(VERSION 3.0.0)

project(VTK-Demos VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

# Global CMake options
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# build target dir
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# Configure Qt
find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork QtXml REQUIRED)
include(${QT_USE_FILE})

# Configure VTK
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
message("-- Found VTK VERSION: ${VTK_VERSION}")

# vtk qt version
if("${VTK_QT_VERSION}" STREQUAL "")
    message(FATAL_ERROR "VTK was not built with Qt")
else()
    message("-- Found VTK QT VERSION: Qt${VTK_QT_VERSION}")
endif()

# rpath
set(CMAKE_SKIP_BUILD_RPATH FALSE) # 编译时加上RPATH
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) # 编译时RPATH不使用安装的RPATH
set(CMAKE_INSTALL_RPATH $ORIGIN:$ORIGIN/../:$VTK_DIR/lib) # 安装时加上RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) # 安装的执行文件加上RPATH

add_subdirectory(src)

# # CTest
# include(CTest)
# enable_testing()

# # CPack
# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
