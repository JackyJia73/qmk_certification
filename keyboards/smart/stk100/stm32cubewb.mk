ST_LIBS = $(TOP_DIR)/keyboards/smart/STM32_WPAN
ST_APP = $(TOP_DIR)/keyboards/smart/STM32_APP

# List the files for BLE on STM32WB
STM32_LIB_SRC = \
  $(ST_LIBS)/ble/core/template/ble_osal.c \
  $(ST_LIBS)/ble/svc/Src/hids.c \
  $(ST_LIBS)/ble/svc/Src/dis.c \
  $(ST_LIBS)/ble/svc/Src/svc_ctl.c \
  $(ST_LIBS)/interface/patterns/ble_thread/tl/hci_tl_if.c \
  $(ST_LIBS)/interface/patterns/ble_thread/tl/hci_tl.c \
  $(ST_LIBS)/interface/patterns/ble_thread/tl/shci_tl_if.c \
  $(ST_LIBS)/interface/patterns/ble_thread/tl/shci_tl.c \
  $(ST_LIBS)/interface/patterns/ble_thread/tl/tl_mbox.c \
  $(ST_LIBS)/interface/patterns/ble_thread/shci/shci.c \
  $(ST_LIBS)/ble/core/auto/ble_events.c \
  $(ST_LIBS)/ble/core/auto/ble_gap_aci.c \
  $(ST_LIBS)/ble/core/auto/ble_gatt_aci.c \
  $(ST_LIBS)/ble/core/auto/ble_hal_aci.c \
  $(ST_LIBS)/ble/core/auto/ble_l2cap_aci.c \
  $(ST_LIBS)/ble/core/auto/ble_hci_le.c \
  $(ST_LIBS)/ble/svc/Src/bas.c \
  $(ST_LIBS)/mac_802_15_4/src/mac_802_15_4_core_wb.c \
  $(ST_LIBS)/Utilities/stm_list.c \
	$(ST_LIBS)/Utilities/otp.c

# Required include directories
STM32_LIB_INC = $(ST_LIBS) \
  $(ST_LIBS)/ble/svc/Inc \
  $(ST_LIBS)/interface/patterns/ble_thread/tl \
  $(ST_LIBS)/ble/core/template \
  $(ST_LIBS)/interface/patterns/ble_thread/shci \
  $(ST_LIBS)/ble/core \
  $(ST_LIBS)/ble \
  $(ST_LIBS)/Middlewares/ST/STM32_WPAN \
  $(ST_LIBS)/interface/patterns/ble_thread \
  $(ST_LIBS)/mac_802_15_4/inc \
  $(ST_LIBS)/Utilities \

# Shared variables
SRC += $(STM32_LIB_SRC)
VPATH += $(STM32_LIB_INC)

OPT_DEFS += -DMAC_802_15_4_WB




# List the files for BLE on STM32WB
STM32_APP_SRC = \
  $(ST_APP)/app_ble.c \
	$(ST_APP)/dis_app.c \
	$(ST_APP)/hids_app.c \
	$(ST_APP)/bas_app.c \
	$(ST_APP)/app_mac_802_15_4.c \
  $(ST_APP)/hal_ipcc_lld.c \
  $(ST_APP)/hw_ipcc.c \
  $(ST_APP)/st_wireless.c \
	$(ST_APP)/st_tasks.c
  

# Required include directories
STM32_APP_INC = $(ST_APP) \

# Shared variables
SRC += $(STM32_APP_SRC)
VPATH += $(STM32_APP_INC)

OPT_DEFS += -DST_APPLICATIONS_ENABLE
