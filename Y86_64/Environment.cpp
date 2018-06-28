#include "stdafx.h"
#include "Environment.h"

unsigned char Memory[MAX_MEMORY_ADDRESS];

void Environment::stop_exec(void* args)
{
	fprintf(stdout, "%s\n", "CPU: 执行已经停止！");
	getchar();
}

void Environment::invalid_addr(void* args)
{
	auto state = static_cast<ExecutionState *>(args);
	fprintf(stdout, "%s\n", "CPU: 地址无效！");
	fprintf(stdout, "最后执行地址：0x%llX\n", state->_programCounter);
	stop_exec(args);
}

void Environment::invalid_ins(void* args)
{
	auto state = static_cast<ExecutionState *>(args);
	fprintf(stdout, "%s\n", "CPU: 无法识别指令！");
	fprintf(stdout, "最后执行地址：0x%llX\n", state->_programCounter);
	stop_exec(args);
}

void Environment::stack_overflow(void* args)
{
	auto state = static_cast<ExecutionState *>(args);
	fprintf(stdout, "%s\n", "CPU: 栈空间溢出（Stack Overflow）！");
	fprintf(stdout, "最后执行地址：0x%llX\n", state->_programCounter);
	stop_exec(args);
}

void Environment::stack_underflow(void* args)
{
	auto state = static_cast<ExecutionState *>(args);
	fprintf(stdout, "%s\n", "CPU: 栈空间向下溢出（Stack Underflow）！");
	fprintf(stdout, "最后执行地址：0x%llX\n", state->_programCounter);
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
	while (_state->_programState == PS_OK)
	{
		ADDRESS pc = _vCPU->Execute();
		_state->_programCounter = pc;
		CheckState();
	}
	DisplayMem(START_ADDRESS, 128);
	_vCPU->dump_regs();
	_vCPU->dump_stack();
	_vCPU->dump_heap();
	if (_globalStateTable.find(_state->_programState) != _globalStateTable.end())
	{
		if (_globalStateTable[_state->_programState])
		{
			(this->*_globalStateTable[_state->_programState])(_state);
		}
	}
}

void Environment::Load(unsigned char* buffer, size_t num)
{
}

void Environment::SetMem(ADDRESS address, unsigned char* buffer, size_t num)
{
	if (address + num >= MAX_MEMORY_ADDRESS || address == 0)
	{
		_state->_programState = PS_ADR;
		return;
	}
	memcpy(Memory + address, buffer, num);
}

void Environment::DisplayMem(ADDRESS address, size_t num)
{
	fprintf(stdout, "起始地址内存空间：");
	for (size_t i = 0; i < num; i++)
	{
		if (i == 0 || i % 16 == 0)
			fprintf(stdout, "\n0x%.8llX: ", address + i);
		fprintf(stdout, "%.2x ", Memory[address + i]);
	}
	fprintf(stdout, "\n\n");
	fflush(stdout);
}


void Environment::init()
{
	memset(Memory, 0, sizeof(Memory));
	_state = new ExecutionState;

	_state->_conditionCode = 0x0;
	_state->_programState = PS_OK;
	_state->_programCounter = START_ADDRESS;

	_vCPU = new v_cpu(_state);


	_globalStateTable.insert(std::make_pair(PS_HALT, &Environment::stop_exec));
	_globalStateTable.insert(std::make_pair(PS_ADR, &Environment::invalid_addr));
	_globalStateTable.insert(std::make_pair(PS_INS, &Environment::invalid_ins));
	_globalStateTable.insert(std::make_pair(PS_STACK_OVERFLOW, &Environment::stack_overflow));
	_globalStateTable.insert(std::make_pair(PS_STACK_UNDERFLOW, &Environment::stack_underflow));
}


void Environment::CheckState()
{
	if (_state->_programCounter >= MAX_MEMORY_ADDRESS || _state->_programCounter == 0)
	{
		_state->_programState = PS_ADR;
	}
}
