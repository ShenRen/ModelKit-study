project( testModelKit )
cmake_minimum_required( VERSION 3.2 )
set ( CMAKE_BUILD_TYPE Debug )
add_definitions ( -Wall )
find_package ( Qt5Core REQUIRED )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories ( ${SDK_DIR})

file(GLOB_RECURSE SRC
    "*.h"
    "*.hpp"
    "*.cpp"
)

add_library ( ${PROJECT_NAME} SHARED ${SRC} )
target_link_libraries ( ${PROJECT_NAME} Qt5::Core )
