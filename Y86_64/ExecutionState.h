#ifndef Y86_64_EXEC_STATE_H
#define Y86_64_EXEC_STATE_H
#include "ISA.h"

typedef struct {
	ADDRESS			_programCounter;
	Register		_conditionCode;
	ProgramStatus	_programState;
} ExecutionState;

extern char		Memory[MAX_MEMORY_ADDRESS];

#endif