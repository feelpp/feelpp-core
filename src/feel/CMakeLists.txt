add_subdirectory( feelcore )

add_library(${FEELPP_CORE_LIBRARY} ${FEELPP_CORE_LIB_SRCS} )
add_library(Feelpp::${FEELPP_CORE_LIBRARY} ALIAS ${FEELPP_CORE_LIBRARY})
target_link_libraries(${FEELPP_CORE_LIBRARY} PUBLIC ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies )

target_compile_definitions(${FEELPP_CORE_LIBRARY} PRIVATE FEELPP_CORE_LIB_LIBRARY_EXPORTING)
target_compile_definitions(${FEELPP_CORE_LIBRARY} PRIVATE ${FEELPP_CORE_CMAKE_PREFIX}_DATA_DIR=${${FEELPP_CORE_CMAKE_PREFIX}_DATA_DIR})

target_include_directories(${FEELPP_CORE_LIBRARY} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> )



install(TARGETS ${FEELPP_CORE_LIBRARY} EXPORT ${FEELPP_CORE_LIBRARY}-targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)


set( _dirs feelcore )
foreach(_includedir ${_dirs})
  file(GLOB _hdr ${_includedir}/*.hpp)
  install(FILES ${_hdr} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/feel/${_includedir} COMPONENT Devel)
endforeach()

install(EXPORT ${FEELPP_CORE_LIBRARY}-targets
  NAMESPACE Feelpp::
  DESTINATION ${FEELPP_CORE_CONFIG_INSTALL_DIR} )

# Exporting Targets from the Build Tree
export(EXPORT ${FEELPP_CORE_LIBRARY}-targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${FEELPP_CORE_LIBRARY}Targets.cmake"
  NAMESPACE Feelpp::
)
