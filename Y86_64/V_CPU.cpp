#include "stdafx.h"
#include "V_CPU.h"


V_CPU::V_CPU()
{
	memset(_commonRegs, 0, sizeof(_commonRegs));
}

V_CPU::V_CPU(ExecutionState * state)
{
	_state = state;
}

V_CPU::~V_CPU()
{
}

ADDRESS V_CPU::Execute()
{
	ADDRESS pc = _state->_programCounter;
	if (Memory[pc] == STOP) 	_state->_programState = PS_HALT;
	return _state->_programCounter + 1;
}
