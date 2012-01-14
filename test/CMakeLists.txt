function(add_cppgtest TITLE NAME LIBS)
  add_executable(${NAME} ${NAME}.cpp)
  target_link_libraries(${NAME} gtest gtest_main ${LIBS})
  add_test(${TITLE} ${NAME} )
endfunction(add_cppgtest)

add_cppgtest(Writer unit_writer json_writer)
