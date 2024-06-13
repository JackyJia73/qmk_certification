
# List the files for BLE on STM32WB
STM32UTILSRC = $(ST_LIBS)/Utilities/stm_list.c \
	$(ST_LIBS)/Utilities/otp.c

# Required include directories
STM32UTILINC = $(ST_LIBS)/Utilities

# Shared variables
SRC += $(STM32UTILSRC)
VPATH += $(STM32UTILINC)
