
# List the files for BLE on STM32WB
STM32WPANSRC = $(ST_LIBS)/STM32_WPAN/app_ble.c \
	$(ST_LIBS)/STM32_WPAN/dis_app.c \
	$(ST_LIBS)/STM32_WPAN/hids_app.c \
	$(ST_LIBS)/STM32_WPAN/bas_app.c \
	$(ST_LIBS)/STM32_WPAN/app_mac_802_15_4.c

# Required include directories
STM32WPANINC = $(ST_LIBS)/STM32_WPAN

# Shared variables
SRC += $(STM32WPANSRC)
VPATH += $(STM32WPANINC)
