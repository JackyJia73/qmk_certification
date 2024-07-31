# List of all the board related files.
BOARDSRC = $(BOARD_PATH)/boards/ST_NUCLEO144_F723ZE/board.c

# Required include directories
BOARDINC = $(BOARD_PATH)/boards/ST_NUCLEO144_F723ZE

# Shared variables
ALLCSRC += $(BOARDSRC)
ALLINC  += $(BOARDINC)
