function(setupCmakeOptionDependencies)
  set(options USE_SYSTEM)
  set(oneValueArgs "")
  set(multiValueArgs REQUIRED OPTIONAL)
  cmake_parse_arguments(PARSE_ARGV 0 arg
    "${options}" "${oneValueArgs}" "${multiValueArgs}"
  )

  set(_use_system OFF)
  if( arg_USE_SYSTEM)
    set(_use_system ON)
  endif()

  if ( arg_REQUIRED )
    foreach(_dep IN LISTS arg_REQUIRED)
      option(FEELPP_USE_SYSTEM_${_dep} "use ${_dep} from system" ${_use_system})
    endforeach()
  endif()

  if ( arg_OPTIONAL )
    foreach(_dep IN LISTS arg_OPTIONAL)
      set(_enabled ON)
      if ( DEFINED FEELPP_ENBALE_DEFAULT_OPTION_${_dep} )
        set(_enabled ${FEELPP_ENBALE_DEFAULT_OPTION_${_dep}})
      endif()
      option(FEELPP_ENABLE_${_dep} "enable third party ${_dep}" ${_enabled})
      option(FEELPP_USE_SYSTEM_${_dep} "use ${_dep} from system" ${_use_system})
    endforeach()
  endif()
endfunction()


# call a function with a dynamic name
function(feelpp_detail_callFunction _id)
  if (NOT COMMAND ${_id})
    message(FATAL_ERROR "Unsupported function/macro \"${_id}\"")
  else()
    set(_helper "${CMAKE_BINARY_DIR}/helpers/macro_helper_${_id}.cmake")
    if (NOT EXISTS "${_helper}")
      file(WRITE "${_helper}" "${_id}(\$\{ARGN\})\n")
    endif()
    include("${_helper}")
  endif()
endfunction()

# call a macro with a dynamic name
macro(feelpp_detail_callMacro)
  set(options "")
  set(oneValueArgs NAME)
  set(multiValueArgs ARGUMENTS)
  cmake_parse_arguments(arg_callMacro
    "${options}" "${oneValueArgs}" "${multiValueArgs}"
    ${ARGN}
  )
  set( _id ${arg_callMacro_NAME} )
  set( _args ${arg_callMacro_ARGUMENTS})
  if (NOT COMMAND ${_id})
    message(FATAL_ERROR "Unsupported function/macro \"${_id}\"")
  else()
    set(_helper "${CMAKE_BINARY_DIR}/helpers/macro_helper_${_id}.cmake")
    if (NOT EXISTS "${_helper}")
      file(WRITE "${_helper}" "${_id}(\$\{_args\} )\n")
    endif()
    include("${_helper}")
  endif()
endmacro()

macro(importDependencies)
  set(options "")
  set(oneValueArgs PREFIX TARGET_DEPENDENCIES TARGET_DEFINITIONS)
  set(multiValueArgs REQUIRED OPTIONAL)
  cmake_parse_arguments(arg
    "${options}" "${oneValueArgs}" "${multiValueArgs}"
    ${ARGN}
  )
  set(_cmakePrefix ${arg_PREFIX})
  set(_targetDependencies ${arg_TARGET_DEPENDENCIES})
  set(_targetDefinitions ${arg_TARGET_DEFINITIONS})
  if ( arg_REQUIRED )
    foreach(_dep IN LISTS arg_REQUIRED)
      if ( NOT FEELPP_HAS_${_dep} )
        feelpp_detail_callMacro(NAME importDependency_${_dep} ARGUMENTS ${FEELPP_USE_SYSTEM_${_dep}} ${_targetDependencies} ${_targetDefinitions} ${_cmakePrefix} )
      endif()
    endforeach()
  endif()
  if ( arg_OPTIONAL )
    foreach(_dep IN LISTS arg_OPTIONAL)
      if ( FEELPP_ENABLE_${_dep} )
        if ( NOT FEELPP_HAS_${_dep} )
          feelpp_detail_callMacro(NAME importDependency_${_dep} ARGUMENTS ${FEELPP_USE_SYSTEM_${_dep}} ${_targetDependencies} ${_targetDefinitions} ${_cmakePrefix} )
        endif()
      endif( )
    endforeach()
  endif()
