#pragma once

#include <stdint.h>

typedef uint32_t err_code;

#define ERROR_OK           (0)
#define ERROR_BASE         (0x80000000)
#define EBUTTONS_BASE      (ERROR_BASE + (0x01 << 16))
#define EEVENTPUMP_BASE    (ERROR_BASE + (0x02 << 16))
