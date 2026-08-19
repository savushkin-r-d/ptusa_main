cmake_minimum_required(VERSION 3.13)

project(unixodbc LANGUAGES C)

# Native Windows builds use the system ODBC libraries.
# AXCF cross-builds are still hosted on Windows, but CMAKE_CROSSCOMPILING is true,
# so they must not take the native Windows branch.
if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
    find_package(ODBC REQUIRED)
    if(NOT TARGET ODBC::ODBC)
        add_library(ODBC::ODBC UNKNOWN IMPORTED GLOBAL)
        set_target_properties(ODBC::ODBC PROPERTIES
            IMPORTED_LOCATION "${ODBC_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${ODBC_INCLUDE_DIR}"
        )
    endif()
    return()
endif()

if(TARGET ODBC::ODBC)
    return()
endif()

find_package(ODBC QUIET)
if(ODBC_FOUND)
    if(NOT TARGET ODBC::ODBC)
        add_library(ODBC::ODBC UNKNOWN IMPORTED GLOBAL)
        set_target_properties(ODBC::ODBC PROPERTIES
            IMPORTED_LOCATION "${ODBC_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${ODBC_INCLUDE_DIR}"
        )
    endif()
    message(STATUS "Using system ODBC library: ${ODBC_LIBRARY}")
    return()
endif()

include(ExternalProject)

