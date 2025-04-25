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
function(importDependencyNapp _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "Napp" )
  if ( _useSystem )
    find_package(napp REQUIRED)
  else()
    FetchContent_Declare(napp GIT_REPOSITORY https://github.com/vincentchabannes/napp.git GIT_TAG v0.3.0 GIT_SHALLOW ON )
    FetchContent_MakeAvailable(napp)
    #target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_USE_INTERNAL_NAPP )
  endif()
  if ( TARGET napp::napp )
    target_link_libraries( ${_target_dependencies} INTERFACE napp::napp )
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_NAPP )
    set( ${_cmakeVariablePrefix}_MANAGE_NAPP ON PARENT_SCOPE )
    if ( NOT _useSystem )
      set( ${_cmakeVariablePrefix}_USE_INTERNAL_NAPP ON PARENT_SCOPE )
    endif()
  endif()
  printDependencySectionEnd( "Napp" )
endfunction(importDependencyNapp)

#--------------------------------------
# Fmt
#--------------------------------------
function(importDependencyFmt _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "Fmt" )
  if ( _useSystem )
    find_package(fmt REQUIRED)
  else()
    FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git GIT_TAG 10.2.1 GIT_SHALLOW ON )
    FetchContent_MakeAvailable(fmt)
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_USE_INTERNAL_FMT )
  endif()
  if ( TARGET fmt::fmt )
    target_link_libraries( ${_target_dependencies} INTERFACE fmt::fmt )
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_FMT )
    set( ${_cmakeVariablePrefix}_MANAGE_FMT ON PARENT_SCOPE )
    if ( NOT _useSystem )
      set( ${_cmakeVariablePrefix}_USE_INTERNAL_FMT ON PARENT_SCOPE )
    endif()
  endif()
  printDependencySectionEnd( "Fmt" )
endfunction(importDependencyFmt)

#--------------------------------------
# Boost
#--------------------------------------
function(importDependencyBoost _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "Boost" )
  SET(BOOST_MIN_VERSION "1.74.0")
  set(BOOST_COMPONENTS program_options)
  find_package(Boost 1.74.0 REQUIRED COMPONENTS program_options) #OPTIONAL_COMPONENTS program_options)
  target_link_libraries( ${_target_dependencies} INTERFACE ${Boost_LIBRARIES})
  target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_BOOST )
  # if ( Boost_program_options_FOUND )
  #   target_compile_definitions( ${_target_definitions}  INTERFACE FEELPP_HAS_BOOST_PROGRAM_OPTIONS )
  # endif()
  set( ${_cmakeVariablePrefix}_MANAGE_BOOST ON PARENT_SCOPE )
  # forward variables scope (required in package config)
  set(BOOST_MIN_VERSION ${BOOST_MIN_VERSION} PARENT_SCOPE)
  set(BOOST_COMPONENTS ${BOOST_COMPONENTS} PARENT_SCOPE )
  printDependencySectionEnd( "Boost" )
endfunction(importDependencyBoost)


#--------------------------------------
# Cpr
#--------------------------------------
function(importDependencyCpr _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "Cpr" )
  if ( _useSystem )
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
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_USE_INTERNAL_CPR )
    export(TARGETS cpr
      FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${_target_dependencies}_cprTargets.cmake"
      NAMESPACE cpr::
    )
  endif()
  if ( TARGET cpr::cpr )
    target_link_libraries( ${_target_dependencies} INTERFACE cpr::cpr )
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_CPR )
    set( ${_cmakeVariablePrefix}_MANAGE_CPR ON PARENT_SCOPE )
    if ( NOT _useSystem )
      set( ${_cmakeVariablePrefix}_USE_INTERNAL_CPR ON PARENT_SCOPE )
    endif()
  endif()
  printDependencySectionEnd( "Cpr" )
endfunction(importDependencyCpr)

#--------------------------------------
# nlohmann_json
#--------------------------------------
function(importDependencyNlohmannJson _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "nlohmann_json" )
  if ( _useSystem )
    find_package(nlohmann_json REQUIRED)
  else()
    FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG v3.11.3 GIT_SHALLOW ON )
    set( JSON_Install ON )
    #  set(NLOHMANN_JSON_CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/nlohmann_json CACHE )
    #  set(NLOHMANN_JSON_PKGCONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
    FetchContent_MakeAvailable(json)
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_USE_INTERNAL_NLOHMANN_JSON )
  endif()
  if ( TARGET nlohmann_json::nlohmann_json )
    target_link_libraries( ${_target_dependencies} INTERFACE nlohmann_json::nlohmann_json )
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_NLOHMANN_JSON )
    set( ${_cmakeVariablePrefix}_MANAGE_NLOHMANN_JSON ON PARENT_SCOPE )
    if ( NOT _useSystem )
      set( ${_cmakeVariablePrefix}_USE_INTERNAL_NLOHMANN_JSON ON PARENT_SCOPE )
    endif()
  endif()
  printDependencySectionEnd( "nlohmann_json" )
endfunction(importDependencyNlohmannJson)

#--------------------------------------
# spdlog
#--------------------------------------
function(importDependencySpdlog _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "spdlog" )
  if ( _useSystem )
    find_package(spdlog REQUIRED)
  else()
    FetchContent_Declare(spdlog GIT_REPOSITORY https://github.com/gabime/spdlog.git
      GIT_TAG v1.14.1 GIT_SHALLOW ON )
    set( SPDLOG_INSTALL ON)
    FetchContent_MakeAvailable(spdlog)
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_USE_INTERNAL_SPDLOG )
  endif()
  if ( TARGET spdlog::spdlog )
    target_link_libraries( ${_target_dependencies} INTERFACE spdlog::spdlog )
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_SPDLOG )
    set( ${_cmakeVariablePrefix}_MANAGE_SPDLOG ON PARENT_SCOPE )
    if ( NOT _useSystem )
      set( ${_cmakeVariablePrefix}_USE_INTERNAL_SPDLOG ON PARENT_SCOPE )
    endif()
  endif()
  printDependencySectionEnd( "spdlog" )
