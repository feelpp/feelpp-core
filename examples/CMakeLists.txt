
file(GLOB files_tui feelcore-tui*.cpp)

foreach(file ${files_tui})
  get_filename_component(file_basename ${file} NAME_WE)
  set(example feelpp_core_${file_basename})
  add_executable(${example}  ${file})
  target_link_libraries(${example} PRIVATE ${FEELPP_CORE_LIBRARY} )
endforeach()
