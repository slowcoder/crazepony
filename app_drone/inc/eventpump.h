#pragma once

#include "error.h"

#define EEVENTPUMP_NOINIT       (EEVENTPUMP_BASE+0)
#define EEVENTPUMP_TIMEOUT		(EEVENTPUMP_BASE+1)
#define EEVENTPUMP_OVERFLOW		(EEVENTPUMP_BASE+2)
#define EEVENTPUMP_NOMEM		(EEVENTPUMP_BASE+3)

typedef enum {
	eEventPumpSource_Invalid = 0,
	eEventPumpSource_Buttons,
} eEventPumpSource;

err_code eventpump_init(void);

err_code eventpump_post(eEventPumpSource source,uint32_t param);
err_code eventpump_post_irq(eEventPumpSource source,uint32_t param);
err_code eventpump_poll(eEventPumpSource *pSource,uint32_t *pParam,uint32_t uTimeoutMs);
