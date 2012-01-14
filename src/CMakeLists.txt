add_executable(sample sample.cpp)
add_library(json_writer json_writer.cpp)

target_link_libraries(sample json_writer)