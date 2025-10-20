set(SOURCES luafilesystem/src/lfs.c)

# create the library
add_library(lfs ${SOURCES})
target_include_directories(lfs PRIVATE luafilesystem/src src/lib ${LUA_INCLUDE_DIR})
target_link_libraries(lfs PRIVATE liblua)

target_compile_definitions(lfs PRIVATE _CRT_SECURE_NO_WARNINGS)

set_target_properties (lfs PROPERTIES OUTPUT_NAME lfs PREFIX "")