endmacro(importDependencies)



function(feelpp_configure_dependencies_package_config _cmakePrefix _outputDir )
  # create importDependenciesPackageConfig.cmake (need to apply configure_file twice)
  set(FEELPP_TEMPLATE_CMAKE_PREFIX ${_cmakePrefix})
  configure_file(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/importDependenciesPackageConfig.cmake.in  ${_outputDir}/importDependenciesPackageConfig.cmake.in  @ONLY)
  configure_file(${_outputDir}/importDependenciesPackageConfig.cmake.in  ${_outputDir}/importDependenciesPackageConfig.cmake  @ONLY)
endfunction()



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


function(feelpp_updateImportDependencyForUseBis _depName _useSystem _cmakeVariablePrefix)
  #target_link_libraries( ${_target_dependencies} INTERFACE ${_depTarget} )
  #target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_${_depName} )
  set( ${_cmakeVariablePrefix}_MANAGE_${_depName} 1 PARENT_SCOPE )
  if ( NOT ${_useSystem} )
    set( FEELPP_USE_INTERNAL_${_depName} 1 PARENT_SCOPE )
  endif()
  set( FEELPP_HAS_${_depName} 1 PARENT_SCOPE)
endfunction()

function(feelpp_updateImportDependencyForUse _depName _depTarget _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  target_link_libraries( ${_target_dependencies} INTERFACE ${_depTarget} )
  target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_${_depName} )
  set( ${_cmakeVariablePrefix}_MANAGE_${_depName} 1 PARENT_SCOPE )
  if ( NOT ${_useSystem} )
    set( FEELPP_USE_INTERNAL_${_depName} 1 PARENT_SCOPE )
  endif()
  set( FEELPP_HAS_${_depName} 1 PARENT_SCOPE)
endfunction()

#--------------------------------------
# Napp
#--------------------------------------
macro(importDependency_NAPP _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "Napp" )
  if ( ${_useSystem} )
    find_package(napp REQUIRED)
  else()
    FetchContent_Declare(napp GIT_REPOSITORY https://github.com/vincentchabannes/napp.git GIT_TAG v0.3.0 GIT_SHALLOW ON )
    FetchContent_MakeAvailable(napp)
  endif()
  if ( TARGET napp::napp )
    feelpp_updateImportDependencyForUse( NAPP napp::napp ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "Napp" )
endmacro(importDependency_NAPP)

#--------------------------------------
# Fmt
#--------------------------------------
macro(importDependency_FMT _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "Fmt" )
  if ( ${_useSystem} )
    find_package(fmt REQUIRED)
  else()
    FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git GIT_TAG 10.2.1 GIT_SHALLOW ON )
    FetchContent_MakeAvailable(fmt)
  endif()
  if ( TARGET fmt::fmt )
    feelpp_updateImportDependencyForUse( FMT fmt::fmt ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "Fmt" )
endmacro(importDependency_FMT)


