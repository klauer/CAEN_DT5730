TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += gtrAD

# xxxRecord.h will be created from xxxRecord.dbd

INC += gtrBaseDriver.h

# install gtr.dbd into <top>/dbd
DBD += gtr.dbd

USR_CPPFLAGS = -std=c++0x -Wextra
USR_CFLAGS += -DDEBUG -g
USR_CPPFLAGS += -DDEBUG -g

# specify all source files to be compiled and added to the library
gtrAD_SRCS += gtrBaseDriver.h

gtrAD_LIBS += $(EPICS_BASE_IOC_LIBS)
gtrAd_LIBS += ADBase
gtrAd_LIBS += asyn

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

