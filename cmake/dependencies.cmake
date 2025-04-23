include(FetchContent)

add_library(${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies INTERFACE)
add_library(${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions INTERFACE)
target_link_libraries(${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies INTERFACE ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions )
if (WIN32)
  # TODO: check if really good way??
  target_compile_definitions(${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions INTERFACE _USE_MATH_DEFINES )
endif()

function(printDependencySection DEPNAME IS_BEGIN )
  if (${IS_BEGIN})
    set(SECTION_STATE "started")
  else()
    set(SECTION_STATE "finished")
  endif()
  message(STATUS "============================================================")
  message(STATUS "||----> ${DEPNAME} : configure ${SECTION_STATE}")
  message(STATUS "============================================================")
endfunction()
function(printDependencySectionBegin DEPNAME )
  printDependencySection( ${DEPNAME} TRUE)
endfunction()
function(printDependencySectionEnd DEPNAME )
  printDependencySection( ${DEPNAME} FALSE)
endfunction()

#--------------------------------------
# Napp
#--------------------------------------
printDependencySectionBegin( "Napp" )
# if ( ${FEELPP_CORE_CMAKE_PREFIX}_USE_SYSTEM_NAPP )
#   find_package(napp REQUIRED)
# else()
#   FetchContent_Declare(napp GIT_REPOSITORY https://github.com/vincentchabannes/napp.git GIT_TAG v0.3.0 GIT_SHALLOW ON )
#   FetchContent_MakeAvailable(napp)
#   target_compile_definitions(${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions INTERFACE ${FEELPP_CORE_CMAKE_PREFIX}_USE_INTERNAL_NAPP )
# endif()
if ( TARGET napp::napp )
  target_link_libraries( ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies INTERFACE napp::napp )
  target_compile_definitions( ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions INTERFACE ${FEELPP_CORE_CMAKE_PREFIX}_HAS_NAPP )
endif()
printDependencySectionEnd( "Napp" )




# install and export dependencies and definitions targets
foreach( _suffix dependencies definitions )
  install(TARGETS ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_${_suffix}
    EXPORT ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_${_suffix}-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
  install(EXPORT ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_${_suffix}-targets
    NAMESPACE Ktirio-Geom::
    DESTINATION ${FEELPP_CORE_CONFIG_INSTALL_DIR}
    )
  export(EXPORT ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_${_suffix}-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${FEELPP_CORE_LIBRARY_PREFIX}_third_party_${_suffix}Targets.cmake"
    NAMESPACE Ktirio-Geom::
    )
endforeach()