#--------------------------------------
# libzip
#--------------------------------------
macro(importDependency_LIBZIP _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "libzip" )
  if ( ${_useSystem} )
    find_package(libzip REQUIRED)
  else()
    # disable these variables (unfortunately impact other project as Cgal)
    option(BUILD_EXAMPLES "Build examples" OFF)
    option(BUILD_DOC "Build documentation" OFF)
    FetchContent_Declare(libzip GIT_REPOSITORY https://github.com/nih-at/libzip.git GIT_TAG v1.11.4 GIT_SHALLOW ON )
    FetchContent_MakeAvailable(libzip)
  endif()
  if ( TARGET libzip::zip )
    feelpp_updateImportDependencyForUse( LIBZIP libzip::zip ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "libzip" )
endmacro(importDependency_LIBZIP)

#--------------------------------------
# Boost
#--------------------------------------
macro(importDependency_BOOST _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "Boost" )
  SET(BOOST_MIN_VERSION "1.74.0")
  set(BOOST_COMPONENTS program_options)
  if ( ${_useSystem} )
    find_package(Boost 1.74.0 REQUIRED COMPONENTS program_options) #OPTIONAL_COMPONENTS program_options)
    # target_link_libraries( ${_target_dependencies} INTERFACE ${Boost_LIBRARIES})
    # target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_BOOST )
    # # if ( Boost_program_options_FOUND )
    # #   target_compile_definitions( ${_target_definitions}  INTERFACE FEELPP_HAS_BOOST_PROGRAM_OPTIONS )
    # # endif()
    # set( ${_cmakeVariablePrefix}_MANAGE_BOOST ON )
  else()
    message(FATAL_ERROR "Boost dependency can be enabled only from system")
  endif()
  #set( _useSystem TRUE)
  feelpp_updateImportDependencyForUse( BOOST ${Boost_LIBRARIES} ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )

  printDependencySectionEnd( "Boost" )
endmacro(importDependency_BOOST)


#--------------------------------------
# Cpr
#--------------------------------------
macro(importDependency_CPR _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "Cpr" )
  if ( ${_useSystem} )
    find_package(cpr REQUIRED)
  else()
    FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git GIT_TAG 1.11.1 GIT_SHALLOW ON )
    set(CPR_USE_SYSTEM_CURL ON)
    if ( EMSCRIPTEN )
      set( CPR_ENABLE_SSL OFF )
      set( CPR_USE_SYSTEM_CURL OFF )
    endif()
    # if ( WIN32 )
    #   set( CPR_USE_SYSTEM_CURL OFF )
    # endif()
    FetchContent_MakeAvailable(cpr)
    export(TARGETS cpr
      FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${_target_dependencies}_cprTargets.cmake"
      NAMESPACE cpr::
    )
  endif()
  if ( TARGET cpr::cpr )
    feelpp_updateImportDependencyForUse( CPR cpr::cpr ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "Cpr" )
endmacro(importDependency_CPR)

#--------------------------------------
# nlohmann_json
#--------------------------------------
macro(importDependency_NLOHMANN_JSON _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "nlohmann_json" )
  if ( ${_useSystem} )
    find_package(nlohmann_json REQUIRED)
  else()
    FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.3 GIT_SHALLOW ON )
    set( JSON_Install ON )
    #  set(NLOHMANN_JSON_CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/nlohmann_json CACHE )
    #  set(NLOHMANN_JSON_PKGCONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
    FetchContent_MakeAvailable(json)
  endif()
  if ( TARGET nlohmann_json::nlohmann_json )
    feelpp_updateImportDependencyForUse( NLOHMANN_JSON nlohmann_json::nlohmann_json ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "nlohmann_json" )
endmacro(importDependency_NLOHMANN_JSON)

#--------------------------------------
# spdlog
#--------------------------------------
macro(importDependency_SPDLOG _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "spdlog" )
  if ( ${_useSystem} )
    find_package(spdlog REQUIRED)
  else()
    FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git
      GIT_TAG v1.14.1 GIT_SHALLOW ON )
    set( SPDLOG_INSTALL ON)
    if ( FEELPP_HAS_FMT )
      set(SPDLOG_FMT_EXTERNAL ON)
    endif()
    FetchContent_MakeAvailable(spdlog)
  endif()
  if ( TARGET spdlog::spdlog )
    feelpp_updateImportDependencyForUse( SPDLOG spdlog::spdlog ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "spdlog" )
endmacro(importDependency_SPDLOG)

#--------------------------------------
# libassert
#--------------------------------------
macro(importDependency_LIBASSERT _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "libassert" )
  if ( ${_useSystem} )
    find_package(libassert REQUIRED)
  else()
    FetchContent_Declare(libassert GIT_REPOSITORY https://github.com/jeremy-rifkin/libassert.git
      GIT_TAG v2.1.5 GIT_SHALLOW ON )
    set(CPPTRACE_USE_EXTERNAL_ZSTD ON)
    FetchContent_MakeAvailable(libassert)
    # export(TARGETS libassert-lib cpptrace-lib dwarf
    #   FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${_target_dependencies}_assertTargets.cmake"
    #   NAMESPACE libassert::
    # )
  endif()
  if ( TARGET libassert::assert )
    feelpp_updateImportDependencyForUse( LIBASSERT libassert::assert ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "libassert" )
endmacro(importDependency_LIBASSERT)

