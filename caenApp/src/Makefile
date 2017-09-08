TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += caenAD

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install caen.dbd into <top>/dbd
DBD += caen.dbd

USR_CPPFLAGS = -std=c++0x -Wextra

USR_CFLAGS += -DDEBUG -g
USR_CPPFLAGS += -DDEBUG -g

# specify all source files to be compiled and added to the library
#caen_SRCS += xxx
caenAD_SRCS += caenDriver.cpp
caenAD_SRCS += caenDT5730Driver.cpp
caenAD_SRCS += caenReg.cpp
caenAD_SRCS += caenDT5730Reg.cpp

caenAD_LIBS += $(EPICS_BASE_IOC_LIBS)
caenAD_LIBS += asyn
caenAD_LIBS += ADBase
caenAD_SYS_LIBS += CAENDigitizer

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

