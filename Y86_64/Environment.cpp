#include "stdafx.h"
#include "Environment.h"
#include <string.h>

char Memory[MAX_MEMORY_ADDRESS];


Environment::Environment()
{
	init();
}


Environment::~Environment()
{
	delete _vCPU;
	delete _state;
}

int64_t Environment::GetData(ADDRESS address)
{
	if (address <= 0 || address >= MAX_ADDRESS_BYTES) {
		_state->_programState = PS_ADR;
		return -1;
	}
	return Memory[address];
}

void Environment::SetData(ADDRESS address, int64_t value)
{
	if (address <= 0 || address >= MAX_ADDRESS_BYTES) {
		_state->_programState = PS_ADR;
		return;
	}
	Memory[address] = value;
}

void Environment::StopExec()
{

	fprintf(stdout, "%s\n", "CPU: EXECUTION Stopped!");
	getchar();
}

void Environment::Start()
{
	while (_state->_programState == PS_OK) {
		_vCPU->Execute();
	}

	if (_state->_programState == PS_HALT) {
		StopExec();
	}
}

void Environment::Load(char * buffer, size_t num)
{
}

void Environment::init()
{
	memset(Memory, 0, sizeof(Memory));
	_state = new ExecutionState;

	_state->_conditionCode = 0x0;
	_state->_programState = PS_OK;
	_state->_programCounter = START_ADDRESS;

	_vCPU = new V_CPU(_state);
}
