#include <stdint.h>
#include <string.h>
#include "stm32f10x.h"

static void send_command(int command, void *message) {
   asm("mov r0, %[cmd];"
       "mov r1, %[msg];"
       "bkpt #0xAB"
         :
         : [cmd] "r" (command), [msg] "r" (message)
         : "r0", "r1", "memory");
}

void semih_puts(const char *pzStr) {
        uint32_t m[3]; // Handle, Pointer, Length

        //if( ( CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk ) == 0 )
        //  return;

        m[0] = 2; // STDERR
        m[1] = (uint32_t)pzStr;
        m[2] = strlen(pzStr);

        send_command(0x05, m); // 0x05 == SYS_WRITE
}
