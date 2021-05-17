cmake_minimum_required(VERSION 3.13)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

project(Ptusa)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

################# create target #######################################################
add_definitions(-DPAC_PLCNEXT)
add_definitions(-DLINUX_OS)
add_definitions(-DTEST_SPEED)

set (WILDCARD_SOURCE *.cpp)
set (WILDCARD_HEADER *.h *.hpp *.hxx)

file(GLOB_RECURSE Headers CONFIGURE_DEPENDS src/${WILDCARD_HEADER} intermediate/code/${WILDCARD_HEADER})
file(GLOB_RECURSE Sources CONFIGURE_DEPENDS src/${WILDCARD_SOURCE} intermediate/code/${WILDCARD_SOURCE})

file(GLOB Main_src
	../../../../../common/PAC-driver/*.cpp
	../../../../../zlib/*.c
	../../../../../common/util/*.c
	../../../../../common/util/*.cpp
	../../../../../common/*.cpp

	../../../../../PAC/common/*.cpp
	../../../../../PAC/common/sys/*.cpp
	../../../../../PAC/linux/*.cpp
	../../../../../PAC/linux/PAC_PC/*.cpp
	../../../../../PAC/linux/PAC_PLCnext/*.cpp
	../../../../../PAC/IoT/*.cpp

	../../../../../lua/src/*.c
	../../../../../tolua++/src/lib/*.c
	)

list(FILTER Main_src EXCLUDE REGEX ".*OPCUA*")
list(FILTER Main_src EXCLUDE REGEX ".*rfid*")

list(FILTER Main_src EXCLUDE REGEX "wmain.c$")
list(FILTER Main_src EXCLUDE REGEX "luac.c$")
list(FILTER Main_src EXCLUDE REGEX "lua.c$")
list(FILTER Main_src EXCLUDE REGEX "loadlib_rel.c$")

add_library(Ptusa SHARED ${Headers} ${Sources} ${Main_src})

#######################################################################################

################# project include-paths ###############################################

target_include_directories(Ptusa
    PUBLIC
   	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common/util>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common/PAC-driver>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../zlib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../PAC/common>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../PAC/common/sys>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../PAC/linux>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../PAC/linux/PAC_PC>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../PAC/linux/PAC_PLCnext>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../PAC/IoT>

	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../tolua++/include>

	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../lua/src>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../bin>

    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/intermediate/code>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

#######################################################################################

################# include arp cmake module path #######################################

list(INSERT CMAKE_MODULE_PATH 0 "${ARP_TOOLCHAIN_CMAKE_MODULE_PATH}")

#######################################################################################

################# set RPATH to ORIGIN for thrid party libraries #######################
# Only by setting this is it possible to use libraries which are not already          #
# installed in the default install locations on the target.                           #
#######################################################################################

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    # With Windows there is a known issue with CMake: https://gitlab.kitware.com/cmake/cmake/-/issues/21291
    # This is currently the only way to set the RPATH to $ORIGIN in Windows
    target_link_libraries(Ptusa PRIVATE "-Wl,-rpath,\$$ORIGIN")
else()
    set_target_properties(Ptusa PROPERTIES INSTALL_RPATH $ORIGIN)
endif()

#######################################################################################

################# set link options ####################################################
# WARNING: Without --no-undefined the linker will not check, whether all necessary    #
#          libraries are linked. When a library which is necessary is not linked,     #
#          the firmware will crash and there will be NO indication why it crashed.    #
#######################################################################################

target_link_options(Ptusa PRIVATE LINKER:--no-undefined)

#######################################################################################

################# add link targets ####################################################

find_package(ArpDevice REQUIRED)
find_package(ArpProgramming REQUIRED)

target_link_libraries(Ptusa PRIVATE ArpDevice ArpProgramming dl)

#######################################################################################

################# install ############################################################

string(REGEX REPLACE "^.*\\(([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+).*$" "\\1" _ARP_SHORT_DEVICE_VERSION ${ARP_DEVICE_VERSION})
install(TARGETS Ptusa
    LIBRARY DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/lib
    ARCHIVE DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/lib
    RUNTIME DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/bin)
unset(_ARP_SHORT_DEVICE_VERSION)

#######################################################################################