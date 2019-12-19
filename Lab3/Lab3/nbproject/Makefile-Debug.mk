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
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/55904c/deliver.o \
	${OBJECTDIR}/_ext/1d1c0a54/server.o \
	${OBJECTDIR}/_ext/5c0/deliver.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lab3

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lab3: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lab3 ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/55904c/deliver.o: ../../Lab2Submission/Section\ 3\ of\ Lab\ \(file\ transfer\)/deliver.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/55904c
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/55904c/deliver.o ../../Lab2Submission/Section\ 3\ of\ Lab\ \(file\ transfer\)/deliver.c

${OBJECTDIR}/_ext/1d1c0a54/server.o: ../../Lab2Submission/Section\ 3\ of\ Lab\ \(file\ transfer\)/serverfolder/server.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1d1c0a54
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1d1c0a54/server.o ../../Lab2Submission/Section\ 3\ of\ Lab\ \(file\ transfer\)/serverfolder/server.c

${OBJECTDIR}/_ext/5c0/deliver.o: ../deliver.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/5c0
	${RM} "$@.d"
	$(COMPILE.c) -g -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5c0/deliver.o ../deliver.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/lab3

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
