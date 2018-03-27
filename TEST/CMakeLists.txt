cmake_minimum_required( VERSION 2.8 )
project(test_gain )

set( CMAKE_BUILD_TYPE Release )
set( CMAKE_CXX_FLAGS "-std=c++11 -O3 -fpermissive" )

# include(ExternalProject)
# ExternalProject_Add(ceres_src
#   GIT_REPOSITORY https://github.com/JakobEngel/dso.git 

# )

find_package( OpenCV )

FIND_PACKAGE(Boost REQUIRED COMPONENTS thread system)




# Include dirs
INCLUDE_DIRECTORIES(
#   include
#   ${Eigen_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
#   ${Sophus_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
 
)

# Set link libraries
LIST(APPEND LINK_LIBS 
  ${OpenCV_LIBS}
#   ${Sophus_LIBRARIES}
  ${Boost_LIBRARIES}
)

add_executable( test_gain gain.cpp )
target_link_libraries( test_gain ${OpenCV_LIBS} )

add_executable( test_brightness brightness.cpp )
target_link_libraries( test_brightness ${OpenCV_LIBS} )

# add_executable( bind bind.cpp )
# target_link_libraries( bind pthread )
add_executable( capture v4l2.cpp )
target_link_libraries( capture ${OpenCV_LIBS} pthread )


add_executable( queue queue.cpp )
target_link_libraries( queue ${OpenCV_LIBS})

add_executable( lock lock.cpp )
target_link_libraries( lock pthread ${LINK_LIBS})

add_executable( warp test_warp.cpp )
target_link_libraries( warp ${OpenCV_LIBS})

add_executable( readmp4 readmp4.cpp )
target_link_libraries( readmp4 ${OpenCV_LIBS})

add_executable( record record.cpp )
target_link_libraries( record ${OpenCV_LIBS})
