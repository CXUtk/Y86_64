#include "stdafx.h"
#include "V_CPU.h"


V_CPU::V_CPU()
{
	Init();
}

V_CPU::V_CPU(ExecutionState * state)
{
	_state = state;
	Init();
}

V_CPU::~V_CPU()
{
}

ADDRESS V_CPU::Execute()
{
	ADDRESS pc = _state->_programCounter;
	ISA_ByteCode code = (ISA_ByteCode)Memory[pc];
	if (code == STOP) {
		_state->_programState = PS_HALT;
		return _state->_programCounter + 1;
	}
	else {
		if (_execFuncTable.find(code) != _execFuncTable.end() && _execFuncTable[code]) {
			ADDRESS addr = (this->*_execFuncTable[code])(pc, nullptr);
			return addr;
		}
		else {
			_state->_programState = PS_INS;
			return pc;
		}
	}
}

ADDRESS V_CPU::SysCall(ADDRESS addr, void * args)
{
	SysCall_Type type = (SysCall_Type)Memory[addr + 1];
	if (type == Sys_TestFunc) {
		fprintf(stdout, "%s\n", "系统调用#0，测试环境 正常");
	}
	if (type == Sys_PrintA) {
		ADDRESS target = internal_pop();
		// TODO：检测是否越界
		fprintf(stdout, "%s\n", Memory + target);
	}
	return addr + 2;
}

ADDRESS V_CPU::PushI(ADDRESS addr, void *)
{
	// 将立即数推进栈中
	_commonRegs[RSP] -= sizeof(uint64_t);
	ADDRESS sstop = _commonRegs[RSP];
	if (sstop < INIT_STACK_POS - MAX_STACK_GROW) {
		_state->_programState = PS_STACK_OVERFLOW;
		return addr;
	}
	memcpy(Memory + sstop, Memory + addr + 1, 8);
	return addr + 1 + 8;
}

ADDRESS V_CPU::Pop(ADDRESS addr, void *)
{
	ADDRESS sstop = _commonRegs[RSP];
	if (sstop >= INIT_STACK_POS) {
		_state->_programState = PS_STACK_UNDERFLOW;
		return addr;
	}
	// addr + 1 就是哪个寄存器的字节
	ISA_REGISTER target = (ISA_REGISTER)Memory[addr + 1];
	memcpy(_commonRegs + target, Memory + sstop, 8);
	_commonRegs[RSP] += sizeof(uint64_t);
	return addr + 1 + 8;
}

uint64_t V_CPU::internal_pop()
{
	ADDRESS sstop = _commonRegs[RSP];
	if (sstop >= INIT_STACK_POS) {
		_state->_programState = PS_STACK_UNDERFLOW;
	}
	// addr + 1 就是哪个寄存器的字节
	uint64_t result;
	memcpy(&result, Memory + sstop, 8);
	_commonRegs[RSP] += sizeof(uint64_t);
	return result;
}

void V_CPU::Init()
{
	memset(_commonRegs, 0, sizeof(_commonRegs));
	_commonRegs[RSP] = INIT_STACK_POS;
	_execFuncTable.insert(std::make_pair(SYSCALL, &V_CPU::SysCall));
	_execFuncTable.insert(std::make_pair(PUSHI, &V_CPU::PushI));
	_execFuncTable.insert(std::make_pair(POP, &V_CPU::Pop));
}


