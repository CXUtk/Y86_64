#ifndef Y86_64_EXEC_STATE_H
#define Y86_64_EXEC_STATE_H
#include "ISA.h"

typedef struct {
	ADDRESS			_programCounter;
	Register		_conditionCode;
	ProgramState	_programState;
} ExecutionState;

extern unsigned char Memory[MAX_MEMORY_ADDRESS];

#endif