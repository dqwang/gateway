#ifndef __MAIN__H__
#define __MAIN__H__

#include "LPC8xx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "uart.h"
#include "type.h"
#include "gpio.h"
#include "queue.h"


typedef enum ERRNO{
	E_QUEUE_EMPTY = -2,
	E_QUEUE_FULL = -1,
	EOK = 0,
	E_INVALID_PACKET,
	E_NULL_POINTER,
	E_INVALID_CMD,
}errno_t;



#endif