#--------------------------------------
# Eigen3
#--------------------------------------
macro(importDependency_EIGEN3 _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "Eigen3" )
  if ( ${_useSystem} )
    find_package(Eigen3 3.1.0 REQUIRED) #(3.1.0 or greater)
  else()
    FetchContent_Declare( eigen3 GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
      #GIT_TAG 3.4.0
      GIT_TAG 02bcf9b5918d46016bc88e5e9abebb6caa5a80b7
      #GIT_SHALLOW ON
    )
    set(EIGEN_BUILD_CMAKE_PACKAGE ON)
    set(EIGEN_BUILD_PKGCONFIG ON)
    FetchContent_MakeAvailable(eigen3)
  endif()
  if ( TARGET Eigen3::Eigen )
    set( EIGEN3_FOUND ON )
    feelpp_updateImportDependencyForUse( EIGEN3 Eigen3::Eigen ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "Eigen3" )
endmacro(importDependency_EIGEN3)

#--------------------------------------
# CGAL
#--------------------------------------
macro(importDependency_CGAL _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "CGAL" )
  if ( ${_useSystem} )
    find_package(CGAL REQUIRED COMPONENTS Core)
  else()
    FetchContent_Declare( cgal GIT_REPOSITORY https://github.com/CGAL/cgal.git
      GIT_TAG v6.0.1 #  v5.6
      #GIT_TAG 5ffa817b6211a4bcec05faf85726bc4845682d50
      GIT_SHALLOW ON
    )
    set( WITH_CGAL_Qt5 OFF )
    FetchContent_MakeAvailable(cgal)
    set( CGAL_DIR ${CGAL_BINARY_DIR})
    find_package(CGAL PATHS ${CGAL_BINARY_DIR} NO_DEFAULT_PATH REQUIRED COMPONENTS Core )
    if(WIN32 AND NOT UNIX) # Fix windows deploy of dll gmp and mpfr
      install(
        DIRECTORY "$ENV{GMP_DIR}/lib/" #C:/gmp/lib/" #$ENV{GMP_DIR}/lib/
        TYPE BIN
        FILES_MATCHING REGEX "[^\\\\/.]\\.[dD][lL][lL]$"
      )
    endif()
  endif()

  if ( CGAL_FOUND )
    # workaround with CGAL cmake : feelpp has EIGEN3 but not cmake var EIGEN3_FOUND
    if (FEELPP_HAS_EIGEN3 AND NOT EIGEN3_FOUND )
      set( EIGEN3_FOUND 1)
    endif()
    include(CGAL_Eigen3_support)
    if ( EMSCRIPTEN )
      target_compile_definitions( CGAL INTERFACE CGAL_ALWAYS_ROUND_TO_NEAREST)
    endif()
   feelpp_updateImportDependencyForUse( CGAL "CGAL::CGAL;CGAL::CGAL_Core;CGAL::Eigen3_support" ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  else()
    message( WARNING "ThirdParty CGAL not found")
  endif()
  printDependencySectionEnd( "CGAL" )
endmacro(importDependency_CGAL)

#--------------------------------------
# BVH
#--------------------------------------
macro(importDependency_BVH _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "BVH" )
  if ( ${_useSystem} )
    find_package(bvh REQUIRED)
  else()
    FetchContent_Declare(bvh GIT_REPOSITORY https://github.com/feelpp/bvh.git GIT_TAG 327b67b1d8d798e4dab1686b0dd022a1e0cbcefe)
    FetchContent_MakeAvailable(bvh)
  endif()
  if ( TARGET bvh )
    feelpp_updateImportDependencyForUse( BVH bvh ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "BVH" )
endmacro(importDependency_BVH)

# #--------------------------------------
# # CURL
# #--------------------------------------
# macro(importDependency_CURL _target_dependencies _target_definitions)
# if ( NOT EMSCRIPTEN )
# printDependencySectionBegin("CURL")
# find_package(CURL REQUIRED)
# if (CURL_FOUND)
#     target_link_libraries( ${_target_dependencies} INTERFACE CURL::libcurl)
#     target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_CURL)
# else()
#     message(WARNING "ThirdParty CURL not found")
# endif()
# printDependencySectionEnd("CURL")
# endif()
# endmacro(importDependency_CURL)

#--------------------------------------
# PNG
#--------------------------------------
macro(importDependency_PNG _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin("PNG")
  if ( ${_useSystem} )
    find_package(PNG REQUIRED)
  else()
    message(FATAL_ERROR "PNG dependency can be enabled only from system")
  endif()
  if ( TARGET PNG::PNG  )
    feelpp_updateImportDependencyForUse( PNG PNG::PNG ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd("PNG")
endmacro(importDependency_PNG)

#--------------------------------------
# Catch2
#--------------------------------------
macro(importDependency_CATCH2 _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "Catch2" )
  if ( ${_useSystem} )
    find_package(Catch2 3 REQUIRED)
  else()
    FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.8.1
      PATCH_COMMAND git apply "${FEELPP_CORE_CMAKE_DIR}/catch2.patch" UPDATE_DISCONNECTED 1
    )
    FetchContent_MakeAvailable(Catch2)
    export(TARGETS Catch2 Catch2WithMain
      FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${_target_dependencies}_catch2Targets.cmake"
      NAMESPACE Catch2::
    )
  endif()
  if ( TARGET Catch2::Catch2 )
    feelpp_updateImportDependencyForUse( CATCH2  "Catch2::Catch2;Catch2::Catch2WithMain" ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "Catch2" )
endmacro(importDependency_CATCH2)

#--------------------------------------
# FTXUI
#--------------------------------------
macro(importDependency_FTXUI _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "FTXUI" )
  if ( ${_useSystem} )
    find_package(ftxui REQUIRED)
  else()
    FetchContent_Declare(ftxui GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI GIT_TAG v6.1.9)
    FetchContent_MakeAvailable(ftxui)
  endif()
  if ( TARGET ftxui::screen )
    feelpp_updateImportDependencyForUse( FTXUI  "ftxui::screen;ftxui::dom;ftxui::component" ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "FTXUI" )
endmacro(importDependency_FTXUI)

#--------------------------------------
# Assimp
#--------------------------------------
macro(importDependency_ASSIMP _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "ASSIMP" )
  if ( ${_useSystem} )
    find_package(assimp REQUIRED)
  else()
    message(FATAL_ERROR "Assimp dependency can be enabled only from system")
  endif()
  if ( TARGET assimp::assimp )
    feelpp_updateImportDependencyForUse( ASSIMP  "assimp::assimp" ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "ASSIMP" )
endmacro(importDependency_ASSIMP)


#--------------------------------------
# MPI
#--------------------------------------
macro(importDependency_MPI _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "MPI" )
  if ( ${_useSystem} )
    # Disable searching for MPI-2 C++ bindings
    set(MPI_CXX_SKIP_MPICXX TRUE)
    find_package(MPI REQUIRED)
  else()
    message(FATAL_ERROR "MPI dependency can be enabled only from system")
  endif()
  if ( TARGET MPI::MPI_C )
    feelpp_updateImportDependencyForUse( MPI  "MPI::MPI_C" ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  endif()
  printDependencySectionEnd( "MPI" )
endmacro(importDependency_MPI)

#--------------------------------------
# HDF5
#--------------------------------------
macro(importDependency_HDF5 _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "HDF5" )
  if ( ${_useSystem} )
    if ( NOT EMSCRIPTEN )
      set(HDF5_PREFER_PARALLEL TRUE)
      find_package(HDF5 REQUIRED)
    else()
      find_package(HDF5 REQUIRED CONFIG)
    endif()
  else()
    message(FATAL_ERROR "HDF5 dependency can be enabled only from system")
  endif()
  if ( TARGET HDF5::HDF5 )
    feelpp_updateImportDependencyForUse( HDF5  "HDF5::HDF5" ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  elseif ( TARGET hdf5-static ) # fix emscripten install, to investigate
    feelpp_updateImportDependencyForUse( HDF5  "hdf5-static" ${_useSystem} ${_target_dependencies} ${_target_definitions} ${_cmakeVariablePrefix} )
  else()
    message(FATAL_ERROR "HDF5 target not exists")
  endif()
  printDependencySectionEnd( "HDF5" )
endmacro(importDependency_HDF5)
