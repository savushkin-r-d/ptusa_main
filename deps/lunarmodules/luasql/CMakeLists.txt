cmake_minimum_required(VERSION 3.13)

# Драйверы LuaSQL для сборки (совпадают с именами файлов ls_<driver>.c и <driver>.def)
set(LUASQL_DRIVERS "odbc" CACHE STRING "LuaSQL drivers to build (e.g. 'odbc;sqlite3;mysql;postgres;firebird')")

# Общие настройки путей.
set(LUASQL_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/luasql/src")

add_compile_definitions(LUASQL_VERSION_NUMBER=\"2.7.1\")

# Вспомогательная функция для добавления драйвера.
function(add_luasql_driver driver)
    set(src_c "${LUASQL_SRC_DIR}/ls_${driver}.c")
    if(NOT EXISTS "${src_c}")
        message(WARNING "LuaSQL driver '${driver}' skipped: source not found: ${src_c}")
        return()
    endif()

    set(SOURCES
        "${LUASQL_SRC_DIR}/luasql.c"
        "${src_c}"
    )

    # Имя цели: luasql_<driver>, выходной файл: <driver>.dll/.so без префикса.
    add_library(luasql_${driver} ${SOURCES})

    if(MSVC)
        set_target_properties(luasql_${driver} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()

    target_include_directories(luasql_${driver} PRIVATE
        "${LUASQL_SRC_DIR}"
        ${LUA_INCLUDE_DIR}
    )
    target_link_libraries(luasql_${driver} PRIVATE liblua)

    # Специфичные зависимости по драйверам.
    if(driver STREQUAL "odbc")
        if(WIN32)
            target_link_libraries(luasql_${driver} PRIVATE odbc32)
        else()
            target_link_libraries(luasql_${driver} PRIVATE odbc)
        endif()
    elseif(driver STREQUAL "sqlite3")
        find_package(SQLite3 QUIET)
        if(TARGET SQLite::SQLite3)
            target_link_libraries(luasql_${driver} PRIVATE SQLite::SQLite3)
        else()
            find_library(SQLITE3_LIBRARY NAMES sqlite3)
            if(SQLITE3_LIBRARY)
                target_link_libraries(luasql_${driver} PRIVATE "${SQLITE3_LIBRARY}")
            else()
                message(WARNING "SQLite3 not found; driver '${driver}' may fail to link")
            endif()
        endif()
    elseif(driver STREQUAL "mysql")
        # Пытаемся найти клиентскую библиотеку MySQL/MariaDB.
        find_library(MYSQL_LIB NAMES libmysql mysqlclient mariadb)
        if(MYSQL_LIB)
            target_link_libraries(luasql_${driver} PRIVATE "${MYSQL_LIB}")
        else()
            message(WARNING "MySQL client library not found; driver '${driver}' may fail to link")
        endif()
    elseif(driver STREQUAL "postgres")
        find_package(PostgreSQL QUIET)
        if(TARGET PostgreSQL::PostgreSQL)
            target_link_libraries(luasql_${driver} PRIVATE PostgreSQL::PostgreSQL)
        else()
            message(WARNING "PostgreSQL not found; driver '${driver}' may fail to link")
        endif()
    elseif(driver STREQUAL "firebird")
        find_library(FIREBIRD_LIB NAMES fbclient firebird)
        if(FIREBIRD_LIB)
            target_link_libraries(luasql_${driver} PRIVATE "${FIREBIRD_LIB}")
        else()
            message(WARNING "Firebird client library not found; driver '${driver}' may fail to link")
        endif()
    endif()

    # MSVC-опции и дефайны, аналогичные Makefile.win.
    target_compile_definitions(luasql_${driver} PRIVATE
        $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS;WIN32>
    )

    # Имена и каталоги вывода: luasql/<driver>.dll
    set_target_properties(luasql_${driver} PROPERTIES
        OUTPUT_NAME "${driver}"
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/luasql"
        LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/luasql"
        ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/luasql"
    )
endfunction()

# Добавляем заявленные драйверы.
foreach(d ${LUASQL_DRIVERS})
    string(TOLOWER "${d}" d_lower)
    add_luasql_driver("${d_lower}")
endforeach()
