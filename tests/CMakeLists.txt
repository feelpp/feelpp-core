
file(GLOB files_feelcore test-feelcore*.cpp)
file(GLOB files_ftxui test-ftxui*.cpp)
file(GLOB files_hdf5 test-hdf5*.cpp)

foreach(file ${files_feelcore} ${files_ftxui} ${files_hdf5})
  get_filename_component(file_basename ${file} NAME_WE)
  #string(REGEX REPLACE "unit-([^$]+)" "test-\\1" testcase ${file_basename})
  set(testcase feelpp_core_${file_basename})
  add_executable(${testcase}  ${file})
  target_link_libraries(${testcase} PRIVATE ${FEELPP_CORE_LIBRARY} Catch2::Catch2WithMain)

  add_test(NAME "${testcase}"
    COMMAND ${testcase}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endforeach()
