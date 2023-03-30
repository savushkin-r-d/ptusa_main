file (STRINGS "@CMAKE_BINARY_DIR@/CTestTestfile.cmake" LINES)

# overwrite the file....
file(WRITE "@CMAKE_BINARY_DIR@/CTestTestfile.cmake" "")

# loop through the lines,
foreach(LINE IN LISTS LINES)
  if(NOT "${LINE}" MATCHES [[subdirs\(\"lua\"\)|subdirs\(\"zlib\"\)|subdirs\(\"toluapp\"\)|subdirs\(\"benchmark\"\)]] )
    # write line without unwanted parts ...
    file(APPEND "@CMAKE_BINARY_DIR@/CTestTestfile.cmake" "${LINE}\n")
  endif()
endforeach()
