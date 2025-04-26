function(feelpp_get_compile_definition varTarget varValue )
  get_property(CD TARGET ${varTarget} PROPERTY INTERFACE_COMPILE_DEFINITIONS)
  list(FIND CD "${varValue}" res)
  if ( NOT res EQUAL -1 )
    set(${varValue} ${res} PARENT_SCOPE )
  endif()
endfunction()

function(ATTIC_feelpp_set_options varTarget prefix key )
  get_property(CD TARGET ${varTarget} PROPERTY INTERFACE_COMPILE_DEFINITIONS)
  foreach( opts IN LISTS CD )
    string( REGEX MATCH "${prefix}_${key}_[a-zA-Z0-9_]+$" OPT ${opts} )
    if ( OPT )
      #message("OPT = ${OPT}")
      set(${OPT} 1 PARENT_SCOPE)
    endif()
  endforeach()
endfunction()




# -------------------------------------------------------------------
# Get all compile definition recursively from (interface) link libraries
function(feelpp_get_recursive_compile_definitions target out_defs visited)
  if(${target} IN_LIST visited)
    set(${out_defs} "" PARENT_SCOPE)
    return()
  endif()

  list(APPEND visited ${target})

  set(defs "")

  # Get directly set compile definitions
  get_target_property(cd ${target} COMPILE_DEFINITIONS)
  if(cd)
    list(APPEND defs ${cd})
  endif()

  get_target_property(icd ${target} INTERFACE_COMPILE_DEFINITIONS)
  if(icd)
    list(APPEND defs ${icd})
  endif()

  # Get dependencies
  get_target_property(deps ${target} INTERFACE_LINK_LIBRARIES)
  if(NOT deps)
    get_target_property(deps ${target} LINK_LIBRARIES)
  endif()

  foreach(dep IN LISTS deps)
    if(TARGET ${dep})
      feelpp_get_recursive_compile_definitions(${dep} sub_defs "${visited}")
      list(APPEND defs ${sub_defs})
    endif()
  endforeach()

  list(REMOVE_DUPLICATES defs)
  set(${out_defs} "${defs}" PARENT_SCOPE)
endfunction()


# -------------------------------------------------------------------
# set variables FEELPP_HAS_<...> from compile definitions
function(feelpp_set_options varTarget prefix key )
  feelpp_get_recursive_compile_definitions( ${varTarget} all_defs "")
  foreach( opts IN LISTS all_defs )
    string( REGEX MATCH "${prefix}_${key}_[a-zA-Z0-9_]+$" OPT ${opts} )
    if ( OPT )
      #message("OPT = ${OPT}")
      set(${OPT} 1 PARENT_SCOPE)
    endif()
  endforeach()
endfunction()
