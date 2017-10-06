/**********************************/
/*    Written by Daniel Estrada   */
/**********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "st.h"
#include "buffer.h"

#define B_BUFFER_SIZE 20
#define STR_LENGTH_EIGHTY 80

void *newLines(void *args);
void *asterisks(void *args);
void *print(void *args);

typedef struct {
	bBuffer *inBuffer;
	bBuffer *outBuffer;
} tArgs;

int main(void) {

	int c;
	int i;

	bBuffer *buffers[3];
	st_thread_t threads[3];
	tArgs arguments[3];

	//Initializes the State Threads.
	if (st_init() < 0) {
		perror("Error with initialization: ");
		exit(1);
	}

	//Creation of buffers.
	for (i = 0; i < 3; i++) {
		if ((buffers[i] = newBBuffer(B_BUFFER_SIZE)) == NULL) {
			fprintf(stderr, "Creation of buffer failed.");
			exit(1);
		}
	}

	arguments[0].inBuffer = buffers[0];
	arguments[0].outBuffer = buffers[1];

	arguments[1].inBuffer = buffers[1];
	arguments[1].outBuffer = buffers[2];

	//The only difference here is that threads[2]
	//  will print out the buffer, so it will
	//  not be writing to another buffer like in
	//  the prior threads.
	arguments[2].inBuffer = buffers[2];
	arguments[2].outBuffer = NULL;

	//Here is where the threads are created, using
	//  their respective arguments.
	threads[0] = st_thread_create(newLines, &arguments[0], 1, 0);
	threads[1] = st_thread_create(asterisks, &arguments[1], 1, 0);
	threads[2] = st_thread_create(print, &arguments[2], 1, 0);

	if (!threads[0] || !threads[1] || !threads[2]) {
		fprintf(stderr, "Creation of threads failed.");
		exit(1);
	}

	//This is what reads in everything.
	while (!ferror(stdin) && !feof(stdin) && (c = getchar()) != EOF) {
		deposit(buffers[0], c);
	}

	//Tells *newLines when to stop.
	deposit(buffers[0], EOF);

	for (i = 0; i < 3; i++) {
		st_thread_join(threads[i], NULL);
	}

	return 0;

}

//Replaces newLine characters with
//  a space, unless an EOF, where the
//  the program will break, or exit.
void *newLines(void *args) {

	int c;

	bBuffer *input = ((tArgs *)args)->inBuffer;
	bBuffer *output = ((tArgs *)args)->outBuffer;

	while ((c = remoove(input)) != EOF) {
		if (c == '\n') {
			c = ' ';
		}
		deposit(output, c);
	}

	//Tells *asterisks when to stop.
	deposit(output, EOF);

	return 0;

}

//*asterisks very similar to *newLines.
//Only difference, is now whenever two asterisks
//  are consecutive, '**', they are both replaced by
//  a carrot, '^'.
void *asterisks(void *args) {

	int c, nextC;

	bBuffer *input = ((tArgs *)args)->inBuffer;
	bBuffer *output = ((tArgs *)args)->outBuffer;

	while ((c = remoove(input)) != EOF) {
		if (c == '*') {

			//Only replaces '**' with '^'.
			//If not two asterisks, deposit both
			//  c and nextC.
			nextC = remoove(input);
			if (nextC == '*') {
				deposit(output, '^');
			} else if (nextC == EOF) {
				break;
			} else {
				deposit(output, c);
				deposit(output, nextC);
			}

		} else {
			deposit(output, c);
		}
	}

	//Tells *print when to stop.
	deposit(output, EOF);

	return 0;

}

//This just prints out 80 character strings and stops
//  once an EOF is passed into remoove.
void *print(void *args) {

	char str[STR_LENGTH_EIGHTY + 1];
	int c;
	size_t i = 0;

	bBuffer *input = ((tArgs *)args)->inBuffer;

	str[STR_LENGTH_EIGHTY] = 0;

	while ((c = remoove(input)) != EOF) {
			str[i++] = c;
				if (i == STR_LENGTH_EIGHTY) {
					printf("%s\n", str);
					i = 0;
				}
	}

	return 0;

}
