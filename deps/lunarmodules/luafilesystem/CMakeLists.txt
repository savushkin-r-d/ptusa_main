set(SOURCES luafilesystem/src/lfs.c)

# create the executable
add_library(lfs ${SOURCES})
target_include_directories(lfs PRIVATE luafilesystem/src src/lib ${LUA_INCLUDE_DIR})
target_link_libraries(lfs PRIVATE liblua)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
