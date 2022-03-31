#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Example.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/Example.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=demoP2.c /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/ADC/ADC.c /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/PWM/PWM.c /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/Timers/Timers.c /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/UART/UART.c /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/ExternalInterrupt/ExternalInterrupt.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/demoP2.o ${OBJECTDIR}/_ext/475165527/ADC.o ${OBJECTDIR}/_ext/475150513/PWM.o ${OBJECTDIR}/_ext/1135468837/Timers.o ${OBJECTDIR}/_ext/1844635963/UART.o ${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o
POSSIBLE_DEPFILES=${OBJECTDIR}/demoP2.o.d ${OBJECTDIR}/_ext/475165527/ADC.o.d ${OBJECTDIR}/_ext/475150513/PWM.o.d ${OBJECTDIR}/_ext/1135468837/Timers.o.d ${OBJECTDIR}/_ext/1844635963/UART.o.d ${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/demoP2.o ${OBJECTDIR}/_ext/475165527/ADC.o ${OBJECTDIR}/_ext/475150513/PWM.o ${OBJECTDIR}/_ext/1135468837/Timers.o ${OBJECTDIR}/_ext/1844635963/UART.o ${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o

# Source Files
SOURCEFILES=demoP2.c /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/ADC/ADC.c /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/PWM/PWM.c /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/Timers/Timers.c /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/UART/UART.c /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/ExternalInterrupt/ExternalInterrupt.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/Example.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX795F512L
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/demoP2.o: demoP2.c  .generated_files/flags/default/f08d10f6602ffc37a7973827b7cd6e435d322a5e .generated_files/flags/default/f771c43d2b5d2a67da2c4ea3e647e16a157ee5dd
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/demoP2.o.d 
	@${RM} ${OBJECTDIR}/demoP2.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -fno-common -MP -MMD -MF "${OBJECTDIR}/demoP2.o.d" -o ${OBJECTDIR}/demoP2.o demoP2.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/_ext/475165527/ADC.o: /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/ADC/ADC.c  .generated_files/flags/default/2b98b6470eb9d3f5d0b942452222d620ed3b1c46 .generated_files/flags/default/f771c43d2b5d2a67da2c4ea3e647e16a157ee5dd
	@${MKDIR} "${OBJECTDIR}/_ext/475165527" 
	@${RM} ${OBJECTDIR}/_ext/475165527/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/475165527/ADC.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -fno-common -MP -MMD -MF "${OBJECTDIR}/_ext/475165527/ADC.o.d" -o ${OBJECTDIR}/_ext/475165527/ADC.o /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/ADC/ADC.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/_ext/475150513/PWM.o: /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/PWM/PWM.c  .generated_files/flags/default/282f010ab45296598736442da3d50b1ebde0b461 .generated_files/flags/default/f771c43d2b5d2a67da2c4ea3e647e16a157ee5dd
	@${MKDIR} "${OBJECTDIR}/_ext/475150513" 
	@${RM} ${OBJECTDIR}/_ext/475150513/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/475150513/PWM.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -fno-common -MP -MMD -MF "${OBJECTDIR}/_ext/475150513/PWM.o.d" -o ${OBJECTDIR}/_ext/475150513/PWM.o /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/PWM/PWM.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/_ext/1135468837/Timers.o: /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/Timers/Timers.c  .generated_files/flags/default/35c14c8bfa99585e0c5dc950d09cad0869a1c972 .generated_files/flags/default/f771c43d2b5d2a67da2c4ea3e647e16a157ee5dd
	@${MKDIR} "${OBJECTDIR}/_ext/1135468837" 
	@${RM} ${OBJECTDIR}/_ext/1135468837/Timers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1135468837/Timers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -fno-common -MP -MMD -MF "${OBJECTDIR}/_ext/1135468837/Timers.o.d" -o ${OBJECTDIR}/_ext/1135468837/Timers.o /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/Timers/Timers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/_ext/1844635963/UART.o: /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/UART/UART.c  .generated_files/flags/default/e053feaa7cbfbfb43b6bdfe46807dff83e78a199 .generated_files/flags/default/f771c43d2b5d2a67da2c4ea3e647e16a157ee5dd
	@${MKDIR} "${OBJECTDIR}/_ext/1844635963" 
	@${RM} ${OBJECTDIR}/_ext/1844635963/UART.o.d 
	@${RM} ${OBJECTDIR}/_ext/1844635963/UART.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -fno-common -MP -MMD -MF "${OBJECTDIR}/_ext/1844635963/UART.o.d" -o ${OBJECTDIR}/_ext/1844635963/UART.o /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/UART/UART.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o: /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/ExternalInterrupt/ExternalInterrupt.c  .generated_files/flags/default/e8dc7f32f14585e7e9c83fa4ef8f63d1b55abe3c .generated_files/flags/default/f771c43d2b5d2a67da2c4ea3e647e16a157ee5dd
	@${MKDIR} "${OBJECTDIR}/_ext/1201089759" 
	@${RM} ${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -fno-common -MP -MMD -MF "${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o.d" -o ${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/ExternalInterrupt/ExternalInterrupt.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
else
${OBJECTDIR}/demoP2.o: demoP2.c  .generated_files/flags/default/3b95f5c7b4c83745fb433909aefa1607b42431d0 .generated_files/flags/default/f771c43d2b5d2a67da2c4ea3e647e16a157ee5dd
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/demoP2.o.d 
	@${RM} ${OBJECTDIR}/demoP2.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -fno-common -MP -MMD -MF "${OBJECTDIR}/demoP2.o.d" -o ${OBJECTDIR}/demoP2.o demoP2.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/_ext/475165527/ADC.o: /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/ADC/ADC.c  .generated_files/flags/default/790091fd0eef0990ecf26a598c7801b0e6b7d36c .generated_files/flags/default/f771c43d2b5d2a67da2c4ea3e647e16a157ee5dd
	@${MKDIR} "${OBJECTDIR}/_ext/475165527" 
	@${RM} ${OBJECTDIR}/_ext/475165527/ADC.o.d 
	@${RM} ${OBJECTDIR}/_ext/475165527/ADC.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -fno-common -MP -MMD -MF "${OBJECTDIR}/_ext/475165527/ADC.o.d" -o ${OBJECTDIR}/_ext/475165527/ADC.o /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/ADC/ADC.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/_ext/475150513/PWM.o: /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/PWM/PWM.c  .generated_files/flags/default/6e9cfffd5daccd5dbcab01be1637907816f050b0 .generated_files/flags/default/f771c43d2b5d2a67da2c4ea3e647e16a157ee5dd
	@${MKDIR} "${OBJECTDIR}/_ext/475150513" 
	@${RM} ${OBJECTDIR}/_ext/475150513/PWM.o.d 
	@${RM} ${OBJECTDIR}/_ext/475150513/PWM.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -fno-common -MP -MMD -MF "${OBJECTDIR}/_ext/475150513/PWM.o.d" -o ${OBJECTDIR}/_ext/475150513/PWM.o /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/PWM/PWM.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/_ext/1135468837/Timers.o: /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/Timers/Timers.c  .generated_files/flags/default/a54894f959f3c73a2eac10139dc76911ea798094 .generated_files/flags/default/f771c43d2b5d2a67da2c4ea3e647e16a157ee5dd
	@${MKDIR} "${OBJECTDIR}/_ext/1135468837" 
	@${RM} ${OBJECTDIR}/_ext/1135468837/Timers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1135468837/Timers.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -fno-common -MP -MMD -MF "${OBJECTDIR}/_ext/1135468837/Timers.o.d" -o ${OBJECTDIR}/_ext/1135468837/Timers.o /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/Timers/Timers.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/_ext/1844635963/UART.o: /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/UART/UART.c  .generated_files/flags/default/50cf9917ed497b7fa04e62efab81ef01d56b5437 .generated_files/flags/default/f771c43d2b5d2a67da2c4ea3e647e16a157ee5dd
	@${MKDIR} "${OBJECTDIR}/_ext/1844635963" 
	@${RM} ${OBJECTDIR}/_ext/1844635963/UART.o.d 
	@${RM} ${OBJECTDIR}/_ext/1844635963/UART.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -fno-common -MP -MMD -MF "${OBJECTDIR}/_ext/1844635963/UART.o.d" -o ${OBJECTDIR}/_ext/1844635963/UART.o /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/UART/UART.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o: /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/ExternalInterrupt/ExternalInterrupt.c  .generated_files/flags/default/a80d75ae212c1d0123b8e1203b0a93584fe1f7bc .generated_files/flags/default/f771c43d2b5d2a67da2c4ea3e647e16a157ee5dd
	@${MKDIR} "${OBJECTDIR}/_ext/1201089759" 
	@${RM} ${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o 
	${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -fno-common -MP -MMD -MF "${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o.d" -o ${OBJECTDIR}/_ext/1201089759/ExternalInterrupt.o /home/carlosfmv/Desktop/SIE/Pratica/SIE/Common/ExternalInterrupt/ExternalInterrupt.c    -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -mdfp="${DFP_DIR}"  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/Example.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE) -g -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/Example.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)   -mreserve=data@0x0:0x1FC -mreserve=boot@0x1FC02000:0x1FC02FEF -mreserve=boot@0x1FC02000:0x1FC024FF  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D=__DEBUG_D,--defsym=__MPLAB_DEBUGGER_PK3=1,--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,${DISTDIR}/memoryfile.xml -mdfp="${DFP_DIR}"
	
else
${DISTDIR}/Example.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/Example.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}          -DXPRJ_default=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--no-code-in-dinit,--no-dinit-in-serial-mem,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,${DISTDIR}/memoryfile.xml -mdfp="${DFP_DIR}"
	${MP_CC_DIR}/xc32-bin2hex ${DISTDIR}/Example.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
