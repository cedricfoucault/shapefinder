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
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/freqprocess.o \
	${OBJECTDIR}/tifwrap.o \
	${OBJECTDIR}/fileread.o \
	${OBJECTDIR}/shapefinder.o \
	${OBJECTDIR}/centerfinder.o \
	${OBJECTDIR}/complex.o \
	${OBJECTDIR}/cvwrap.o \
	${OBJECTDIR}/main.o


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
LDLIBSOPTIONS=-L../../../libraries_C/GnuWin32/lib -ltiff `pkg-config --libs opencv`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/shapefinder

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/shapefinder: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/shapefinder ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/freqprocess.o: freqprocess.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g `pkg-config --cflags opencv`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/freqprocess.o freqprocess.c

${OBJECTDIR}/tifwrap.o: tifwrap.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g `pkg-config --cflags opencv`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/tifwrap.o tifwrap.c

${OBJECTDIR}/fileread.o: fileread.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g `pkg-config --cflags opencv`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/fileread.o fileread.c

${OBJECTDIR}/shapefinder.o: shapefinder.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g `pkg-config --cflags opencv`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/shapefinder.o shapefinder.c

${OBJECTDIR}/centerfinder.o: centerfinder.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g `pkg-config --cflags opencv`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/centerfinder.o centerfinder.c

${OBJECTDIR}/complex.o: complex.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g `pkg-config --cflags opencv`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/complex.o complex.c

${OBJECTDIR}/cvwrap.o: cvwrap.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g `pkg-config --cflags opencv`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/cvwrap.o cvwrap.c

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -g `pkg-config --cflags opencv`    -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/shapefinder

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
