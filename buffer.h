#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stddef.h>

#include "semaphore.h"

typedef struct{

	size_t size;
	size_t nextIn, nextOut;

	int *buffer;

	semaphore empty;
	semaphore full;

} bBuffer;

void deposit(bBuffer *buffer, int c);
int remoove(bBuffer *buffer);
bBuffer *newBBuffer(size_t size);

#endif