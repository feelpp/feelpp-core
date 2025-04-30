
include(FetchContent)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/macro.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/importDependenciesHelpers.cmake)

# create interface lib
add_library(${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies INTERFACE)
add_library(${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions INTERFACE)
target_link_libraries(${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies INTERFACE ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions )
if (WIN32)
  # TODO: check if really good way??
  target_compile_definitions(${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions INTERFACE _USE_MATH_DEFINES )
endif()


# disable dependencies with EMSCRIPTEN
if (EMSCRIPTEN)
  set(FEELPP_ENBALE_DEFAULT_OPTION_LIBASSERT OFF)
endif()

# setup dependencies options
setupCmakeOptionDependencies(
  #PREFIX ${FEELPP_CORE_CMAKE_PREFIX}
  REQUIRED NAPP FMT NLOHMANN_JSON SPDLOG EIGEN3
  OPTIONAL CPR LIBASSERT CATCH2
)
setupCmakeOptionDependencies(
  USE_SYSTEM REQUIRED BOOST
)

importDependencies(
  PREFIX ${FEELPP_CORE_CMAKE_PREFIX}
  TARGET_DEPENDENCIES ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies
  TARGET_DEFINITIONS ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions
  REQUIRED NAPP FMT NLOHMANN_JSON SPDLOG BOOST EIGEN3
  OPTIONAL CPR LIBASSERT # CATCH2
)

# required dependencies
# importDependency_NAPP( ${FEELPP_USE_SYSTEM_NAPP} ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions ${FEELPP_CORE_CMAKE_PREFIX} )
# importDependency_FMT( ${FEELPP_USE_SYSTEM_FMT} ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions ${FEELPP_CORE_CMAKE_PREFIX})
# importDependency_BOOST( ${FEELPP_USE_SYSTEM_BOOST} ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions ${FEELPP_CORE_CMAKE_PREFIX} )
# importDependency_NLOHMANN_JSON( ${FEELPP_USE_SYSTEM_NLOHMANN_JSON} ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions ${FEELPP_CORE_CMAKE_PREFIX} )
# importDependency_SPDLOG( ${FEELPP_USE_SYSTEM_SPDLOG} ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions ${FEELPP_CORE_CMAKE_PREFIX} )
# importDependency_EIGEN3( ${FEELPP_USE_SYSTEM_EIGEN3} ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions ${FEELPP_CORE_CMAKE_PREFIX} )

# # optional dependencies
# if ( FEELPP_ENABLE_CPR )
#   importDependency_CPR( ${FEELPP_USE_SYSTEM_CPR} ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions ${FEELPP_CORE_CMAKE_PREFIX} )
# endif()
# if ( FEELPP_ENABLE_LIBASSERT )
#   importDependency_LIBASSERT( ${FEELPP_USE_SYSTEM_LIBASSERT} ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions ${FEELPP_CORE_CMAKE_PREFIX} )
# endif()

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
    NAMESPACE Feelpp::
    DESTINATION ${FEELPP_CORE_CONFIG_INSTALL_DIR}
    )
  export(EXPORT ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_${_suffix}-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${FEELPP_CORE_LIBRARY_PREFIX}_third_party_${_suffix}Targets.cmake"
    NAMESPACE Feelpp::
    )
endforeach()
