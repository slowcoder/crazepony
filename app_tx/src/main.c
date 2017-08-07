#include <stdio.h>
#include <string.h>
#include <stm32f10x.h>

#include <FreeRTOS.h>
#include <task.h>
#include <cmsis_os.h>

#include "semihosting.h"
#include "eventpump.h"
#include "drivers/adc.h"
#include "drivers/buttons.h"
#include "drivers/leds.h"
#include "drivers/nrf24l01plus/nrf24.h"
#include "log.h"

static osThreadId defaultTaskHandle;

#pragma pack(push,1)
typedef struct {
	uint16_t axis[4];
	uint8_t  flags;
} pkt_t;
#pragma pack(pop)

void StartDefaultTask(void const * argument) {
	int state = 0;
	uint8_t flags = 0;

	LOG("Starting transmitter");
	if( eventpump_init() != ERROR_OK ) {
		LOG("Failed to init eventpump");
	}
	leds_init();
	adc_init();
	buttons_init();

	nrf24_init();
    nrf24_config(2,4*2 + 1);

	uint8_t rx_mac[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
	uint8_t tx_mac[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
	nrf24_rx_address(tx_mac); // Our address
	nrf24_tx_address(rx_mac); // Destination address

    //nrf24_printDetails();

	LOG("Entering mainloop");

	int delay = 0;

	while(1) {
		eEventPumpSource source;

		uint32_t param;
		if( eventpump_poll(&source,&param,delay) == ERROR_OK ) {
			switch(source) {
				case eEventPumpSource_Buttons:
					if( param & 1 ) flags |=  (1<<((param>>16)+0));
					else            flags &= ~(1<<((param>>16)+0));
					LOG("Got button! (0x%x) f=0x%02x",param,flags);
					break;
				default:
					LOG("Unhandled event 0x%08x",source);
					break;
			}
		}

		pkt_t pkt;

		adc_getValues(pkt.axis);
//		LOG("v=[ %u,%u,%u,%u ]",v[0],v[1],v[2],v[3]);

		GPIO_WriteBit(GPIOA,GPIO_Pin_12, state);
		GPIO_WriteBit(GPIOB,GPIO_Pin_4 , state);
		GPIO_WriteBit(GPIOB,GPIO_Pin_5 ,!state);
		GPIO_WriteBit(GPIOB,GPIO_Pin_6 , state);
		GPIO_WriteBit(GPIOB,GPIO_Pin_7 ,!state);

		pkt.flags = flags;
		nrf24_send((uint8_t*)&pkt);
		while( nrf24_isSending() != 0 ) vTaskDelay(1);

		state = !state;
		//delay = pkt.axis[0] / 40;
		//osDelay(delay);
	}
}

int main(void) {

	SystemCoreClockUpdate();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	osKernelStart();

	for(;;);

	return 0;
}
