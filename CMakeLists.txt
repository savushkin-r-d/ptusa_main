cmake_minimum_required(VERSION 3.13)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)

project(ptusa_main VERSION 2025.07.16.0)

option(USE_OPCUA "Use OPC UA" ON)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
################# create targets ######################################################

if(LINUX)
    add_compile_definitions(LINUX_OS)
elseif(WIN32)
    add_compile_definitions(WIN_OS)
    add_compile_definitions(SIMPLE_LOG)
endif()

add_compile_definitions(TEST_SPEED MODBUS)

if(MSVC)
    add_compile_options("/utf-8")

    # Исключить конфликтующую библиотеку
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")
endif(MSVC)

configure_file( version_info.in version_info.h @ONLY)

set(BUILD_SHARED_LIBS OFF)
add_subdirectory(deps/fmt EXCLUDE_FROM_ALL)
set(BUILD_SHARED_LIBS ON)

add_subdirectory(deps/lua EXCLUDE_FROM_ALL)

set(LUA_LIBRARIES liblua)
set(LUA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/lua/lua/src;${CMAKE_BINARY_DIR}/deps/lua/lua)

add_subdirectory(deps/toluapp EXCLUDE_FROM_ALL)

add_subdirectory(deps/zlib EXCLUDE_FROM_ALL)

set(INSTALL_GTEST OFF)
add_subdirectory(deps/googletest EXCLUDE_FROM_ALL)

add_subdirectory(deps/subhook EXCLUDE_FROM_ALL)

if(NOT MINGW)
    set (BENCHMARK_ENABLE_TESTING OFF)
    add_subdirectory(deps/benchmark EXCLUDE_FROM_ALL)
endif()

if(USE_OPCUA)
    add_subdirectory(deps/open62541 EXCLUDE_FROM_ALL)
    add_compile_definitions(OPCUA)
endif()

add_subdirectory(deps/cxxopts EXCLUDE_FROM_ALL)

