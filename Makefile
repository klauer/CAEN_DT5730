#Makefile at top of application tree

# this makefile is used for development of this project
MODULE_NAME := caenAD
TARGET_USER := codac-dev
TARGET := 10.5.0.72

#TARGET := 10.5.3.175
#TARGET := 10.5.3.162
#TARGET := 10.5.2.98
TARGET_ROOT := ~/sandbox

TARGET_DIR := $(TARGET_ROOT)/$(MODULE_NAME)

%:
ifneq ($(TARGET_USER),$(USER))
#	rsync -avuzb --exclude ".svn" -e "$(RSYNC_PARAMS)" . $(TARGET_USER)@$(TARGET):$(TARGET_DIR)/m-epics-nixseries
# plink.exe -ssh -pw core09 $(TARGET_USER)@$(TARGET) "cd $(TARGET_DIR)/m-epics-nixseries; make $@"

	@ssh $(TARGET_USER)@$(TARGET) "mkdir -p $(TARGET_DIR)"
	rsync -avuzb --exclude ".svn" . $(TARGET_USER)@$(TARGET):$(TARGET_DIR)
	@ssh $(TARGET_USER)@$(TARGET) "cd $(TARGET_DIR); make clean uninstall; make $@"

else

TOP = .
include $(TOP)/configure/CONFIG
DIRS := $(DIRS) $(filter-out $(DIRS), configure)
##DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *App))
DIRS += gtrApp
DIRS += caenApp
DIRS += testApp
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard iocBoot))

define DIR_template
 $(1)_DEPEND_DIRS = configure
endef
$(foreach dir, $(filter-out configure,$(DIRS)),$(eval $(call DIR_template,$(dir))))

iocBoot_DEPEND_DIRS += $(filter %App,$(DIRS))

include $(TOP)/configure/RULES_TOP

endif