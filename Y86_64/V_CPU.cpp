#include "stdafx.h"
#include "V_CPU.h"


v_cpu::v_cpu()
{
	init();
}

v_cpu::v_cpu(ExecutionState * state)
{
	_state = state;
	init();
}

v_cpu::~v_cpu()
{
}

ADDRESS v_cpu::Execute()
{
	const ADDRESS pc = _state->_programCounter;
	const auto code = static_cast<ISA_OPCode>(Memory[pc]);

	if (_execFuncTable.find(code) != _execFuncTable.end() && _execFuncTable[code]) {
		ADDRESS addr = (this->*_execFuncTable[code])(pc, nullptr);
		return addr;
	}
	else {
		_state->_programState = PS_INS;
		return pc;
	}

}

bool v_cpu::get_condition_bit(ConditionBit bit)
{
	return (_state->_conditionCode >> bit) & 1;
}

ADDRESS v_cpu::Stop(ADDRESS, void * args)
{
	_state->_programState = PS_HALT;
	return _state->_programCounter + 1;
}

ADDRESS v_cpu::SysCall(ADDRESS addr, void * args)
{
	const auto type = static_cast<SysCall_Type>(Memory[addr + 1]);
	if (type == Sys_TestFunc) {
		fprintf(stdout, "%s\n", "系统调用#0，测试环境 正常");
	}
	if (type == Sys_PrintA) {
		ADDRESS target = internal_pop();
		// TODO：检测是否越界
		fprintf(stdout, "%s\n", reinterpret_cast<char *>(&Memory[target]));
	}
	return addr + 2;
}

ADDRESS v_cpu::PushI(ADDRESS addr, void * args)
{
	// 将立即数[addr + 1]推进栈中
	_commonRegs[RSP] -= sizeof(uint64_t);
	const ADDRESS sstop = _commonRegs[RSP];
	if (sstop < INIT_STACK_POS - MAX_STACK_GROW) {
		_state->_programState = PS_STACK_OVERFLOW;
		return addr;
	}
	memcpy(&Memory[sstop], &Memory[addr + 1], MAX_ADDRESS_BYTES);
	return addr + 1 + MAX_ADDRESS_BYTES;
}

ADDRESS v_cpu::Push(ADDRESS addr, void *)
{
	// [addr + 1]的值是寄存器编号
	const auto reg = static_cast<ISA_Register>(Memory[addr + 1]);
	_commonRegs[RSP] -= MAX_ADDRESS_BYTES;
	const ADDRESS sstop = _commonRegs[RSP];
	if (sstop < INIT_STACK_POS - MAX_STACK_GROW) {
		_state->_programState = PS_STACK_OVERFLOW;
		return addr;
	}
	memcpy(&Memory[sstop], &_commonRegs[reg], MAX_ADDRESS_BYTES);
	return addr + 2;
}

ADDRESS v_cpu::Pop(ADDRESS addr, void * args)
{
	const ADDRESS sstop = _commonRegs[RSP];
	if (sstop >= INIT_STACK_POS) {
		_state->_programState = PS_STACK_UNDERFLOW;
		return addr;
	}
	// addr + 1 就是哪个寄存器的字节
	const auto target = static_cast<ISA_Register>(Memory[addr + 1]);
	memcpy(&_commonRegs[target], &Memory[sstop], MAX_ADDRESS_BYTES);
	_commonRegs[RSP] += MAX_ADDRESS_BYTES;
	return addr + 2;
}

ADDRESS v_cpu::Nop(ADDRESS addr, void * args)
{
	return addr + 1;
}

ADDRESS v_cpu::MovQuadR2R(ADDRESS addr, void * args)
{
	const unsigned char regByte = Memory[addr + 1];
	auto regB = static_cast<ISA_Register>(regByte & 0xF);
	auto regA = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	_commonRegs[regB] = _commonRegs[regA];
	return addr + 2;
}

ADDRESS v_cpu::MovQuadI2R(ADDRESS addr, void *)
{
	return _internalI2R(addr, QUAD_BYTES);
}

