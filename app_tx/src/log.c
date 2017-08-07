#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <log.h>
#include "semihosting.h"

static char tmpStr[80];//,tmpStr2[1024];

void __logi(const char *pzFunc,const char *pzFile,int line,const char *pzMessage,...) {
	va_list ap;

	if( strlen(pzMessage) > 128 ) {
	  semih_puts("Log output too long. Skipping\n");
	  return;
	}

//	snprintf(tmpStr,1023,"%s@%s:%i: ",pzFunc,pzFile,line);

	va_start(ap,pzMessage);
	vsnprintf(tmpStr,80,pzMessage,ap);
	va_end(ap);

//	strcat(tmpStr,tmpStr2);
	strcat(tmpStr,"\r\n");

	semih_puts(tmpStr);
}