endfunction(importDependencySpdlog)

#--------------------------------------
# libassert
#--------------------------------------
function(importDependencyLibassert _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "libassert" )
  if ( _useSystem )
    find_package(libassert REQUIRED)
  else()
    FetchContent_Declare(libassert GIT_REPOSITORY https://github.com/jeremy-rifkin/libassert.git
      GIT_TAG v2.1.0 GIT_SHALLOW ON )
    set(CPPTRACE_USE_EXTERNAL_ZSTD ON)
    FetchContent_MakeAvailable(libassert)
    export(TARGETS libassert-lib cpptrace-lib dwarf
      FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${_target_dependencies}_assertTargets.cmake"
      NAMESPACE libassert::
    )
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_USE_INTERNAL_LIBASSERT )
  endif()
  if ( TARGET libassert::assert )
    target_link_libraries( ${_target_dependencies} INTERFACE libassert::assert )
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_LIBASSERT )
    set( ${_cmakeVariablePrefix}_MANAGE_LIBASSERT ON PARENT_SCOPE )
    if ( NOT _useSystem )
      set( ${_cmakeVariablePrefix}_USE_INTERNAL_LIBASSERT ON PARENT_SCOPE )
    endif()
  endif()
  printDependencySectionEnd( "libassert" )
endfunction(importDependencyLibassert)

#--------------------------------------
# Eigen3
#--------------------------------------
function(importDependencyEigen3 _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "Eigen3" )
  if ( _useSystem )
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
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_USE_INTERNAL_EIGEN3 )
  endif()
  if ( TARGET Eigen3::Eigen )
    set( EIGEN3_FOUND ON )
    target_link_libraries( ${_target_dependencies} INTERFACE Eigen3::Eigen)
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_EIGEN3 )
    set( ${_cmakeVariablePrefix}_MANAGE_EIGEN3 ON PARENT_SCOPE )
    if ( NOT _useSystem )
      set( ${_cmakeVariablePrefix}_USE_INTERNAL_EIGEN3 ON PARENT_SCOPE )
    endif()
  endif()
  printDependencySectionEnd( "Eigen3" )
endfunction(importDependencyEigen3)

#--------------------------------------
# CGAL
#--------------------------------------
function(importDependencyCGAL _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "CGAL" )
  if ( _useSystem )
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
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_USE_INTERNAL_CGAL )
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
    include(CGAL_Eigen3_support)
    if ( EMSCRIPTEN )
      target_compile_definitions( CGAL INTERFACE CGAL_ALWAYS_ROUND_TO_NEAREST)
    endif()
    target_link_libraries( ${_target_dependencies} INTERFACE CGAL::CGAL CGAL::CGAL_Core CGAL::Eigen3_support)
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_CGAL )
    set( ${_cmakeVariablePrefix}_MANAGE_CGAL ON PARENT_SCOPE )
    if ( NOT _useSystem )
      set( ${_cmakeVariablePrefix}_USE_INTERNAL_CGAL ON PARENT_SCOPE )
    endif()
  else()
    message( WARNING "ThirdParty CGAL not found")
  endif()
  printDependencySectionEnd( "CGAL" )
endfunction(importDependencyCGAL)

#--------------------------------------
# BVH
#--------------------------------------
function(importDependencyBVH _useSystem _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin( "BVH" )
  if ( _useSystem )
    find_package(bvh REQUIRED)
  else()
    FetchContent_Declare(bvh GIT_REPOSITORY https://github.com/feelpp/bvh.git GIT_TAG 327b67b1d8d798e4dab1686b0dd022a1e0cbcefe)
    FetchContent_MakeAvailable(bvh)
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_USE_INTERNAL_BVH )
  endif()
  if ( TARGET bvh )
    target_link_libraries( ${_target_dependencies} INTERFACE bvh )
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_BVH )
    set( ${_cmakeVariablePrefix}_MANAGE_BVH ON PARENT_SCOPE )
    if ( NOT _useSystem )
      set( ${_cmakeVariablePrefix}_USE_INTERNAL_BVH ON PARENT_SCOPE )
    endif()
  endif()
  printDependencySectionEnd( "BVH" )
endfunction(importDependencyBVH)

# #--------------------------------------
# # CURL
# #--------------------------------------
# function(importDependencyCURL _target_dependencies _target_definitions)
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
# endfunction(importDependencyCURL)

#--------------------------------------
# PNG
#--------------------------------------
function(importDependencyPNG _target_dependencies _target_definitions _cmakeVariablePrefix)
  printDependencySectionBegin("PNG")
  find_package(PNG REQUIRED)
  if (PNG_FOUND)
    target_include_directories( ${_target_dependencies} INTERFACE ${PNG_INCLUDE_DIRS})
    target_link_libraries( ${_target_dependencies}INTERFACE ${PNG_LIBRARIES})
    target_compile_definitions( ${_target_definitions} INTERFACE FEELPP_HAS_PNG)
    set( ${_cmakeVariablePrefix}_MANAGE_PNG ON PARENT_SCOPE )
  else()
    message(WARNING "ThirdParty PNG not found")
  endif()
  printDependencySectionEnd("PNG")
endfunction(importDependencyPNG)