ADDRESS v_cpu::MovDoubI2R(ADDRESS addr, void *)
{
	return _internalI2R(addr, DWORD_BYTES);
}

ADDRESS v_cpu::MovWordI2R(ADDRESS addr, void *)
{
	return _internalI2R(addr, WORD_BYTES);
}

ADDRESS v_cpu::MovByteI2R(ADDRESS addr, void *)
{
	return _internalI2R(addr, SINGLE_BYTE);
}

ADDRESS v_cpu::MovQuadR2M(ADDRESS addr, void *)
{
	return _internalR2M(addr, QUAD_BYTES);
}

ADDRESS v_cpu::MovDoubR2M(ADDRESS addr, void *)
{
	return _internalR2M(addr, DWORD_BYTES);
}

ADDRESS v_cpu::MovWordR2M(ADDRESS addr, void *)
{
	return _internalR2M(addr, WORD_BYTES);
}

ADDRESS v_cpu::MovByteR2M(ADDRESS addr, void *)
{
	return _internalR2M(addr, SINGLE_BYTE);
}

ADDRESS v_cpu::MovQuadM2R(ADDRESS addr, void *)
{
	return _internalM2R(addr, QUAD_BYTES);
}

ADDRESS v_cpu::MovDoubM2R(ADDRESS addr, void *)
{
	return _internalM2R(addr, DWORD_BYTES);
}

ADDRESS v_cpu::MovWordM2R(ADDRESS addr, void *)
{
	return _internalM2R(addr, WORD_BYTES);
}

ADDRESS v_cpu::MovByteM2R(ADDRESS addr, void *)
{
	return _internalM2R(addr, SINGLE_BYTE);
}

ADDRESS v_cpu::Op_add(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte & 0xF);
	auto reg_b = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	const Reg_Value prevB = _commonRegs[reg_b];
	const Reg_Value prevA = _commonRegs[reg_a];
	_commonRegs[reg_b] = prevB + prevA;
	set_flags(_commonRegs[reg_b]);
	set_condition_bit(CC_OVERFLOW, (prevA < 0 == prevB < 0) && (_commonRegs[reg_b] < 0 != prevA < 0));
	return addr + 2;
}

ADDRESS v_cpu::Op_sub(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte & 0xF);
	auto reg_b = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	const Reg_Value prevB = _commonRegs[reg_b];
	const Reg_Value prevA = _commonRegs[reg_a];
	_commonRegs[reg_b] = prevB - prevA;
	set_flags(_commonRegs[reg_b]);
	set_condition_bit(CC_OVERFLOW, (prevB < 0 != prevA < 0) && (_commonRegs[reg_b] < 0 != prevB < 0));
	return addr + 2;
}

uint64_t v_cpu::internal_pop()
{
	const ADDRESS sstop = _commonRegs[RSP];
	if (sstop >= INIT_STACK_POS) {
		_state->_programState = PS_STACK_UNDERFLOW;
	}
	// addr + 1 就是哪个寄存器的字节
	ADDRESS result;
	memcpy(&result, &Memory[sstop], MAX_ADDRESS_BYTES);
	_commonRegs[RSP] += MAX_ADDRESS_BYTES;
	return result;
}

