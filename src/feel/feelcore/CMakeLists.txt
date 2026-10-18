# SPDX-FileCopyrightText: 2025-2026 Université de Strasbourg
# SPDX-FileContributor: Javier Cladellas
# SPDX-FileContributor: Vincent Chabannes
# SPDX-License-Identifier: LGPL-3.0-or-later

add_subdirectory(tui)

set(FEELPP_CORE_LIB_SRCS
  ${FEELPP_CORE_LIB_SRCS}
  ${CMAKE_CURRENT_SOURCE_DIR}/environment.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/table.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/tabulateinformations.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/threadpool.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/timekeeper.cpp
  ${FEELPP_CORE_LIB_SRCS_TUI}
  PARENT_SCOPE )
