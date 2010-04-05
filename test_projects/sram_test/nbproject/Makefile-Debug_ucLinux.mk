#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=as

# Macros
CND_PLATFORM=arm-uclinux-elf-Linux-x86
CND_CONF=Debug_ucLinux
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/common/PAC-driver/g_device.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/common/PAC_dev.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/common/sys.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/uclinux/sys_w750.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/common/param_ex.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/uclinux/tcp_cmctr.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-O2 -fomit-frame-pointer -Dlinux -D__linux__ -Dunix -D__uClinux__ -DEMBED -I/home/id/uclinux-dist/lib/uClibc/include -I/home/id/uclinux-dist/lib/libm -I/home/id/uclinux-dist/lib/libcrypt_old -I/home/id/uclinux-dist -fno-builtin -nostartfiles -I/home/id/uclinux-dist/linux-2.6.x/include
CXXFLAGS=-O2 -fomit-frame-pointer -Dlinux -D__linux__ -Dunix -D__uClinux__ -DEMBED -I/home/id/uclinux-dist/lib/uClibc/include -I/home/id/uclinux-dist/lib/libm -I/home/id/uclinux-dist/lib/libcrypt_old -I/home/id/uclinux-dist -fno-builtin -nostartfiles -I/home/id/uclinux-dist/linux-2.6.x/include

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Debug_ucLinux.mk dist/Debug_ucLinux/arm-uclinux-elf-Linux-x86/sram_test

dist/Debug_ucLinux/arm-uclinux-elf-Linux-x86/sram_test: ${OBJECTFILES}
	${MKDIR} -p dist/Debug_ucLinux/arm-uclinux-elf-Linux-x86
	${LINK.cc} -Wall -O2 -fomit-frame-pointer -Dlinux -D__linux__ -Dunix -D__uClinux__ -DEMBED -I/home/id/uclinux-dist/lib/uClibc/include -I/home/id/uclinux-dist/lib/libm -I/home/id/uclinux-dist/lib/libcrypt_old -I/home/id/uclinux-dist -fno-builtin -nostartfiles -I/home/id/uclinux-dist/linux-2.6.x/include -Wl,-elf2flt /home/id/uclinux-dist/lib/uClibc/lib/crt0.o /home/id/uclinux-dist/lib/uClibc/lib/crti.o /home/id/uclinux-dist/lib/uClibc/lib/crtn.o -L/home/id/uclinux-dist/lib/uClibc/. -L/home/id/uclinux-dist/lib/uClibc/lib -L/home/id/uclinux-dist/lib/libm -L/home/id/uclinux-dist/lib/libnet -L/home/id/uclinux-dist/lib/libdes -L/home/id/uclinux-dist/lib/libaes -L/home/id/uclinux-dist/lib/libpcap -L/home/id/uclinux-dist/lib/libcrypt_old -L/home/id/uclinux-dist/lib/libssl -L/home/id/uclinux-dist/lib/zlib -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sram_test ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/common/PAC-driver/g_device.o: nbproject/Makefile-${CND_CONF}.mk ../../src/common/PAC-driver/g_device.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/common/PAC-driver
	${RM} $@.d
	$(COMPILE.cc) -Wall -DUSE_NO_TANK_COMB_DEVICE -DDEBUG -DPAC -DW750 -DDEBUG_DEV_CMCTR -I../../src/PAC/uclinux -I../../src/PAC/common -I../../src/common/PAC-driver -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/common/PAC-driver/g_device.o ../../src/common/PAC-driver/g_device.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/common/PAC_dev.o: nbproject/Makefile-${CND_CONF}.mk ../../src/PAC/common/PAC_dev.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/common
	${RM} $@.d
	$(COMPILE.cc) -Wall -DUSE_NO_TANK_COMB_DEVICE -DDEBUG -DPAC -DW750 -DDEBUG_DEV_CMCTR -I../../src/PAC/uclinux -I../../src/PAC/common -I../../src/common/PAC-driver -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/common/PAC_dev.o ../../src/PAC/common/PAC_dev.cpp

${OBJECTDIR}/main.o: nbproject/Makefile-${CND_CONF}.mk main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -Wall -DUSE_NO_TANK_COMB_DEVICE -DDEBUG -DPAC -DW750 -DDEBUG_DEV_CMCTR -I../../src/PAC/uclinux -I../../src/PAC/common -I../../src/common/PAC-driver -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/common/sys.o: nbproject/Makefile-${CND_CONF}.mk ../../src/PAC/common/sys.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/common
	${RM} $@.d
	$(COMPILE.cc) -Wall -DUSE_NO_TANK_COMB_DEVICE -DDEBUG -DPAC -DW750 -DDEBUG_DEV_CMCTR -I../../src/PAC/uclinux -I../../src/PAC/common -I../../src/common/PAC-driver -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/common/sys.o ../../src/PAC/common/sys.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/uclinux/sys_w750.o: nbproject/Makefile-${CND_CONF}.mk ../../src/PAC/uclinux/sys_w750.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/uclinux
	${RM} $@.d
	$(COMPILE.cc) -Wall -DUSE_NO_TANK_COMB_DEVICE -DDEBUG -DPAC -DW750 -DDEBUG_DEV_CMCTR -I../../src/PAC/uclinux -I../../src/PAC/common -I../../src/common/PAC-driver -lc -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/uclinux/sys_w750.o ../../src/PAC/uclinux/sys_w750.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/common/param_ex.o: nbproject/Makefile-${CND_CONF}.mk ../../src/PAC/common/param_ex.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/common
	${RM} $@.d
	$(COMPILE.cc) -Wall -DUSE_NO_TANK_COMB_DEVICE -DDEBUG -DPAC -DW750 -DDEBUG_DEV_CMCTR -I../../src/PAC/uclinux -I../../src/PAC/common -I../../src/common/PAC-driver -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/common/param_ex.o ../../src/PAC/common/param_ex.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/uclinux/tcp_cmctr.o: nbproject/Makefile-${CND_CONF}.mk ../../src/PAC/uclinux/tcp_cmctr.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/uclinux
	${RM} $@.d
	$(COMPILE.cc) -Wall -DUSE_NO_TANK_COMB_DEVICE -DDEBUG -DPAC -DW750 -DDEBUG_DEV_CMCTR -I../../src/PAC/uclinux -I../../src/PAC/common -I../../src/common/PAC-driver -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/src/PAC/uclinux/tcp_cmctr.o ../../src/PAC/uclinux/tcp_cmctr.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Debug_ucLinux
	${RM} dist/Debug_ucLinux/arm-uclinux-elf-Linux-x86/sram_test

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
