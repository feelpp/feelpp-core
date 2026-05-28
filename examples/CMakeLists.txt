
file(GLOB files feelcore*.cpp)

foreach(file ${files})
  get_filename_component(file_basename ${file} NAME_WE)
  set(example feelpp_core_${file_basename})
  add_executable(${example}  ${file})
  target_link_libraries(${example} PRIVATE ${FEELPP_CORE_LIBRARY} )
endforeach()