void v_cpu::init()
{
	memset(_commonRegs, 0, sizeof(_commonRegs));
	_commonRegs[RSP] = INIT_STACK_POS;
	_execFuncTable.insert(std::make_pair(SYSCALL, &v_cpu::SysCall));
	_execFuncTable.insert(std::make_pair(PUSHI, &v_cpu::PushI));
	_execFuncTable.insert(std::make_pair(PUSH, &v_cpu::Push));
	_execFuncTable.insert(std::make_pair(POP, &v_cpu::Pop));
	_execFuncTable.insert(std::make_pair(STOP, &v_cpu::Stop));
	_execFuncTable.insert(std::make_pair(NOP, &v_cpu::Nop));
	_execFuncTable.insert(std::make_pair(RRMOV, &v_cpu::MovQuadR2R));
	_execFuncTable.insert(std::make_pair(IRMOVQ, &v_cpu::MovQuadI2R));
	_execFuncTable.insert(std::make_pair(IRMOVD, &v_cpu::MovDoubI2R));
	_execFuncTable.insert(std::make_pair(IRMOVW, &v_cpu::MovWordI2R));
	_execFuncTable.insert(std::make_pair(IRMOVB, &v_cpu::MovByteI2R));
	_execFuncTable.insert(std::make_pair(RMMOVQ, &v_cpu::MovQuadR2M));
	_execFuncTable.insert(std::make_pair(RMMOVD, &v_cpu::MovDoubR2M));
	_execFuncTable.insert(std::make_pair(RMMOVW, &v_cpu::MovWordR2M));
	_execFuncTable.insert(std::make_pair(RMMOVB, &v_cpu::MovByteR2M));
	_execFuncTable.insert(std::make_pair(MRMOVQ, &v_cpu::MovQuadM2R));
	_execFuncTable.insert(std::make_pair(MRMOVD, &v_cpu::MovDoubM2R));
	_execFuncTable.insert(std::make_pair(MRMOVW, &v_cpu::MovWordM2R));
	_execFuncTable.insert(std::make_pair(MRMOVB, &v_cpu::MovByteM2R));
	_execFuncTable.insert(std::make_pair(OP_ADD, &v_cpu::Op_add));
	_execFuncTable.insert(std::make_pair(OP_SUB, &v_cpu::Op_sub));

}

void v_cpu::set_condition_bit(ConditionBit bit, bool value)
{
	if (value) {
		const uint64_t mask = 1 << bit;
		_state->_conditionCode |= mask;
	}
	else
	{
		const uint64_t mask = ~(1 << bit);
		_state->_conditionCode &= mask;
	}
}

void v_cpu::set_flags(Reg_Value reg)
{
	set_condition_bit(CC_ZERO, reg == 0);
	set_condition_bit(CC_SIGN, reg < 0);
}


ADDRESS v_cpu::_internalR2M(ADDRESS addr, size_t size)
{
	// 将寄存器RA中的值放进寄存器RB的值所代表地址+val的内存中
	// rmmovX ra, D(rb)
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte & 0xF);
	auto reg_b = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	ADDRESS val;
	memcpy(&val, &Memory[addr + 2], MAX_ADDRESS_BYTES);
	const ADDRESS target = val + _commonRegs[reg_b];
	if (target >= MAX_MEMORY_ADDRESS || target == 0) {
		_state->_programState = PS_ADR;
		return addr;
	}
	memcpy(&Memory[target], &_commonRegs[reg_a], size);
	return addr + 2 + MAX_ADDRESS_BYTES;
}

ADDRESS v_cpu::_internalM2R(ADDRESS addr, size_t size)
{
	// 将寄存器RB的值所代表地址+val的内存中的数据 放进寄存器RA中
	// mrmovX D(rb), ra
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte & 0xF);
	auto reg_b = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	ADDRESS val;
	memcpy(&val, &Memory[addr + 2], MAX_ADDRESS_BYTES);
	const ADDRESS target = val + _commonRegs[reg_b];
	if (target >= MAX_MEMORY_ADDRESS || target == 0) {
		_state->_programState = PS_ADR;
		return addr;
	}
	// 当前启用零扩展，但是实际上要考虑是零扩展还是符号扩展
	// TODO: 增加符号扩展指令 对于DWB
	int64_t tmp = 0;
	memcpy(&tmp, &Memory[target], size);
	_commonRegs[reg_a] = tmp;
	return addr + 2 + MAX_ADDRESS_BYTES;
}

ADDRESS v_cpu::_internalI2R(ADDRESS addr, size_t size)
{
	const auto reg = static_cast<ISA_Register>(Memory[addr + 1]);
	memcpy(&_commonRegs[reg], &Memory[addr + 2], size);
	return addr + 2 + size;
}


