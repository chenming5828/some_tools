cmake_minimum_required( VERSION 2.8 )
project(test_gain )

set( CMAKE_BUILD_TYPE Release )
set( CMAKE_CXX_FLAGS "-std=c++11 -O3" )

find_package( OpenCV )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( test_gain gain.cpp )
target_link_libraries( test_gain ${OpenCV_LIBS} )
