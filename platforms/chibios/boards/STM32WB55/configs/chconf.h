// Copyright 2022 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// This will make scan frequency drop down from 3600+ to 1800+
//#define CH_CFG_ST_TIMEDELTA 0

#define CH_CFG_USE_MAILBOXES     TRUE

#include_next <chconf.h>
