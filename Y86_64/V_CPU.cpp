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
	ISA_OPCode code = (ISA_OPCode)Memory[pc];

	if (_execFuncTable.find(code) != _execFuncTable.end() && _execFuncTable[code]) {
		ADDRESS addr = (this->*_execFuncTable[code])(pc, nullptr);
		return addr;
	}
	else {
		_state->_programState = PS_INS;
		return pc;
	}

}

ADDRESS V_CPU::Stop(ADDRESS, void * args)
{
	_state->_programState = PS_HALT;
	return _state->_programCounter + 1;
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

ADDRESS V_CPU::PushI(ADDRESS addr, void * args)
{
	// 将立即数[addr + 1]推进栈中
	_commonRegs[RSP] -= sizeof(uint64_t);
	ADDRESS sstop = _commonRegs[RSP];
	if (sstop < INIT_STACK_POS - MAX_STACK_GROW) {
		_state->_programState = PS_STACK_OVERFLOW;
		return addr;
	}
	memcpy(&Memory[sstop], &Memory[addr + 1], MAX_ADDRESS_BYTES);
	return addr + 1 + MAX_ADDRESS_BYTES;
}

ADDRESS V_CPU::Push(ADDRESS addr, void *)
{
	// [addr + 1]的值是寄存器编号
	ISA_Register reg = (ISA_Register)Memory[addr + 1];
	_commonRegs[RSP] -= MAX_ADDRESS_BYTES;
	ADDRESS sstop = _commonRegs[RSP];
	if (sstop < INIT_STACK_POS - MAX_STACK_GROW) {
		_state->_programState = PS_STACK_OVERFLOW;
		return addr;
	}
	memcpy(&Memory[sstop], &_commonRegs[reg], MAX_ADDRESS_BYTES);
	return addr + 2;
}

ADDRESS V_CPU::Pop(ADDRESS addr, void * args)
{
	ADDRESS sstop = _commonRegs[RSP];
	if (sstop >= INIT_STACK_POS) {
		_state->_programState = PS_STACK_UNDERFLOW;
		return addr;
	}
	// addr + 1 就是哪个寄存器的字节
	ISA_Register target = (ISA_Register)Memory[addr + 1];
	memcpy(&_commonRegs[target], &Memory[sstop], MAX_ADDRESS_BYTES);
	_commonRegs[RSP] += MAX_ADDRESS_BYTES;
	return addr + 2;
}

ADDRESS V_CPU::Nop(ADDRESS addr, void * args)
{
	return addr + 1;
}

ADDRESS V_CPU::MovQuadR2R(ADDRESS addr, void * args)
{
	unsigned char regByte = Memory[addr + 1];
	ISA_Register regB = (ISA_Register)(regByte & 0xF);
	ISA_Register regA = (ISA_Register)((regByte >> 4) & 0xF);
	_commonRegs[regB] = _commonRegs[regA];
	return addr + 2;
}

ADDRESS V_CPU::MovQuadI2R(ADDRESS addr, void *)
{
	ISA_Register reg = (ISA_Register)Memory[addr + 1];
	memcpy(&_commonRegs[reg], &Memory[addr + 2], MAX_ADDRESS_BYTES);
	return addr + 2 + MAX_ADDRESS_BYTES;
}

ADDRESS V_CPU::MovQuadR2M(ADDRESS addr, void *)
{
	ISA_Register reg = (ISA_Register)Memory[addr + 1];
	ADDRESS target;
	memcpy(&target, &Memory[addr + 2], QUAD_BYTES);
	if (target >= MAX_MEMORY_ADDRESS || target == 0) {
		_state->_programState = PS_ADR;
		return addr;
	}
	memcpy(&Memory[target], &_commonRegs[reg], QUAD_BYTES);
	return addr + 2 + MAX_ADDRESS_BYTES;
}

uint64_t V_CPU::internal_pop()
{
	ADDRESS sstop = _commonRegs[RSP];
	if (sstop >= INIT_STACK_POS) {
		_state->_programState = PS_STACK_UNDERFLOW;
	}
	// addr + 1 就是哪个寄存器的字节
	uint64_t result;
	memcpy(&result, &Memory[sstop], MAX_ADDRESS_BYTES);
	_commonRegs[RSP] += MAX_ADDRESS_BYTES;
	return result;
}

void V_CPU::Init()
{
	memset(_commonRegs, 0, sizeof(_commonRegs));
	_commonRegs[RSP] = INIT_STACK_POS;
	_execFuncTable.insert(std::make_pair(SYSCALL, &V_CPU::SysCall));
	_execFuncTable.insert(std::make_pair(PUSHI, &V_CPU::PushI));
	_execFuncTable.insert(std::make_pair(PUSH, &V_CPU::Push));
	_execFuncTable.insert(std::make_pair(POP, &V_CPU::Pop));
	_execFuncTable.insert(std::make_pair(STOP, &V_CPU::Stop));
	_execFuncTable.insert(std::make_pair(NOP, &V_CPU::Nop));
	_execFuncTable.insert(std::make_pair(RRMOV, &V_CPU::MovQuadR2R));
	_execFuncTable.insert(std::make_pair(IRMOV, &V_CPU::MovQuadI2R));
	_execFuncTable.insert(std::make_pair(RMMOVQ, &V_CPU::MovQuadR2M));
}