file(GLOB common_src CONFIGURE_DEPENDS
        common/PAC-driver/*.cpp
        common/util/*.c
        common/util/*.cpp
        common/*.cpp

        PAC/common/*.cpp
        PAC/common/sys/*.cpp
        PAC/common/device/*.cpp
        PAC/IoT/*.cpp)

if(USE_OPCUA)
    list(FILTER common_src EXCLUDE REGEX ".*rfid*|snprintf\\.c")
else()
    list(FILTER common_src EXCLUDE REGEX ".*OPCUA*|.*rfid*|snprintf\\.c")
endif()

file(GLOB test_src CONFIGURE_DEPENDS
            test/*.cpp
            test/device/*.cpp
            test/dll/*.cpp)

if(NOT USE_OPCUA)
    list(FILTER test_src EXCLUDE REGEX ".*OPCUA*")
endif()

if(LINUX)
    file(GLOB linux_src CONFIGURE_DEPENDS
            PAC/linux/*.cpp
            PAC/linux/PAC_PC/*.cpp)
endif()

if(WIN32)
    file(GLOB win_src CONFIGURE_DEPENDS
            common/win_util/*.cpp

            PAC/win/*.cpp
            PAC/win/PAC_PC/*.cpp)
endif()

if(LINUX)
    add_executable(${PROJECT_NAME} ${common_src} ${linux_src}
            PAC_control_projects/main_control_prg/main_cycle.cpp
            PAC_control_projects/main_control_prg/main.cpp)

    add_library(libptusa_main SHARED ${common_src} ${linux_src}
            PAC_control_projects/main_control_prg/main_control_dll_vc/main_control_dll_vc.cpp)

    add_executable(main_test ${common_src} ${linux_src} ${test_src}
           PAC_control_projects/main_control_prg/main_cycle.cpp
           PAC_control_projects/main_control_prg/main_control_dll_vc/main_control_dll_vc.cpp
           test/main_test.cpp)

    add_executable(main_perfomance_test ${common_src} ${linux_src}
           test/perfomance/main_perfomance_test.cpp
           PAC_control_projects/main_control_prg/version.rc)

    if(ARP_DEVICE)
        execute_process(COMMAND plcncli
            generate code -s . -o ${CMAKE_BINARY_DIR}/PtusaPLCnextEngineer/intermediate/code --verbose
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext/PtusaPLCnextEngineer
            COMMAND_ECHO STDOUT )
        execute_process(COMMAND plcncli
            generate config -s . -o ${CMAKE_BINARY_DIR}/PtusaPLCnextEngineer/intermediate/config --verbose
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext/PtusaPLCnextEngineer
            COMMAND_ECHO STDOUT )

        file(GLOB PLCnext_engineer_src CONFIGURE_DEPENDS
            PAC/linux/PAC_PLCnext/PtusaPLCnextEngineer/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext/PtusaPLCnextEngineer/*.cpp
            ${CMAKE_BINARY_DIR}/PtusaPLCnextEngineer/intermediate/code/*.cpp )

        add_library(PtusaPLCnextEngineer SHARED
            ${common_src} ${linux_src} ${PLCnext_engineer_src}
            PAC_control_projects/main_control_prg/main_cycle.cpp )
    endif()

elseif(WIN32)
    add_executable(${PROJECT_NAME} ${common_src} ${win_src}
            PAC_control_projects/main_control_prg/main_cycle.cpp
            PAC_control_projects/main_control_prg/main.cpp
            PAC_control_projects/main_control_prg/version.rc)

    add_library(libptusa_main SHARED ${common_src} ${win_src}
            PAC_control_projects/main_control_prg/main_control_dll_vc/main_control_dll_vc.cpp
            PAC_control_projects/main_control_prg/main_control_dll_vc/dllmain.cpp
            PAC_control_projects/main_control_prg/version.rc)

    add_executable(main_test ${common_src} ${win_src} ${test_src}
           test/main_test.cpp
           PAC_control_projects/main_control_prg/main_cycle.cpp
           PAC_control_projects/main_control_prg/main_control_dll_vc/main_control_dll_vc.cpp
           PAC_control_projects/main_control_prg/version.rc)

if(NOT MINGW)
    add_executable(main_perfomance_test ${common_src} ${win_src}
           test/perfomance/main_perfomance_test.cpp
           PAC_control_projects/main_control_prg/version.rc)
endif()

    set_target_properties (toluapp toluapp_lib liblua PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

    add_custom_command( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/PAC_dev_lua.cpp
        COMMAND toluapp -n PAC_dev -o PAC/common/PAC_dev_lua.cpp PAC/common/toloa++/pkg/PAC_dev.hh
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/toloa++/pkg/PAC_dev.hh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
    add_custom_command( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/PAC/IoT/IOT_dev_lua.cpp
        COMMAND toluapp -n IOT_dev -o PAC/IoT/IOT_dev_lua.cpp PAC/common/toloa++/pkg/IOT_dev.hh
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/toloa++/pkg/IOT_dev.hh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
    if (USE_OPCUA)
        add_custom_command( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/OPCUA_tolua.cpp
            COMMAND toluapp -n OPC_UA -o PAC/common/OPCUA_tolua.cpp PAC/common/toloa++/pkg/OPC_UA.hh
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/toloa++/pkg/OPC_UA.hh
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
    endif()
endif()

target_compile_definitions(main_test PUBLIC DEBUG_NO_IO_MODULES)
target_compile_definitions(libptusa_main PUBLIC DEBUG_NO_IO_MODULES)
if(NOT MINGW)
    target_compile_definitions(main_perfomance_test PUBLIC DEBUG_NO_IO_MODULES)
endif()

if(ARP_DEVICE)
    target_compile_definitions(PtusaPLCnextEngineer PUBLIC PAC_PLCNEXT)
    target_compile_definitions(ptusa_main PUBLIC PAC_PC)
    target_compile_definitions(libptusa_main PUBLIC PAC_PC)
    target_compile_definitions(main_perfomance_test PUBLIC PAC_PC)
else()
    add_compile_definitions(PAC_PC)
endif()

if(ARP_DEVICE OR MINGW)
    list(INSERT CMAKE_MODULE_PATH 0 "${ARP_TOOLCHAIN_CMAKE_MODULE_PATH}")

    set_target_properties(main_test PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()

option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  # Add required flags (GCC & LLVM/Clang)
  target_compile_options(main_test PUBLIC
    -O0        # no optimization
    -g         # generate debug info
    --coverage # sets all required flags
    )

  target_link_libraries(main_test gcov)
  target_link_options(main_test PUBLIC --coverage)
endif()

set_target_properties (libptusa_main PROPERTIES OUTPUT_NAME ptusa_main PREFIX "" CLEAN_DIRECT_OUTPUT 1)
target_compile_definitions(libptusa_main PUBLIC _USRDLL)

target_compile_definitions(gmock PUBLIC GTEST_CREATE_SHARED_LIBRARY)
target_compile_definitions(main_test PUBLIC GTEST_LINKED_AS_SHARED_LIBRARY PTUSA_TEST)

set_target_properties (subhook gtest gtest_main gmock gmock_main
                        PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
if(NOT MINGW)
    set_target_properties (benchmark PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
endif()
#######################################################################################

################# project include-paths ###############################################

set(GENERAL_INCLUDES
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/util>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/PAC-driver>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/sys>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC_control_projects/main_control_prg>

        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>

if (WIN32)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/win_util>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/win>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/win/PAC_PC>
elseif (LINUX)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PC>

if(ARP_DEVICE)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext/PtusaPLCnextEngineer>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/PtusaPLCnextEngineer/intermediate/code>
endif()

endif ()
        if (USE_OPCUA)
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/open62541/open62541/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/open62541/open62541/plugins/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/open62541/open62541/arch>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/open62541/open62541/deps>
            $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/deps/open62541/open62541/src_generated>
        endif ()

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/IoT>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/toluapp/toluapp/include>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/lua/lua/src>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/deps/lua/lua>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/zlib/zlib>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/deps/zlib/zlib>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/fmt/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/cxxopts/cxxopts/include>)

target_include_directories(${PROJECT_NAME} PUBLIC ${GENERAL_INCLUDES})
target_include_directories(libptusa_main PUBLIC ${GENERAL_INCLUDES})
if(ARP_DEVICE)
    target_include_directories(PtusaPLCnextEngineer PUBLIC ${GENERAL_INCLUDES})
endif()

set(TEST_INCLUDES
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/deps/googletest/googletest/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/deps/googletest/googletest>

        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/deps/googletest/googlemock/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/deps/googlemock/googlemock>)

target_include_directories(main_test PUBLIC ${GENERAL_INCLUDES} ${TEST_INCLUDES})
if(NOT MINGW)
    target_include_directories(main_perfomance_test PUBLIC ${GENERAL_INCLUDES})
endif()

#######################################################################################

################# set link options ####################################################
# WARNING: Without --no-undefined the linker will not check, whether all necessary    #
#          libraries are linked. When a library which is necessary is not linked,     #
#          the firmware will crash and there will be NO indication why it crashed.    #
#######################################################################################
if(LINUX)
    target_link_options(${PROJECT_NAME} PRIVATE LINKER:--no-undefined)
endif()
#######################################################################################

if(ARP_DEVICE)
    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH $ORIGIN)
endif()

################# add link targets ####################################################
if(ARP_DEVICE)
    find_package(ArpDevice REQUIRED)
    find_package(ArpProgramming REQUIRED)
endif()

################# add link targets ####################################################

target_link_libraries(${PROJECT_NAME} PRIVATE liblua_static toluapp_lib_static zlibstatic fmt::fmt)
target_link_libraries(libptusa_main toluapp_lib_static zlibstatic fmt::fmt)
if(ARP_DEVICE)
    target_link_libraries(PtusaPLCnextEngineer ArpDevice ArpProgramming liblua_static toluapp_lib_static zlibstatic fmt::fmt)
    if (USE_OPCUA)
        target_link_libraries(PtusaPLCnextEngineer open62541::open62541)
    endif()
endif()

if(USE_OPCUA)
    target_link_libraries(${PROJECT_NAME} PRIVATE open62541::open62541)
    target_link_libraries(libptusa_main open62541::open62541)
    target_link_libraries(main_test open62541::open62541)
    if(NOT MINGW)
        target_link_libraries(main_perfomance_test open62541::open62541)
    endif()
endif()

target_link_libraries(main_test liblua_static toluapp_lib_static zlibstatic gtest gtest_main gmock gmock_main subhook fmt::fmt)

if(NOT MINGW)
    target_link_libraries(main_perfomance_test liblua_static toluapp_lib_static zlibstatic fmt::fmt benchmark::benchmark)
endif()

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE wsock32 ws2_32)
    target_link_libraries(libptusa_main wsock32 ws2_32)
    target_link_libraries(main_test wsock32 ws2_32)
    if(NOT MINGW)
        target_link_libraries(main_perfomance_test wsock32 ws2_32)
    endif()


    if(DEFINED ENV{LUA_DEV})
        target_link_libraries(libptusa_main $ENV{LUA_DEV}/lib/lua51.lib)
    else()
        target_link_libraries(libptusa_main liblua)
    endif()

elseif (LINUX)
    target_link_libraries(libptusa_main liblua_static)
endif()
#######################################################################################


if(ARP_DEVICE)
    string(REGEX REPLACE "^.*\\(([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+).*$" "\\1" _ARP_SHORT_DEVICE_VERSION ${ARP_DEVICE_VERSION})
    install(TARGETS ${PROJECT_NAME} libptusa_main PtusaPLCnextEngineer
        LIBRARY DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/lib
        ARCHIVE DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/lib
        RUNTIME DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/bin)

    install(CODE "execute_process(COMMAND plcncli
                deploy -s \"${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext/PtusaPLCnextEngineer\"
                -t \"${ARP_DEVICE},${ARP_DEVICE_VERSION}\"
                -f \"${CMAKE_BINARY_DIR}/libPtusaPLCnextEngineer.so|.\"
                --verbose
            WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}/PtusaPLCnextEngineer\"
            COMMAND_ECHO STDOUT )")
     install(FILES ${CMAKE_BINARY_DIR}/PtusaPLCnextEngineer/bin/PtusaPLCnextEngineer.pcwlx
        DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>)

    unset(_ARP_SHORT_DEVICE_VERSION)
else()
    if (LINUX)
        install(TARGETS ${PROJECT_NAME} libptusa_main
            LIBRARY DESTINATION $<CONFIG>
            RUNTIME DESTINATION $<CONFIG>)
    else()
        install(TARGETS ${PROJECT_NAME} libptusa_main RUNTIME DESTINATION $<CONFIG>)
    endif()
endif()

enable_testing()
add_test(run_no_command_line_arguments ${PROJECT_NAME})
if (WIN32)
    set_tests_properties( run_no_command_line_arguments
        PROPERTIES PASS_REGULAR_EXPRESSION
[[
(.*)
  -s, --script arg          The script file to execute \(default: main\.plua\)
  -d, --debug               Enable debugging
      --no_io_nodes         No communicate with I\\O nodes \(default: true\)
      --read_only_io_nodes  Read only from I\\O nodes \(default: true\)
  -p, --port arg            Param port \(default: 10000\)
  -h, --help                Print help info
  -r, --rcrc                Reset params
      --opc-r               Start OPC UA server with program start \(only 
                            read\)
      --opc-rw              Start OPC UA server with program start 
                            \(read-write\)
      --sys_path arg        Sys path \(default: \./sys\)
      --path arg            Path \(default: \.\)
      --extra_paths arg     Extra paths \(default: \./dairy-sys\)
      --sleep_time_ms arg   Sleep time, ms \(default: 2\)
]] )
else()
    set_tests_properties( run_no_command_line_arguments
        PROPERTIES PASS_REGULAR_EXPRESSION
[[
(.*)
  -s, --script arg          The script file to execute \(default: main\.plua\)
  -d, --debug               Enable debugging
      --no_io_nodes         No communicate with I\\O nodes
      --read_only_io_nodes  Read only from I\\O nodes
  -p, --port arg            Param port \(default: 10000\)
  -h, --help                Print help info
  -r, --rcrc                Reset params
      --opc-r               Start OPC UA server with program start \(only 
                            read\)
      --opc-rw              Start OPC UA server with program start 
                            \(read-write\)
      --sys_path arg        Sys path \(default: \./sys\)
      --path arg            Path \(default: \.\)
      --extra_paths arg     Extra paths \(default: \./dairy-sys\)
      --sleep_time_ms arg   Sleep time, ms \(default: 2\)
]] )
endif()

  set_property(
    TARGET libptusa_main
    PROPERTY VS_DEBUGGER_COMMAND
    "lua.exe")

if(NOT ARP_DEVICE AND NOT MINGW AND NOT SKIP_DEMO)
    add_test(NAME T1-PLCnext-Demo_main COMMAND ${PROJECT_NAME} main.plua1
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo )
    set_tests_properties(T1-PLCnext-Demo_main PROPERTIES
        PASS_REGULAR_EXPRESSION "main\.plua1: No such file or directory")

    add_test(T1-PLCnext-Demo lua5.1
                -e "package.cpath = package.cpath..';${CMAKE_BINARY_DIR}/?.dll;${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/?.dll'"
                -e "main_script='${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo/main.plua'"
                -e "sys_path='${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo/sys/'"
                -e "path='${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo/'"
                ${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo/spec/ptusa_main.lua)

    add_test(NAME test_opc_read_mode
            COMMAND ${PROJECT_NAME} --opc-r)
    set_tests_properties(test_opc_read_mode PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo"
            PASS_REGULAR_EXPRESSION "OPC UA server is activated \\(only read\\)")

    add_test(NAME test_opc_read_write_mode
            COMMAND ${PROJECT_NAME} --opc-rw)
    set_tests_properties(test_opc_read_write_mode PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo"
            PASS_REGULAR_EXPRESSION "OPC UA server is activated \\(read-write\\)")

    if(LINUX)
        add_test(T1-PLCnext-Demo_valgrind valgrind
                --error-exitcode=1
                --num-callers=100
                --leak-check=full
                --verbose
                --track-origins=yes
                --show-leak-kinds=all
                lua5.1
                -e "package.cpath = package.cpath..';${CMAKE_BINARY_DIR}/?.dll;${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/?.dll'"
                -e "main_script='${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo/main.plua'"
                -e "sys_path='${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo/sys/'"
                -e "path='${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo/'"
                ${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo/spec/ptusa_main.lua)
    endif()


    add_test(NAME save-speed-benchmark COMMAND main_perfomance_test
            --benchmark_out=${CMAKE_BINARY_DIR}/benchmark_result.json --benchmark_out_format=json
            --benchmark_context=pwd=`${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo`
            --benchmark_counters_tabular=true
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/demo_projects/T1-PLCnext-Demo)

    set_property(TEST save-speed-benchmark PROPERTY RUN_SERIAL 1)
endif()

include(GoogleTest)
gtest_discover_tests(main_test DISCOVERY_TIMEOUT 30)
