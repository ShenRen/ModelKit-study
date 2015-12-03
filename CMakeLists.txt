project( ModelKit )

# cmake requirements
cmake_minimum_required( VERSION 3.2 )

# build type
set ( CMAKE_BUILD_TYPE Debug )

# include
include_directories ( ${RPMANAGER_SDK_DIR} )
find_package ( Qt5Core REQUIRED )

# compile
add_definitions ( -Wall )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# source files
file(GLOB_RECURSE SRC
    "*.h"
    "*.hpp"
    "*.cpp"
)

# project
add_library ( ${PROJECT_NAME} STATIC ${SRC} )

# linking
target_link_libraries ( ${PROJECT_NAME} Qt5::Core)
