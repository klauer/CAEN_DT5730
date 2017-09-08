TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = test
# test.dbd will be created and installed
DBD += test.dbd

# test.dbd will be made up from these files:
test_DBD += base.dbd

# Include dbd files from all support applications:
test_DBD += asyn.dbd
test_DBD += caen.dbd
test_DBD += NDPluginStdArrays.dbd
test_DBD += busySupport.dbd


# test_registerRecordDeviceDriver.cpp derives from test.dbd
test_SRCS += test_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
test_SRCS_DEFAULT += testMain.cpp
test_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#test_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
test_LIBS += $(EPICS_BASE_IOC_LIBS)
test_LIBS += asyn
test_LIBS += ADBase NDPlugin
test_LIBS += gtrAD
test_LIBS += caenAD 
test_LIBS += busy
test_SYS_LIBS += CAENDigitizer


#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

