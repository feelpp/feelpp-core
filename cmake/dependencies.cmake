include(FetchContent)
include(${FEELPP_CORE_CMAKE_DIR}/macro.cmake)
include(${FEELPP_CORE_CMAKE_DIR}/importDependenciesHelpers.cmake)

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
  set(FEELPP_ENBALE_DEFAULT_OPTION_MPI OFF)
  set(FEELPP_ENBALE_DEFAULT_OPTION_LIBASSERT OFF)
endif()

# setup dependencies options
setupCmakeOptionDependencies(
  #PREFIX ${FEELPP_CORE_CMAKE_PREFIX}
  REQUIRED NAPP FMT NLOHMANN_JSON SPDLOG EIGEN3 FTXUI
  OPTIONAL CPR LIBASSERT CATCH2
)
setupCmakeOptionDependencies(
  USE_SYSTEM
  REQUIRED LIBZIP BOOST HDF5
  OPTIONAL MPI
)

# base
importDependencies(
  PREFIX ${FEELPP_CORE_CMAKE_PREFIX}
  TARGET_DEPENDENCIES ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies
  TARGET_DEFINITIONS ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions
  OPTIONAL MPI
)

importDependencies(
  PREFIX ${FEELPP_CORE_CMAKE_PREFIX}
  TARGET_DEPENDENCIES ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies
  TARGET_DEFINITIONS ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions
  REQUIRED HDF5 NAPP FMT LIBZIP NLOHMANN_JSON SPDLOG BOOST EIGEN3 FTXUI
  OPTIONAL CPR LIBASSERT
)

if ( ${FEELPP_CORE_CMAKE_PREFIX}_ENABLE_TESTS )
  importDependencies(
    PREFIX ${FEELPP_CORE_CMAKE_PREFIX}
    TARGET_DEPENDENCIES ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_dependencies
    TARGET_DEFINITIONS ${FEELPP_CORE_LIBRARY_PREFIX}_third_party_definitions
    REQUIRED CATCH2
  )
endif()

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