set(UNIXODBC_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/unixODBC")
set(UNIXODBC_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/unixodbc-build")
set(UNIXODBC_INSTALL_DIR "${CMAKE_BINARY_DIR}/unixodbc-install")

find_program(BASH_EXECUTABLE NAMES bash sh REQUIRED)

if(CMAKE_HOST_WIN32)
    function(normalize_windows_bash_path out_var in_path)
        set(_path "${in_path}")
        string(REPLACE "\\" "/" _path "${_path}")

        if(_path MATCHES "^([A-Za-z]):/(.*)$")
            string(TOLOWER "${CMAKE_MATCH_1}" _unixodbc_drive_letter_lower)
            set(_path "/mnt/${_unixodbc_drive_letter_lower}/${CMAKE_MATCH_2}")
        elseif(_path MATCHES "^/([A-Za-z])/(.*)$")
            string(TOLOWER "${CMAKE_MATCH_1}" _unixodbc_drive_letter_lower)
            set(_path "/mnt/${_unixodbc_drive_letter_lower}/${CMAKE_MATCH_2}")
        endif()

        set(${out_var} "${_path}" PARENT_SCOPE)
    endfunction()

    normalize_windows_bash_path(UNIXODBC_SOURCE_DIR_BASH "${UNIXODBC_SOURCE_DIR}")
    normalize_windows_bash_path(UNIXODBC_BUILD_DIR_BASH "${UNIXODBC_BUILD_DIR}")
    normalize_windows_bash_path(UNIXODBC_INSTALL_DIR_BASH "${UNIXODBC_INSTALL_DIR}")

    find_program(WSLPATH_EXECUTABLE NAMES wslpath)
    find_program(CYGPATH_EXECUTABLE NAMES cygpath)

    if(WSLPATH_EXECUTABLE)
        execute_process(
            COMMAND "${WSLPATH_EXECUTABLE}" -u "${UNIXODBC_SOURCE_DIR}"
            OUTPUT_VARIABLE UNIXODBC_SOURCE_DIR_BASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        execute_process(
            COMMAND "${WSLPATH_EXECUTABLE}" -u "${UNIXODBC_BUILD_DIR}"
            OUTPUT_VARIABLE UNIXODBC_BUILD_DIR_BASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        execute_process(
            COMMAND "${WSLPATH_EXECUTABLE}" -u "${UNIXODBC_INSTALL_DIR}"
            OUTPUT_VARIABLE UNIXODBC_INSTALL_DIR_BASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    elseif(CYGPATH_EXECUTABLE)
        execute_process(
            COMMAND "${CYGPATH_EXECUTABLE}" -u "${UNIXODBC_SOURCE_DIR}"
            OUTPUT_VARIABLE UNIXODBC_SOURCE_DIR_BASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        execute_process(
            COMMAND "${CYGPATH_EXECUTABLE}" -u "${UNIXODBC_BUILD_DIR}"
            OUTPUT_VARIABLE UNIXODBC_BUILD_DIR_BASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        execute_process(
            COMMAND "${CYGPATH_EXECUTABLE}" -u "${UNIXODBC_INSTALL_DIR}"
            OUTPUT_VARIABLE UNIXODBC_INSTALL_DIR_BASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()
else()
    set(UNIXODBC_SOURCE_DIR_BASH "${UNIXODBC_SOURCE_DIR}")
    set(UNIXODBC_BUILD_DIR_BASH "${UNIXODBC_BUILD_DIR}")
    set(UNIXODBC_INSTALL_DIR_BASH "${UNIXODBC_INSTALL_DIR}")
endif()

set(UNIXODBC_CONFIGURE_ARGS
    --prefix=${UNIXODBC_INSTALL_DIR_BASH}
    --enable-shared=yes
    --enable-static=no
    --disable-gui
    --disable-debug
    --enable-driver-config=no
    --enable-drivers=no
    --without-odbcini
    --without-odbcinstini
)
string(REPLACE ";" " " UNIXODBC_CONFIGURE_ARG_STRING "${UNIXODBC_CONFIGURE_ARGS}")

if(CMAKE_HOST_WIN32)
    set(UNIXODBC_SETUP_COMMAND
        "mkdir -p \"${UNIXODBC_BUILD_DIR_BASH}\" && cp -af \"${UNIXODBC_SOURCE_DIR_BASH}/.\" \"${UNIXODBC_BUILD_DIR_BASH}/\""
    )
else()
    set(UNIXODBC_SETUP_COMMAND
        "rm -rf \"${UNIXODBC_BUILD_DIR_BASH}\" && mkdir -p \"${UNIXODBC_BUILD_DIR_BASH}\" && cp -af \"${UNIXODBC_SOURCE_DIR_BASH}/.\" \"${UNIXODBC_BUILD_DIR_BASH}/\""
    )
endif()

set(UNIXODBC_CONFIGURE_COMMAND
    "cd \"${UNIXODBC_BUILD_DIR_BASH}\" && autoreconf -fi -I m4 && ./configure ${UNIXODBC_CONFIGURE_ARG_STRING}"
)

ExternalProject_Add(unixodbc_project
    SOURCE_DIR "${UNIXODBC_SOURCE_DIR}"
    BINARY_DIR "${UNIXODBC_BUILD_DIR}"
    INSTALL_DIR "${UNIXODBC_INSTALL_DIR}"
    PATCH_COMMAND
        "${BASH_EXECUTABLE}" -lc "sed -i '/ACLOCAL_AMFLAGS/d' \"${UNIXODBC_SOURCE_DIR_BASH}/Makefile.am\" && sed -i '/ACLOCAL_AMFLAGS/d' \"${UNIXODBC_SOURCE_DIR_BASH}/Makefile.in\""
    CONFIGURE_COMMAND "${BASH_EXECUTABLE}" -lc "${UNIXODBC_SETUP_COMMAND} && ${UNIXODBC_CONFIGURE_COMMAND}"
    BUILD_COMMAND "${BASH_EXECUTABLE}" -lc "cd \"${UNIXODBC_BUILD_DIR_BASH}\" && make -j${CMAKE_BUILD_PARALLEL_LEVEL}"
    INSTALL_COMMAND "${BASH_EXECUTABLE}" -lc "cd \"${UNIXODBC_BUILD_DIR_BASH}\" && make install"
    BUILD_BYPRODUCTS "${UNIXODBC_INSTALL_DIR}/lib/libodbc${CMAKE_SHARED_LIBRARY_SUFFIX}"
    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_INSTALL TRUE
)

set(_unixodbc_include_dir "${UNIXODBC_SOURCE_DIR}/include")
set(_unixodbc_lib_path "${UNIXODBC_INSTALL_DIR}/lib/libodbc${CMAKE_SHARED_LIBRARY_SUFFIX}")
if(WIN32)
    set(_unixodbc_lib_path "${UNIXODBC_INSTALL_DIR}/lib/libodbc${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

add_library(unixodbc_library_target SHARED IMPORTED GLOBAL)
set_target_properties(unixodbc_library_target PROPERTIES
    IMPORTED_LOCATION "${_unixodbc_lib_path}"
    INTERFACE_INCLUDE_DIRECTORIES "${_unixodbc_include_dir}"
)
add_dependencies(unixodbc_library_target unixodbc_project)

add_library(ODBC::ODBC ALIAS unixodbc_library_target)
