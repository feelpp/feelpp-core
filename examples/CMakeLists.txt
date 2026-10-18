# SPDX-FileCopyrightText: 2026 Université de Strasbourg
# SPDX-FileContributor: Javier Cladellas
# SPDX-License-Identifier: LGPL-3.0-or-later


file(GLOB files feelcore*.cpp)

foreach(file ${files})
  get_filename_component(file_basename ${file} NAME_WE)
  set(example feelpp_core_${file_basename})
  add_executable(${example}  ${file})
  target_link_libraries(${example} PRIVATE ${FEELPP_CORE_LIBRARY} )
endforeach()
