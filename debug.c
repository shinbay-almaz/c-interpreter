#include <stdio.h>

#include "debug.h"
#include "value.h"

void disassembleChunk(Chunk* chunk, const char* name) {
	printf("== %s ==\n", name);
	for (int offset = 0; offset < chunk->count;) {
		offset = disassembleInstruction(chunk, offset);
	}
}

static int constantInstruction(const char* name, Chunk* chunk, 
							   int offset) {
	uint8_t constant = chunk->code[offset + 1];
	printf("%-16s %4d '", name, constant);
	printValue(chunk->constants.values[constant]);
	printf("'\n");
	return offset + 2;
}

static int constantLongInstruction(const char* name, Chunk* chunk, 
								   int offset) {
	uint8_t constant1 = chunk->code[offset + 1];
	uint8_t constant2 = chunk->code[offset + 1];
	uint8_t constant3 = chunk->code[offset + 1];
	int constant = constant1 + ((int)constant2 << 8) + ((int)constant3 << 16);
	printf("%-16s %4d '", name, constant);
	printValue(chunk->constants.values[constant]);
	printf("'\n");
	return offset + 4;
}

static int simpleInstruction(const char* name, int offset) {
	printf("%s\n", name);
	return offset + 1;
}

static int byteInstruction(const char* name, Chunk* chunk, int offset) {
	uint8_t slot = chunk->code[offset + 1];
	printf("%-16s %4d\n", name, slot);
	return offset + 2;
}

static int jumpInstruction(const char* name, int sign,
						   Chunk* chunk, int offset) {
	uint16_t jump = (uint16_t)(chunk->code[offset + 1] << 8);
	jump |= chunk->code[offset + 2];
	printf("%-16s %4d -> %d\n", name, offset,
		   offset + 3 + sign * jump);
	return offset + 3;
}

int getLine(Chunk* chunk, int offset) {
	int index = offset;
	for (int i = 0; i < chunk->lineCount;) {
	//	printf("%d\n", chunk->lines[i]);
		if (chunk->lines[i] < 0) {
			if (index >= -chunk->lines[i]) index += chunk->lines[i];
			else return chunk->lines[i + 1];
			i = i + 2;
		} else {
			if (index == 0) return chunk->lines[i];
			i = i + 1;
		}
	}
}

int disassembleInstruction(Chunk* chunk, int offset) {
	printf("%04d ", offset);
	if (offset > 0 && 
		getLine(chunk, offset) == getLine(chunk, offset - 1)) {
		printf("   | ");
	} else {
		printf("%4d ", getLine(chunk, offset));
	}
	uint8_t instruction = chunk->code[offset];
	switch (instruction) {
		case OP_CONSTANT:
			return constantInstruction("OP_CONSTANT", chunk, offset);
		case OP_CONSTANT_LONG:
			return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
		case OP_NEGATE: 
			return simpleInstruction("OP_NEGATE", offset);
		case OP_NIL:
			return simpleInstruction("OP_NIL", offset);
		case OP_TRUE:
			return simpleInstruction("OP_TRUE", offset);
		case OP_FALSE:
			return simpleInstruction("OP_FALSE", offset);
		case OP_POP:
			return simpleInstruction("OP_POP", offset);
		case OP_GET_LOCAL:
			return byteInstruction("OP_GET_LOCAL", chunk, offset);
		case OP_GET_GLOBAL:
			return constantInstruction("OP_GET_GLOBAL", chunk, offset);
		case OP_DEFINE_GLOBAL:
			return constantInstruction("OP_DEFINE_GLOBAL", chunk, offset);
		case OP_SET_LOCAL:
			return byteInstruction("OP_SET_LOCAL", chunk, offset);
		case OP_SET_GLOBAL:
			return constantInstruction("OP_SET_GLOBAL", chunk, offset);
		case OP_EQUAL:
			return simpleInstruction("OP_EQUAL", offset);
		case OP_GREATER:
			return simpleInstruction("OP_GREATER", offset);
		case OP_LESS:
			return simpleInstruction("OP_LESS", offset);
		case OP_ADD: 
			return simpleInstruction("OP_ADD", offset);
		case OP_SUBTRACT: 
			return simpleInstruction("OP_SUBTRACT", offset);
		case OP_MULTIPLY:
			return simpleInstruction("OP_MULTIPLY", offset);
		case OP_DIVIDE:
			return simpleInstruction("OP_DIVIDE", offset);
		case OP_NOT:
			return simpleInstruction("OP_NOT", offset);
		case OP_PRINT:
			return simpleInstruction("OP_PRINT", offset);
		case OP_JUMP:
			return jumpInstruction("OP_JUMP", 1, chunk, offset);
		case OP_JUMP_IF_FALSE:
			return jumpInstruction("OP_JUMP_IF_FALSE", 1, chunk, offset);
		case OP_LOOP:
			return jumpInstruction("OP_LOOP", -1, chunk, offset);
		case OP_SWITCH_CASE:
			return simpleInstruction("OP_SWITCH_CASE", offset);
		case OP_RETURN:	
			return simpleInstruction("OP_RETURN", offset);
		default: 
			printf("Unknown opcode %d\n", instruction);
			return offset + 1;
	}
}

