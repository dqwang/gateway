#ifndef __ERRNO_H__
#define __ERRNO_H__

typedef enum ERRNO{
	E_QUEUE_EMPTY = -2,
	E_QUEUE_FULL = -1,
	EOK = 0,
	E_INVALID_PACKET,
	E_NULL_POINTER,
	E_INVALID_CMD,
}errno_t;

#endif
