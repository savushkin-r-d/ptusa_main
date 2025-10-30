
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was open62541Config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################
include("${CMAKE_CURRENT_LIST_DIR}/open62541Targets.cmake")

set_and_check(open62541_TOOLS_DIR ${PACKAGE_PREFIX_DIR}/share/open62541 CACHE PATH "Path to the directory that contains the tooling of the stack")
set_and_check(UA_SCHEMA_DIR ${PACKAGE_PREFIX_DIR}/share/open62541/schema CACHE PATH "Path to the directory that contains the schema files")

# Macros for datatype generation, nodeset compiler, etc.
include("${CMAKE_CURRENT_LIST_DIR}/open62541Macros.cmake")

check_required_components(open62541)
