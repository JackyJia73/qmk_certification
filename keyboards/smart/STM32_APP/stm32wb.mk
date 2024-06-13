include $(ST_LIBS)/Middlewares/st_middlewares.mk
include $(ST_LIBS)/STM32_WPAN/st_wpan.mk
include $(ST_LIBS)/Utilities/st_util.mk
include $(ST_LIBS)/IPCCv1/driver.mk

# List the files for STM32WB
STM32WBSRC = $(ST_LIBS)/st_wireless.c \
	$(ST_LIBS)/st_tasks.c

# Required include directories
STM32WBINC = $(ST_LIBS)

# Shared variables
SRC += $(STM32WBSRC)
VPATH += $(STM32WBINC)

OPT_DEFS += -DST_APPLICATIONS_ENABLE
