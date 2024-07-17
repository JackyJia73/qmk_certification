#pragma once

#include "quantum.h"

#define BLEMode B12
#define TwoMode B14

enum kb_mode_t {
  KB_MODE_USB=0,
  KB_MODE_BLE,
  KB_MODE_24G,
  KB_MODE_DEFALT
};

