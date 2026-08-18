cmake_minimum_required(VERSION 3.13)

include(ExternalProject)

if(WIN32)
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

if(CMAKE_CROSSCOMPILING)
    message(STATUS
        "Cross-compiling without a target ODBC package in the sysroot; "
        "skipping vendored unixODBC bootstrap. Use the target sysroot ODBC or disable ENABLE_ODBC."
    )
    return()
endif()

if(UNIX AND NOT APPLE)
    find_program(LIBTOOLIZE_EXECUTABLE libtoolize)
    find_path(LTDL_ACLOCAL_DIR ltdl.m4 PATHS /usr/share/aclocal /usr/local/share/aclocal)
    if(NOT LIBTOOLIZE_EXECUTABLE OR NOT LTDL_ACLOCAL_DIR)
        message(FATAL_ERROR
            "Linux unixODBC build requires libtool + libltdl-dev files. "
            "Install: sudo apt-get install -y libtool libltdl-dev autoconf automake make"
        )
    endif()
endif()

set(UNIXODBC_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/unixODBC")
set(UNIXODBC_BUILD_DIR "${CMAKE_BINARY_DIR}/deps/unixODBC/build")
set(UNIXODBC_INSTALL_DIR "${CMAKE_BINARY_DIR}/deps/unixODBC/install")
file(MAKE_DIRECTORY "${UNIXODBC_INSTALL_DIR}/include")
file(MAKE_DIRECTORY "${UNIXODBC_INSTALL_DIR}/lib")

if(UNIX AND NOT APPLE)
    find_program(BASH_EXECUTABLE bash REQUIRED)
    find_program(AUTORECONF_EXECUTABLE autoreconf)

    if(NOT AUTORECONF_EXECUTABLE)
        message( INFO
            "try: "
            "sudo apt-get update"
            "sudo apt-get install -y autoconf automake libtool make")
        message(FATAL_ERROR
            "Linux unixODBC build requires autotools tools: 'autoreconf' is not installed. "
            "Install 'autoconf', 'automake', 'libtool' and 'make' before configuring this project."
        )
    endif()

    if(NOT EXISTS "${UNIXODBC_SOURCE_DIR}/configure")
        if(EXISTS "${UNIXODBC_SOURCE_DIR}/autogen.sh")
            execute_process(
                COMMAND bash -lc "cd '${UNIXODBC_SOURCE_DIR}' && if [ -x ./autogen.sh ]; then bash ./autogen.sh || '${AUTORECONF_EXECUTABLE}' -fiv; else '${AUTORECONF_EXECUTABLE}' -fiv; fi"
                RESULT_VARIABLE UNIXODBC_BOOTSTRAP_RESULT
                OUTPUT_VARIABLE UNIXODBC_BOOTSTRAP_OUTPUT
                ERROR_VARIABLE UNIXODBC_BOOTSTRAP_ERROR
            )
        elseif(AUTORECONF_EXECUTABLE)
            execute_process(
                COMMAND bash -lc "cd '${UNIXODBC_SOURCE_DIR}' && '${AUTORECONF_EXECUTABLE}' -fiv"
                RESULT_VARIABLE UNIXODBC_BOOTSTRAP_RESULT
                OUTPUT_VARIABLE UNIXODBC_BOOTSTRAP_OUTPUT
                ERROR_VARIABLE UNIXODBC_BOOTSTRAP_ERROR
            )
        else()
            message(FATAL_ERROR
                "unixODBC bootstrap tooling is missing in '${UNIXODBC_SOURCE_DIR}'. "
                "Install autotools or run 'git submodule update --init --recursive'."
            )
        endif()

        if(NOT UNIXODBC_BOOTSTRAP_RESULT EQUAL 0)
            message(FATAL_ERROR
                "unixODBC bootstrap failed for Linux CI build.\n"
                "Output:\n${UNIXODBC_BOOTSTRAP_OUTPUT}\n"
                "Error:\n${UNIXODBC_BOOTSTRAP_ERROR}"
            )
        endif()
    endif()

    execute_process(
        COMMAND bash -lc "cd '${UNIXODBC_SOURCE_DIR}' && bash ./configure --help >/dev/null 2>&1"
        RESULT_VARIABLE UNIXODBC_LINUX_CHECK
    )

    if(NOT UNIXODBC_LINUX_CHECK EQUAL 0)
        message(FATAL_ERROR
            "Linux unixODBC validation failed. The configure script in '${UNIXODBC_SOURCE_DIR}' is not runnable. "
            "Ensure autotools bootstrap completed successfully and the toolchain is available."
        )
    endif()
endif()

ExternalProject_Add(unixodbc_project
    PREFIX "${CMAKE_BINARY_DIR}/deps/unixODBC"
    SOURCE_DIR "${UNIXODBC_SOURCE_DIR}"
    BINARY_DIR "${UNIXODBC_BUILD_DIR}"
    INSTALL_DIR "${UNIXODBC_INSTALL_DIR}"
    CONFIGURE_COMMAND
        bash -lc "cd '${UNIXODBC_SOURCE_DIR}' && if [ ! -f configure ]; then if [ -x ./autogen.sh ]; then bash ./autogen.sh || autoreconf -fiv; else autoreconf -fiv; fi; fi && bash ./configure --prefix='${UNIXODBC_INSTALL_DIR}' --disable-gui --enable-driver-conf=no --disable-dependency-tracking"
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
    INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
    UPDATE_COMMAND ""
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON
)

set(ODBC_INCLUDE_DIR "${UNIXODBC_INSTALL_DIR}/include")
set(ODBC_INCLUDE_DIRS "${ODBC_INCLUDE_DIR}")
set(ODBC_LIBRARY "${UNIXODBC_INSTALL_DIR}/lib/libodbc${CMAKE_SHARED_LIBRARY_SUFFIX}")
if(NOT EXISTS "${ODBC_LIBRARY}")
    set(ODBC_LIBRARY "${UNIXODBC_INSTALL_DIR}/lib/libodbc.so")
endif()

add_custom_command(
    OUTPUT "${ODBC_LIBRARY}"
    DEPENDS unixodbc_project
    COMMAND ${CMAKE_COMMAND} -E make_directory "${UNIXODBC_INSTALL_DIR}/lib"
    COMMENT "Ensuring unixODBC library exists at ${ODBC_LIBRARY}"
    VERBATIM
)
add_custom_target(unixodbc_library_target ALL DEPENDS "${ODBC_LIBRARY}")

add_library(ODBC::ODBC UNKNOWN IMPORTED GLOBAL)
set_target_properties(ODBC::ODBC PROPERTIES
    IMPORTED_LOCATION "${ODBC_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${ODBC_INCLUDE_DIR}"
)
add_dependencies(ODBC::ODBC unixodbc_library_target)
