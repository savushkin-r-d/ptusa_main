cmake_minimum_required(VERSION 3.13)

#set(CMAKE_VERBOSE_MAKEFILE ON)

if (LINUX)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif()

project(PtusaPLCnextEngineer)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

################# create target #######################################################
if (LINUX)
	add_definitions(-DPAC_PLCNEXT)
	add_definitions(-DLINUX_OS)

elseif (WIN32)
	add_definitions(-DWIN_OS)
	add_definitions(-DPAC_PC)
	add_definitions(-DDEBUG_NO_IO_MODULES)
	add_definitions(-DSIMPLE_LOG)
endif()

add_definitions(-DTEST_SPEED)

if (MSVC)
	add_compile_options("/utf-8")
endif (MSVC)

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/lua ${CMAKE_CURRENT_SOURCE_DIR}/lua_bin )

set ( LUA_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/lua_bin/lua.lib )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/tolua++ ${CMAKE_CURRENT_SOURCE_DIR}/tolua++_bin )

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/zlib ${CMAKE_CURRENT_SOURCE_DIR}/zlib_bin )

if (LINUX)
	set ( LINUX_EXTRA_DIR /PAC_control_projects/main_control_prg/eclipse/projects/PLCnext-linux-PLCnextEngineer )

	file(GLOB_RECURSE ${LINUX_EXTRA_DIR}/PLCnext_src  ${LINUX_EXTRA_DIR}/src/*.cpp  ${LINUX_EXTRA_DIR}/intermediate/code/*.cpp)
endif()

if (LINUX)
file(GLOB Main_src
	common/PAC-driver/*.cpp
	zlib/*.c
	common/util/*.c
	common/util/*.cpp
	common/*.cpp

	PAC/common/*.cpp
	PAC/common/sys/*.cpp
	PAC/linux/*.cpp
	PAC/linux/PAC_PC/*.cpp
	PAC/linux/PAC_PLCnext/*.cpp
	PAC/IoT/*.cpp

	tolua++/src/lib/*.c)
endif()

if (WIN32)
file(GLOB Main_src CONFIGURE_DEPENDS common/PAC-driver/*.cpp
	common/util/*.c
	common/util/*.cpp
	common/win_util/*.cpp
	common/*.cpp

	PAC/common/*.cpp
	PAC/common/sys/*.cpp
	PAC/win/*.cpp
	PAC/win/PAC_PC/*.cpp
	PAC/IoT/*.cpp

	tolua++/src/lib/*.c

	PAC_control_projects/main_control_prg/main.cpp)
endif()

list(FILTER Main_src EXCLUDE REGEX ".*OPCUA*")
list(FILTER Main_src EXCLUDE REGEX ".*rfid*")
list(FILTER Main_src EXCLUDE REGEX ".*snprintf*")

if (LINUX)
	add_library(PtusaPLCnextEngineer SHARED ${Main_src} ${PLCnext_src})

elseif(WIN32)
	add_executable(PtusaPLCnextEngineer ${Main_src} )
endif()

#######################################################################################

################# project include-paths ###############################################

target_include_directories(PtusaPLCnextEngineer
	PUBLIC
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
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/linux/PAC_PLCnext>

	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/intermediate/code>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
endif()

	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PAC/IoT>

	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tolua++/include>

	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lua/src>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lua_bin>

	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/zlib>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/zlib_bin>)
#######################################################################################

################# include arp cmake module path #######################################
if (LINUX)
	list(INSERT CMAKE_MODULE_PATH 0 "${ARP_TOOLCHAIN_CMAKE_MODULE_PATH}")
endif()

#######################################################################################

################# set link options ####################################################
# WARNING: Without --no-undefined the linker will not check, whether all necessary    #
#          libraries are linked. When a library which is necessary is not linked,     #
#          the firmware will crash and there will be NO indication why it crashed.    #
#######################################################################################
if (LINUX)
	target_link_options(PtusaPLCnextEngineer PRIVATE LINKER:--no-undefined)
endif()

#######################################################################################

################# add link targets ####################################################

if (LINUX)
	find_package(ArpDevice REQUIRED)
	find_package(ArpProgramming REQUIRED)

	target_link_libraries(PtusaPLCnextEngineer PRIVATE ArpDevice ArpProgramming )

elseif (WIN32)
	target_link_libraries(PtusaPLCnextEngineer liblua_static zlib wsock32 ws2_32)
endif()
#######################################################################################

################# install ############################################################

if (LINUX)
	string(REGEX REPLACE "^.*\\(([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+).*$" "\\1" _ARP_SHORT_DEVICE_VERSION ${ARP_DEVICE_VERSION})
	install(TARGETS PtusaPLCnextEngineer
		LIBRARY DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/lib
		ARCHIVE DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/lib
		RUNTIME DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/bin)
	unset(_ARP_SHORT_DEVICE_VERSION)
endif()
#######################################################################################
