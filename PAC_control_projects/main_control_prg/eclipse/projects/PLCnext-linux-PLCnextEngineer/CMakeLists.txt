cmake_minimum_required(VERSION 3.13)

#set(CMAKE_VERBOSE_MAKEFILE ON)

project(PtusaPLCnextEngineer)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

################# create target #######################################################
add_definitions(-DPAC_PLCNEXT)  
add_definitions(-DLINUX_OS)
add_definitions(-DTEST_SPEED)
add_definitions(-DLUA_USE_POSIX)


file(GLOB_RECURSE PLCnext_src src/*.cpp intermediate/code/*.cpp)

file(GLOB Main_src  
					../../../../../common/lua/*.c
					../../../../../common/PAC-driver/*.cpp
					../../../../../common/QuickLZ/*.c
					../../../../../common/util/*.c					
					../../../../../common/*.cpp
					
					../../../../../PAC/common/*.cpp
					../../../../../PAC/common/sys/*.cpp
					../../../../../PAC/linux/*.cpp
					../../../../../PAC/linux/PAC_PC/*.cpp
					../../../../../PAC/linux/PAC_PLCnext/*.cpp
					../../../../../PAC/IoT/*.cpp
					
					../../../../../tolua++/src/lib/*.c
					)				

list(FILTER Main_src EXCLUDE REGEX ".*OPCUA*")
list(FILTER Main_src EXCLUDE REGEX ".*rfid*")

add_library(PtusaPLCnextEngineer SHARED ${Main_src} ${PLCnext_src})

#######################################################################################

################# project include-paths ###############################################

target_include_directories(PtusaPLCnextEngineer
    PUBLIC
   	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common/lua>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common/util>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common/PAC-driver>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common/QuickLZ>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../PAC/common>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../PAC/common/sys>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../PAC/linux>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../PAC/linux/PAC_PC>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../PAC/linux/PAC_PLCnext>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../PAC/IoT>
    
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../tolua++/include>
    	
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/intermediate/code>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

#######################################################################################

################# include arp cmake module path #######################################

list(INSERT CMAKE_MODULE_PATH 0 "${ARP_TOOLCHAIN_CMAKE_MODULE_PATH}")

#######################################################################################

################# set link options ####################################################
# WARNING: Without --no-undefined the linker will not check, whether all necessary    #
#          libraries are linked. When a library which is necessary is not linked,     #
#          the firmware will crash and there will be NO indication why it crashed.    #
#######################################################################################

target_link_options(PtusaPLCnextEngineer PRIVATE LINKER:--no-undefined)

#######################################################################################

################# add link targets ####################################################

find_package(ArpDevice REQUIRED)
find_package(ArpProgramming REQUIRED)

target_link_libraries(PtusaPLCnextEngineer PRIVATE ArpDevice ArpProgramming)

#######################################################################################

################# install ############################################################

string(REGEX REPLACE "^.*\\(([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+).*$" "\\1" _ARP_SHORT_DEVICE_VERSION ${ARP_DEVICE_VERSION})
install(TARGETS PtusaPLCnextEngineer
    LIBRARY DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/lib
    ARCHIVE DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/lib
    RUNTIME DESTINATION ${ARP_DEVICE}_${_ARP_SHORT_DEVICE_VERSION}/$<CONFIG>/bin)
unset(_ARP_SHORT_DEVICE_VERSION)

#######################################################################################
