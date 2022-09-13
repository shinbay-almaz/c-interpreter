#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
	chunk->count = 0;
	chunk->capacity = 0;
	chunk->code = NULL;
	chunk->lineCount = 0;
	chunk->lineCapacity = 0;
	chunk->lines = NULL;
	initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
	FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
	FREE_ARRAY(int, chunk->lines, chunk->capacity);
	freeValueArray(&chunk->constants);
	initChunk(chunk);
}

void encodeLine(Chunk* chunk, int line) {
	if (!chunk->lineCount || 
		chunk->lines[chunk->lineCount - 1] != line) {
		if (chunk->lineCapacity < chunk->lineCount + 1) {
			int oldLineCapacity = chunk->lineCapacity;
			chunk->lineCapacity = GROW_CAPACITY(oldLineCapacity);
			chunk->lines = GROW_ARRAY(int, chunk->lines,
				oldLineCapacity, chunk->lineCapacity);
		}
		chunk->lines[chunk->lineCount] = line;
		chunk->lineCount++;
	} else {
		if (chunk->lineCount > 1 &&
			chunk->lines[chunk->lineCount - 2] < 0) {
			chunk->lines[chunk->lineCount - 2]--;
		} else {
			if (chunk->lineCapacity < chunk->lineCount + 1) {
				int oldLineCapacity = chunk->lineCapacity;
				chunk->lineCapacity = GROW_CAPACITY(oldLineCapacity);
				chunk->lines = GROW_ARRAY(int, chunk->lines,
					oldLineCapacity, chunk->lineCapacity);
			}
			chunk->lines[chunk->lineCount] = line;
			chunk->lines[chunk->lineCount - 1] = -2;
			chunk->lineCount++;
		}
	}
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
	if (chunk->capacity < chunk->count + 1) {
		int oldCapacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, 
			oldCapacity, chunk->capacity);
	}

	chunk->code[chunk->count] = byte;	
	encodeLine(chunk, line);
	chunk->count++;
}

int addConstant(Chunk* chunk, Value value) {
	writeValueArray(&chunk->constants, value);
	return chunk->constants.count - 1;
}

void writeConstant(Chunk* chunk, Value value, int line) {
	int constant = addConstant(chunk, value);
	if (constant < 256) {
		writeChunk(chunk, OP_CONSTANT, line);
		writeChunk(chunk, constant, line);
	}
	else {
		writeChunk(chunk, OP_CONSTANT_LONG, line);
		writeChunk(chunk, constant & 255, line);
		constant >>= 8;
		writeChunk(chunk, constant & 255, line);
		constant >>= 8;
		writeChunk(chunk, constant & 255, line);
	}
}
