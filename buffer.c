#include <assert.h>
#include <stdlib.h>

#include "buffer.h"

//New BoundedBuffer.
bBuffer *newBBuffer(size_t size) {

	if (size <= 0) {
		return NULL;
	}

	bBuffer *buffer = (bBuffer *) malloc(sizeof(bBuffer));
	if (buffer == NULL) {
		return NULL;
	}

	buffer->buffer = (int *) malloc(size * sizeof(int));

	//Makes sure the allocation worked.
	if (buffer->buffer == NULL) {
		return NULL;
	}

	buffer->size = size;
	buffer->nextIn = 0;
	buffer->nextOut = 0;


	createSem(&buffer->empty, size);
	createSem(&buffer->full, 0);

	return buffer;
}


//Used to put c in first available empty location.
//If full, then will block until there is an
//  empty location available.
void deposit(bBuffer *buffer, int c) {

	//Makes sure buffer still exists
	assert(buffer->buffer != NULL);

	//Waits for empty buffer
	down(&buffer->empty);

	buffer->buffer[buffer->nextIn] = c;
	buffer->nextIn = (buffer->nextIn + 1) % buffer->size;

	//Signals full buffer
	up(&buffer->full);

}

//Removes whatever is in the location that
//  was filled first.
//If empty, then will block until there is
//  something in the location.
int remoove(bBuffer *buffer) {

	int c;

	//Makes sure buffer still exists
	assert(buffer->buffer != NULL);

	//Waits for full buffer
	down(&buffer->full);

	c = buffer->buffer[buffer->nextOut];
	buffer->nextOut = (buffer->nextOut + 1) % buffer->size;

	//Signals empty buffer
	up(&buffer->empty);

	return c;

}