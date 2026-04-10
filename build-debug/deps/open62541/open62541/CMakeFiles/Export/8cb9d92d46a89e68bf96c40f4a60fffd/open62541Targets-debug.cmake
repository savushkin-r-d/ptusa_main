#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "open62541::open62541" for configuration "Debug"
set_property(TARGET open62541::open62541 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(open62541::open62541 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libopen62541.a"
  )

list(APPEND _cmake_import_check_targets open62541::open62541 )
list(APPEND _cmake_import_check_files_for_open62541::open62541 "${_IMPORT_PREFIX}/lib/libopen62541.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
