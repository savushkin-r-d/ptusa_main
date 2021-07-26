cmake_minimum_required(VERSION 3.13)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

project(ptusa)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
################# create targets ######################################################

add_definitions(-DPAC_PC)
add_definitions(-DDEBUG_NO_IO_MODULES)

#### Definitions for run tests ####
add_definitions(-DPTUSA_TEST)

if(LINUX)
    add_definitions(-DLINUX_OS)
elseif(WIN32)
    add_definitions(-DWIN_OS)
    add_definitions(-DSIMPLE_LOG)
endif()

add_definitions(-DTEST_SPEED)

if(MSVC)
    add_compile_options("/utf-8")
endif(MSVC)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lua ${CMAKE_CURRENT_SOURCE_DIR}/bin/lua EXCLUDE_FROM_ALL)

set(LUA_LIBRARIES liblua)
set(LUA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lua/src;${CMAKE_CURRENT_SOURCE_DIR}/bin/lua)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tolua++ ${CMAKE_CURRENT_SOURCE_DIR}/bin/tolua++ EXCLUDE_FROM_ALL)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/zlib ${CMAKE_CURRENT_SOURCE_DIR}/bin/zlib EXCLUDE_FROM_ALL)

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/googletest ${CMAKE_CURRENT_SOURCE_DIR}/bin/googletest EXCLUDE_FROM_ALL)

set(SUBHOOK_TESTS OFF)
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/subhook ${CMAKE_CURRENT_SOURCE_DIR}/bin/subhook EXCLUDE_FROM_ALL)

file(GLOB common_src CONFIGURE_DEPENDS
        common/PAC-driver/*.cpp
        common/util/*.c
        common/util/*.cpp
        common/*.cpp

        PAC/common/*.cpp
        PAC/common/sys/*.cpp
        PAC/IoT/*.cpp)

list(FILTER common_src EXCLUDE REGEX ".*OPCUA*|.*rfid*|snprintf\\.c")

file(GLOB test_src CONFIGURE_DEPENDS
    	test/*.cpp)

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
            PAC_control_projects/main_control_prg/main.cpp)

    add_library(main_control ${common_src} ${linux_src}
            PAC_control_projects/main_control_prg/main_control_dll_vc/main_control_dll_vc.cpp)

    add_executable(main_test ${common_src} ${linux_src} ${test_src}
           test/main_test.cpp)

elseif(WIN32)
    add_executable(${PROJECT_NAME} ${common_src} ${win_src}
            PAC_control_projects/main_control_prg/main.cpp)

    add_library(main_control ${common_src} ${win_src}
            PAC_control_projects/main_control_prg/main_control_dll_vc/main_control_dll_vc.cpp
            PAC_control_projects/main_control_prg/main_control_dll_vc/dllmain.cpp)

    add_executable(main_test ${common_src} ${win_src} ${test_src}
           test/main_test.cpp)


    add_custom_command( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/PAC_dev_lua.cpp
        COMMAND ${CMAKE_COMMAND} -E copy bin/lua/lua.dll bin/tolua++
        COMMAND toluapp -n PAC_dev -o PAC/common/PAC_dev_lua.cpp PAC/common/toloa++/pkg/PAC_dev.hh
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/toloa++/pkg/PAC_dev.hh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

    add_custom_command( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/PAC/IoT/IOT_dev_lua.cpp
        COMMAND ${CMAKE_COMMAND} -E copy bin/lua/lua.dll bin/tolua++
        COMMAND toluapp -n IOT_dev -o PAC/IoT/IOT_dev_lua.cpp PAC/common/toloa++/pkg/IOT_dev.hh
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/toloa++/pkg/IOT_dev.hh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )
endif()

target_compile_definitions(main_control PUBLIC _USRDLL)

target_compile_definitions(gmock PUBLIC GTEST_CREATE_SHARED_LIBRARY)
target_compile_definitions(main_test PUBLIC GTEST_LINKED_AS_SHARED_LIBRARY)

set_target_properties (subhook gtest gtest_main gmock gmock_main PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
#######################################################################################

################# project include-paths ###############################################

set(GENERAL_INCLUDES
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/util>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/PAC-driver>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/common>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/common/sys>

if (WIN32)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/win_util>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/win>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/win/PAC_PC>
elseif (LINUX)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PC>
endif ()

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/IoT>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tolua++/include>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lua/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bin/lua>

        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/zlib>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/bin/zlib>)

target_include_directories(${PROJECT_NAME} PUBLIC ${GENERAL_INCLUDES})
target_include_directories(main_control PUBLIC ${GENERAL_INCLUDES})

set(TEST_INCLUDES
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/googletest/googletest/include>
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/bin/googletest/googletest>

		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/googletest/googlemock/include>
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/bin/googlemock/googlemock>)

target_include_directories(main_test PUBLIC ${GENERAL_INCLUDES} ${TEST_INCLUDES})
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

################# add link targets ####################################################

target_link_libraries(${PROJECT_NAME} liblua_static toluapp_lib_static zlibstatic)
target_link_libraries(main_control toluapp_lib_static zlibstatic)

target_link_libraries(main_test liblua_static toluapp_lib_static zlibstatic gtest gtest_main gmock gmock_main subhook)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
    target_link_libraries(main_control wsock32 ws2_32)
    target_link_libraries(main_test wsock32 ws2_32)

    if(DEFINED ENV{LUA_DEV})
        target_link_libraries(main_control $ENV{LUA_DEV}/lib/lua51.lib)
    else()
        target_link_libraries(main_control liblua)
    endif()

elseif (LINUX)
    target_link_libraries(main_control liblua_static)
endif()
#######################################################################################

install_executable(${PROJECT_NAME})
install_library(main_control)

enable_testing()
add_test(run_no_command_line_arguments ${PROJECT_NAME})

include(GoogleTest)
gtest_discover_tests(main_test)

#remove tests from add_subdirectory() added projects
configure_file(${CMAKE_SOURCE_DIR}/CTestCustom.cmake ${CMAKE_BINARY_DIR} @ONLY)
