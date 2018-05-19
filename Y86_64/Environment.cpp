#include "stdafx.h"
#include "Environment.h"
#include <string.h>

unsigned char Memory[MAX_MEMORY_ADDRESS];

void stop_exec(void * args)
{
	fprintf(stdout, "%s\n", "CPU: 执行已经停止！");
	getchar();
}

void invalid_addr(void * args)
{
	ExecutionState * state = (ExecutionState *)args;
	fprintf(stdout, "%s\n", "CPU: 地址无效！");
	fprintf(stdout, "最后执行地址：0x%X\n", state->_programCounter);
	stop_exec(args);
}

void invalid_ins(void * args)
{
	ExecutionState * state = (ExecutionState *)args;
	fprintf(stdout, "%s\n", "CPU: 无法识别指令！");
	fprintf(stdout, "最后执行地址：0x%X\n", state->_programCounter);
	stop_exec(args);
}

void stack_overflow(void * args)
{
	fprintf(stdout, "%s\n", "CPU: 栈空间溢出（Stack Overflow）！");
	stop_exec(args);
}

void stack_underflow(void * args)
{
	fprintf(stdout, "%s\n", "CPU: 栈空间向下溢出（Stack Underflow）！");
	stop_exec(args);
}

Environment::Environment()
{
	init();
}


Environment::~Environment()
{
	delete _vCPU;
	delete _state;
}

void Environment::Start()
{
	while (_state->_programState == PS_OK) {
		ADDRESS pc = _vCPU->Execute();
		_state->_programCounter = pc;
		CheckState();
	}
	DisplayMem(START_ADDRESS, 128);
	if (_globalStateTable.find(_state->_programState) != _globalStateTable.end()) {
		if (_globalStateTable[_state->_programState]) {
			_globalStateTable[_state->_programState](_state);
		}
	}
}

void Environment::Load(unsigned char * buffer, size_t num)
{
}

void Environment::SetMem(ADDRESS address, unsigned char * buffer, size_t num)
{
	if (address + num >= MAX_MEMORY_ADDRESS || address == 0) {
		_state->_programState = PS_ADR;
		return;
	}
	memcpy(Memory + address, buffer, num);
}

void Environment::DisplayMem(ADDRESS address, size_t num)
{
	for (size_t i = 0; i < num; i++) {
		fprintf(stdout, "%.2x ", Memory[address + i]);
	}
	fprintf(stdout, "\n");
	fflush(stdout);
}

void Environment::init()
{
	memset(Memory, 0, sizeof(Memory));
	_state = new ExecutionState;

	_state->_conditionCode = 0x0;
	_state->_programState = PS_OK;
	_state->_programCounter = START_ADDRESS;

	_vCPU = new V_CPU(_state);


	_globalStateTable.insert(std::make_pair(PS_HALT, stop_exec));
	_globalStateTable.insert(std::make_pair(PS_ADR, invalid_addr));
	_globalStateTable.insert(std::make_pair(PS_INS, invalid_ins));
	_globalStateTable.insert(std::make_pair(PS_STACK_OVERFLOW, stack_overflow));
	_globalStateTable.insert(std::make_pair(PS_STACK_UNDERFLOW, stack_underflow));
}


void Environment::CheckState()
{
	if (_state->_programCounter >= MAX_MEMORY_ADDRESS || _state->_programCounter == 0) {
		_state->_programState = PS_ADR;
	}
}