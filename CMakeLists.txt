###  TEMPLATE.txt.tpl; coding: utf-8 ---

#  Author(s): Christophe Prud'homme <christophe.prudhomme@feelpp.org>
#       Date: 2020-06-03
#
#  Copyright (C) 2020-present Cemosis
#
# Distributed under the GPL(GNU Public License):
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
#
cmake_minimum_required(VERSION 3.21.0)

project(
  feelpp-core
  VERSION 0.1.0 )

set(EXTRA_VERSION "")
set(PROJECT_SHORTNAME "feelpp-core")

set(FEELPP_CORE_CMAKE_PREFIX "FEELPP_CORE")
set(FEELPP_CORE_EXECUTABLE_PREFIX "feelpp_core")
set(FEELPP_CORE_LIBRARY "feelpp_core")
set(FEELPP_CORE_LIBRARY_PREFIX "feelpp_core")
include(GNUInstallDirs)

foreach( _policy IN ITEMS CMP0077 CMP0167 )
  if (POLICY ${_policy})
    cmake_policy(SET ${_policy} NEW)
    set(CMAKE_POLICY_DEFAULT_${_policy} NEW)
  endif()
endforeach()

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release CACHE STRING "Select build type" FORCE )
endif()

# Choose static or shared libraries by default.
if ( EMSCRIPTEN )
  option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
else()
  option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(${FEELPP_CORE_CMAKE_PREFIX}_ENABLE_TESTS "Enable test suite" ${PROJECT_IS_TOP_LEVEL})

# Set a path to the data directory
set( ${FEELPP_CORE_CMAKE_PREFIX}_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data )
set( ${FEELPP_CORE_CMAKE_PREFIX}_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

# Forward variables scope
if( NOT PROJECT_IS_TOP_LEVEL )
  set( ${FEELPP_CORE_CMAKE_PREFIX}_DATA_DIR ${${FEELPP_CORE_CMAKE_PREFIX}_DATA_DIR} PARENT_SCOPE )
  set( ${FEELPP_CORE_CMAKE_PREFIX}_CMAKE_DIR ${${FEELPP_CORE_CMAKE_PREFIX}_CMAKE_DIR} PARENT_SCOPE )
endif()


if(NOT DEFINED FEELPP_CORE_CONFIG_INSTALL_DIR )
  set(FEELPP_CORE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_SHORTNAME}" CACHE INTERNAL "")
  set(FEELPP_CORE_DATAROOTDIR_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_SHORTNAME}" CACHE INTERNAL "")
endif()

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")



# Get dependencies
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/dependencies.cmake)


enable_testing()

if ( NOT TARGET check )
  add_custom_target(check)
endif()


add_subdirectory( src )
#add_subdirectory( docs )

# if ( ${FEELPP_CORE_CMAKE_PREFIX}_ENABLE_APPLICATIONS )
#   add_subdirectory( applications )
# endif()
if ( ${FEELPP_CORE_CMAKE_PREFIX}_ENABLE_TESTS )
  add_subdirectory( tests )
endif()



# prepare package dependencies
feelpp_configure_dependencies_package_config( ${FEELPP_CORE_CMAKE_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/cmake )

# generate and install package config
include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/${PROJECT_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${FEELPP_CORE_CONFIG_INSTALL_DIR}
  PATH_VARS FEELPP_CORE_CONFIG_INSTALL_DIR FEELPP_CORE_DATAROOTDIR_INSTALL_DIR
  )
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
  )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${FEELPP_CORE_CONFIG_INSTALL_DIR}
  )

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/macro.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/importDependenciesHelpers.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/importDependenciesPackageConfig.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/importDependenciesPackageConfig.cmake.in
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/catch2.patch
  DESTINATION ${FEELPP_CORE_DATAROOTDIR_INSTALL_DIR}/cmake
)




# extra ignored files
list(APPEND CPACK_SOURCE_IGNORE_FILES
  .git
  .gitignore
  .travis.yml
  .appveyor.yml
  )

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}${EXTRA_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}${EXTRA_VERSION}")
SET(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}${EXTRA_VERSION}-${CMAKE_SYSTEM_NAME}")
set(CPACK_PACKAGE_DIRECTORY "${PROJECT_BINARY_DIR}/assets")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cemosis feelpp-core template application")
set(CPACK_PACKAGE_DESCRIPTION "Cemosis provides a Feel++ template application.")
set(CPACK_PACKAGE_VENDOR "Cemosis")
set(CPACK_PACKAGE_CONTACT "Christophe Prud'homme <christophe.prudhomme@cemosis.fr>")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")


set(CPACK_GENERATOR "TGZ;DEB")
set(CPACK_SOURCE_GENERATOR "TGZ")

set(CPACK_DEBIAN_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}${EXTRA_VERSION}")
set(CPACK_DEBIAN_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
# set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/cmake/debian/postinst")

include(CPack)

