# List of all the board related files.
BOARDSRC = $(BOARD_PATH)/board/board.c

# Required include directories
BOARDINC = $(BOARD_PATH)/board

#
# ST, yiwei
#
#ST_LIBS = $(TOP_DIR)/lib/st
#ST_CHIP = stm32wb
#ST_MAKE = $(ST_LIBS)/$(ST_CHIP).mk
#include $(ST_MAKE)

#include $(TOP_DIR)/keyboards/smart/stk100/stm32cubewb.mk

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)