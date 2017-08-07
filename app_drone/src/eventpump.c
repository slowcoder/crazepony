#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "eventpump.h"

#pragma pack(push,1)
typedef struct {
	eEventPumpSource source;
	union {
		uint32_t wparam[2];
		uint32_t lparam;
	};
} ev_t;
#pragma pack(pop)

#define QUEUE_LEN 10

static QueueHandle_t xEPQueue = NULL;

err_code eventpump_init(void) {

	// Create a queue
	xEPQueue = xQueueCreate( QUEUE_LEN, sizeof(ev_t) );
	if( xEPQueue == NULL ) {
		return EEVENTPUMP_NOMEM;
	}

	return ERROR_OK;
}

err_code eventpump_post(eEventPumpSource source,uint32_t param) {
	ev_t ev;

	// TODO: More input-validation here? 

	ev.source = source;
	ev.lparam  = param; 

	if( xQueueSendToBack(xEPQueue, &ev, 1 / portTICK_PERIOD_MS) != pdPASS ) {
		return EEVENTPUMP_OVERFLOW;
	}

	return ERROR_OK;
}

err_code eventpump_post_irq(eEventPumpSource source,uint32_t param) {
	ev_t ev;

	// TODO: More input-validation here? 

	ev.source = source;
	ev.lparam  = param; 

	if( xQueueSendToBackFromISR(xEPQueue, &ev, NULL) != pdPASS ) {
		return EEVENTPUMP_OVERFLOW;
	}

	return ERROR_OK;	
}

err_code eventpump_poll(eEventPumpSource *pSource,uint32_t *pLparam,uint32_t uTimeoutMs) {
	ev_t ev;
	TickType_t actualtimeout;

	if( xEPQueue == NULL ) return EEVENTPUMP_NOINIT;

	if( uTimeoutMs == ~0 ) actualtimeout = portMAX_DELAY;
	else actualtimeout = uTimeoutMs / portTICK_PERIOD_MS;

	if( xQueueReceive(xEPQueue,&ev,actualtimeout) == pdTRUE ) {
		if( pSource != NULL ) *pSource = ev.source;
		if( pLparam  != NULL ) *pLparam  = ev.lparam;
		return ERROR_OK;
	}

	return EEVENTPUMP_TIMEOUT;
}
