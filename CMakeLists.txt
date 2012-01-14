cmake_minimum_required( VERSION 2.6 )
project( json_stream_writer )
enable_testing()

set(CMAKE_CXX_FLAGS "-g -Wall -O3")        ## Optimize

include_directories( include )
include_directories( ext/gtest/include )

add_subdirectory( ext/gtest )
add_subdirectory( src )
add_subdirectory( test )
