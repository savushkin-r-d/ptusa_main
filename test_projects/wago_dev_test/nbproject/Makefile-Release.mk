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
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/sys_w750.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/prj_mngr_w750.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/common/PAC-driver/g_device.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/wago.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/tcp_cmctr_w750.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/prj_mngr.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/wago_w750.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/tcp_cmctr.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/PAC_dev.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/kbusapi.o \
	${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/sys.o

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/GNU-Linux-x86/wago_dev_test

dist/Release/GNU-Linux-x86/wago_dev_test: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/wago_dev_test ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/sys_w750.o: nbproject/Makefile-${CND_CONF}.mk ../../PAC/uclinux/sys_w750.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/sys_w750.o ../../PAC/uclinux/sys_w750.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/prj_mngr_w750.o: nbproject/Makefile-${CND_CONF}.mk ../../PAC/uclinux/prj_mngr_w750.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/prj_mngr_w750.o ../../PAC/uclinux/prj_mngr_w750.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/common/PAC-driver/g_device.o: nbproject/Makefile-${CND_CONF}.mk ../../common/PAC-driver/g_device.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/common/PAC-driver
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/common/PAC-driver/g_device.o ../../common/PAC-driver/g_device.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/wago.o: nbproject/Makefile-${CND_CONF}.mk ../../PAC/common/wago.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/wago.o ../../PAC/common/wago.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/tcp_cmctr_w750.o: nbproject/Makefile-${CND_CONF}.mk ../../PAC/uclinux/tcp_cmctr_w750.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/tcp_cmctr_w750.o ../../PAC/uclinux/tcp_cmctr_w750.cpp

${OBJECTDIR}/main.o: nbproject/Makefile-${CND_CONF}.mk main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/prj_mngr.o: nbproject/Makefile-${CND_CONF}.mk ../../PAC/common/prj_mngr.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/prj_mngr.o ../../PAC/common/prj_mngr.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/wago_w750.o: nbproject/Makefile-${CND_CONF}.mk ../../PAC/uclinux/wago_w750.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/wago_w750.o ../../PAC/uclinux/wago_w750.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/tcp_cmctr.o: nbproject/Makefile-${CND_CONF}.mk ../../PAC/common/tcp_cmctr.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/tcp_cmctr.o ../../PAC/common/tcp_cmctr.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/PAC_dev.o: nbproject/Makefile-${CND_CONF}.mk ../../PAC/common/PAC_dev.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/PAC_dev.o ../../PAC/common/PAC_dev.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/kbusapi.o: nbproject/Makefile-${CND_CONF}.mk ../../PAC/uclinux/kbusapi.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/uclinux/kbusapi.o ../../PAC/uclinux/kbusapi.cpp

${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/sys.o: nbproject/Makefile-${CND_CONF}.mk ../../PAC/common/sys.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/_DOTDOT/_DOTDOT/PAC/common/sys.o ../../PAC/common/sys.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/wago_dev_test

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
