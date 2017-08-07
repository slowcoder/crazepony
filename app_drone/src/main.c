#include <stdio.h>
#include <string.h>
#include <stm32f10x.h>

#include <FreeRTOS.h>
#include <task.h>
#include <cmsis_os.h>

#include "semihosting.h"
#include "drivers/i2c_arb.h"
#include "drivers/leds.h"
#include "drivers/motors.h"
#include "drivers/mpu6050.h"
#include "drivers/ms5611.h"
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
	int state = 0,leds;

	LOG("Drone starting");

	leds_init();
	i2c_arb_init();
	mpu6050_init();
	motors_init();

	int16_t acc[3];
	if( mpu6050_read_accel(&acc[0],&acc[1],&acc[2]) == ERROR_OK ) {
		LOG("Accel=[ %d, %d, %d ]",acc[0],acc[1],acc[2]);
	}

	nrf24_init();
    nrf24_config(2,4*2 + 1);

	uint8_t rx_mac[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
	uint8_t tx_mac[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
	nrf24_rx_address(rx_mac); // Our address
	nrf24_tx_address(tx_mac); // Destination address

    //nrf24_printDetails();
	//ms5611_init();

    LOG("Entering mainloop");
	for(;;) {

		GPIO_WriteBit(GPIOA,GPIO_Pin_11,!!(leds & (1<<0)));
		GPIO_WriteBit(GPIOA,GPIO_Pin_8 ,!!(leds & (1<<1)));
		GPIO_WriteBit(GPIOB,GPIO_Pin_1 ,!!(leds & (1<<2)));
		GPIO_WriteBit(GPIOB,GPIO_Pin_3 ,!!(leds & (1<<3)));

		leds = leds << 1;
		if( leds & 0xF0 ) leds = 1;

		state = !state;

		pkt_t pkt;

		if( nrf24_dataReady() ) {
			nrf24_getData(&pkt);
			//LOG("axis=[%d,%d,%d,%d]",pkt.axis[0],pkt.axis[1],pkt.axis[2],pkt.axis[3]);	

			int output;
			output = 4096 - pkt.axis[3];
			output /= 7;

			//LOG("Output=%d",output);

			motors_set_output(output,output,output,output);
		} else {
			motors_set_output(0,0,0,0);			
		}

		osDelay(75);
	}
}

int main(void) {

	SystemCoreClockUpdate();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	// Turn off full JTAG, as we're only using SWD, and need the other pins
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	osKernelStart();

	for(;;);

	return 0;
}
