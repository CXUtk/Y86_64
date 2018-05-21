#include "stdafx.h"
#include <ctime>
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

void v_cpu::dump_regs()
{
	for (int reg = RAX; reg < NOREG; reg++)
	{
		char name[4];
		if (reg == RAX)
			strcpy_s(name, "RAX");
		else if (reg == RBX)
			strcpy_s(name, "RBX");
		else if (reg == RCX)
			strcpy_s(name, "RCX");
		else if (reg == RDX)
			strcpy_s(name, "RDX");
		else if (reg == RBP)
			strcpy_s(name, "RBX");
		else if (reg == RSP)
			strcpy_s(name, "RSP");
		else if (reg == RBP)
			strcpy_s(name, "RBP");
		else if (reg == RSI)
			strcpy_s(name, "RSI");
		else if (reg == RDI)
			strcpy_s(name, "RDI");

		fprintf(stdout, "%s: 0x%.8X\n", name, _commonRegs[reg]);
	}
	fprintf(stdout, "%s: 0x%.8X\n", "RIP", _state->_programCounter);
	fprintf(stdout, "%s: 0x%.8X\n", " PS", _state->_programState);
}

ADDRESS v_cpu::Stop(ADDRESS, void * args)
{
	_state->_programState = PS_HALT;
	return _state->_programCounter + 1;
}

ADDRESS v_cpu::SysCall(ADDRESS addr, void * args)
{
	const auto type = static_cast<SysCall_Type>(Memory[addr + 1]);
	switch (type)
	{
	case Sys_TestFunc:
	{
		fprintf(stdout, "%s\n", "系统调用#0，测试环境 正常");
		break;
	}
	case Sys_PrintA:
	{
		const ADDRESS target = internal_pop();
		// 检测是否越界
		if (target >= MAX_MEMORY_ADDRESS || target == 0) {
			_state->_programState = PS_ADR;
			return addr;
		}
		fprintf(stdout, "%s\n", &Memory[target]);
		break;
	}
	case Sys_PrintDQ:
	{
		const ADDRESS target = internal_pop();
		// 检测是否越界
		if (target >= MAX_MEMORY_ADDRESS || target == 0) {
			_state->_programState = PS_ADR;
			return addr;
		}
		int64_t value;
		memcpy(&value, &Memory[target], QUAD_BYTES);
		fprintf(stdout, "%lld\n", value);
		break;
	}
	case Sys_Time:
	{
		_commonRegs[RAX] = time(nullptr);
		break;
	}
	default:
	{
		_state->_programState = PS_INS;
		return addr;
	}
	}
	return addr + 2;
}

ADDRESS v_cpu::PushI(ADDRESS addr, void * args)
{
	// 将立即数[addr + 1]推进栈中
	_commonRegs[RSP] -= sizeof(uint64_t);
	const ADDRESS sstop = _commonRegs[RSP];
	if (sstop < INIT_STACK_POS - MAX_STACK_SIZE) {
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
	if (sstop < INIT_STACK_POS - MAX_STACK_SIZE) {
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

ADDRESS v_cpu::Op_mul(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte & 0xF);
	auto reg_b = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	const Reg_Value prevB = _commonRegs[reg_b];
	const Reg_Value prevA = _commonRegs[reg_a];
	_commonRegs[reg_b] = prevB * prevA;
	set_flags(_commonRegs[reg_b]);

	// 乘法除法不设置溢出符号
	return addr + 2;
}

ADDRESS v_cpu::Op_div(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte & 0xF);
	auto reg_b = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	const Reg_Value prevB = _commonRegs[reg_b];
	const Reg_Value prevA = _commonRegs[reg_a];
	if (prevA == 0)
	{
		_state->_programState = PS_DIV_BY_ZERO;
		return addr;
	}
	_commonRegs[reg_b] = prevB / prevA;
	set_flags(_commonRegs[reg_b]);

	// 乘法除法不设置溢出符号
	return addr + 2;
}

ADDRESS v_cpu::Op_and(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte & 0xF);
	auto reg_b = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	const Reg_Value prevB = _commonRegs[reg_b];
	const Reg_Value prevA = _commonRegs[reg_a];
	_commonRegs[reg_b] = prevB & prevA;
	set_flags(_commonRegs[reg_b]);
	return addr + 2;
}

ADDRESS v_cpu::Op_or(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte & 0xF);
	auto reg_b = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	const Reg_Value prevB = _commonRegs[reg_b];
	const Reg_Value prevA = _commonRegs[reg_a];
	_commonRegs[reg_b] = prevB | prevA;
	set_flags(_commonRegs[reg_b]);
	return addr + 2;
}

ADDRESS v_cpu::Op_not(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte);
	_commonRegs[reg_a] = ~_commonRegs[reg_a];
	set_flags(_commonRegs[reg_a]);
	return addr + 2;
}

ADDRESS v_cpu::Op_xor(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte & 0xF);
	auto reg_b = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	const Reg_Value prevB = _commonRegs[reg_b];
	const Reg_Value prevA = _commonRegs[reg_a];
	_commonRegs[reg_b] = prevB ^ prevA;
	set_flags(_commonRegs[reg_b]);
	return addr + 2;
}

ADDRESS v_cpu::Op_shr(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte & 0xF);
	auto reg_b = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	const Reg_Value prevB = _commonRegs[reg_b];
	const Reg_Value prevA = _commonRegs[reg_a];
	_commonRegs[reg_b] = prevB >> prevA;
	set_flags(_commonRegs[reg_b]);
	return addr + 2;
}

ADDRESS v_cpu::Op_shl(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte & 0xF);
	auto reg_b = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	const Reg_Value prevB = _commonRegs[reg_b];
	const Reg_Value prevA = _commonRegs[reg_a];
	_commonRegs[reg_b] = prevB << prevA;
	set_flags(_commonRegs[reg_b]);
	return addr + 2;
}

ADDRESS v_cpu::Op_inc(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte);
	_commonRegs[reg_a]++;
	set_flags(_commonRegs[reg_a]);
	return addr + 2;
}

ADDRESS v_cpu::Op_dec(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte);
	_commonRegs[reg_a]--;
	set_flags(_commonRegs[reg_a]);
	return addr + 2;
}

ADDRESS v_cpu::Op_neg(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte);
	_commonRegs[reg_a] = -_commonRegs[reg_a];
	set_flags(_commonRegs[reg_a]);
	return addr + 2;
}

ADDRESS v_cpu::Op_cmp(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte & 0xF);
	auto reg_b = static_cast<ISA_Register>((regByte >> 4) & 0xF);
	const Reg_Value prevB = _commonRegs[reg_b];
	const Reg_Value prevA = _commonRegs[reg_a];
	set_flags(prevB - prevA);
	set_condition_bit(CC_OVERFLOW, (prevB < 0 != prevA < 0) && (_commonRegs[reg_b] < 0 != prevB < 0));
	return addr + 2;
}

ADDRESS v_cpu::Op_test(ADDRESS addr, void *)
{
	const unsigned char regByte = Memory[addr + 1];
	auto reg_a = static_cast<ISA_Register>(regByte);
	const Reg_Value prevA = _commonRegs[reg_a];
	set_flags(prevA);
	return addr + 2;
}

ADDRESS v_cpu::Jmp(ADDRESS addr, void *)
{
	return _internalJMP(addr);

}

ADDRESS v_cpu::Jle(ADDRESS addr, void *)
{
	const bool SF = get_condition_bit(CC_SIGN);
	const bool OF = get_condition_bit(CC_OVERFLOW);
	const bool ZF = get_condition_bit(CC_ZERO);
	if ((SF ^ OF) | ZF)
	{
		return _internalJMP(addr);
	}
	return addr + 1 + MAX_ADDRESS_BYTES;
}

ADDRESS v_cpu::Jl(ADDRESS addr, void *)
{
	const bool SF = get_condition_bit(CC_SIGN);
	const bool OF = get_condition_bit(CC_OVERFLOW);
	if (SF ^ OF)
	{
		return _internalJMP(addr);
	}
	return addr + 1 + MAX_ADDRESS_BYTES;
}

ADDRESS v_cpu::Je(ADDRESS addr, void *)
{
	const bool ZF = get_condition_bit(CC_ZERO);
	if (ZF)
	{
		return _internalJMP(addr);
	}
	return addr + 1 + MAX_ADDRESS_BYTES;
}

ADDRESS v_cpu::Jne(ADDRESS addr, void *)
{
	const bool ZF = get_condition_bit(CC_ZERO);
	if (!ZF)
	{
		return _internalJMP(addr);
	}
	return addr + 1 + MAX_ADDRESS_BYTES;
}

ADDRESS v_cpu::Jge(ADDRESS addr, void *)
{
	const bool SF = get_condition_bit(CC_SIGN);
	const bool OF = get_condition_bit(CC_OVERFLOW);
	if (!(SF ^ OF))
	{
		return _internalJMP(addr);
	}
	return addr + 1 + MAX_ADDRESS_BYTES;
}

ADDRESS v_cpu::Jg(ADDRESS addr, void *)
{
	const bool SF = get_condition_bit(CC_SIGN);
	const bool OF = get_condition_bit(CC_OVERFLOW);
	const bool ZF = get_condition_bit(CC_ZERO);
	if (!(SF ^ OF) & !ZF)
	{
		return _internalJMP(addr);
	}
	return addr + 1 + MAX_ADDRESS_BYTES;
}

ADDRESS v_cpu::Call(ADDRESS addr, void *)
{
	ADDRESS target;
	memcpy(&target, &Memory[addr + 1], MAX_ADDRESS_BYTES);
	if (target >= MAX_MEMORY_ADDRESS || target == 0) {
		_state->_programState = PS_ADR;
		return addr;
	}
	internal_push(addr + 1 + 8);
	// 预留8字节栈空间
	internal_push(0);
	return target;
}

ADDRESS v_cpu::Ret(ADDRESS addr, void *)
{
	// 弹出预留栈空间
	internal_pop();

	// 真实跳转地址
	const ADDRESS target = internal_pop();
	if (target >= MAX_MEMORY_ADDRESS || target == 0) {
		_state->_programState = PS_ADR;
		return addr;
	}
	return target;
}

uint64_t v_cpu::internal_pop()
{
	const ADDRESS sstop = _commonRegs[RSP];
	// 栈溢出检测
	if (sstop >= INIT_STACK_POS) {
		_state->_programState = PS_STACK_UNDERFLOW;
	}
	// addr + 1 就是哪个寄存器的字节
	ADDRESS result;
	memcpy(&result, &Memory[sstop], MAX_ADDRESS_BYTES);
	_commonRegs[RSP] += MAX_ADDRESS_BYTES;
	return result;
}

void v_cpu::internal_push(uint64_t value)
{
	_commonRegs[RSP] -= sizeof(uint64_t);
	const ADDRESS sstop = _commonRegs[RSP];
	// 栈溢出检测
	if (sstop < INIT_STACK_POS - MAX_STACK_SIZE) {
		_state->_programState = PS_STACK_OVERFLOW;
		return;
	}
	memcpy(&Memory[sstop], &value, MAX_ADDRESS_BYTES);
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
	_execFuncTable.insert(std::make_pair(OP_MUL, &v_cpu::Op_mul));
	_execFuncTable.insert(std::make_pair(OP_DIV, &v_cpu::Op_div));
	_execFuncTable.insert(std::make_pair(OP_AND, &v_cpu::Op_and));
	_execFuncTable.insert(std::make_pair(OP_OR, &v_cpu::Op_or));
	_execFuncTable.insert(std::make_pair(OP_NOT, &v_cpu::Op_not));
	_execFuncTable.insert(std::make_pair(OP_XOR, &v_cpu::Op_xor));
	_execFuncTable.insert(std::make_pair(OP_SHL, &v_cpu::Op_shl));
	_execFuncTable.insert(std::make_pair(OP_SHR, &v_cpu::Op_shr));
	_execFuncTable.insert(std::make_pair(OP_INC, &v_cpu::Op_inc));
	_execFuncTable.insert(std::make_pair(OP_DEC, &v_cpu::Op_dec));
	_execFuncTable.insert(std::make_pair(OP_NEG, &v_cpu::Op_neg));
	_execFuncTable.insert(std::make_pair(OP_CMP, &v_cpu::Op_cmp));
	_execFuncTable.insert(std::make_pair(OP_TEST, &v_cpu::Op_test));
	_execFuncTable.insert(std::make_pair(JMP, &v_cpu::Jmp));
	_execFuncTable.insert(std::make_pair(JL, &v_cpu::Jl));
	_execFuncTable.insert(std::make_pair(JLE, &v_cpu::Jle));
	_execFuncTable.insert(std::make_pair(JE, &v_cpu::Je));
	_execFuncTable.insert(std::make_pair(JNE, &v_cpu::Jne));
	_execFuncTable.insert(std::make_pair(JG, &v_cpu::Jg));
	_execFuncTable.insert(std::make_pair(JGE, &v_cpu::Jge));
	_execFuncTable.insert(std::make_pair(CALL, &v_cpu::Call));
	_execFuncTable.insert(std::make_pair(RET, &v_cpu::Ret));
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
	set_condition_bit(CC_CARRY, false);
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

ADDRESS v_cpu::_internalJMP(ADDRESS addr)
{
	int64_t offset;
	memcpy(&offset, &Memory[addr + 1], MAX_ADDRESS_BYTES);
	if (_state->_programCounter + offset >= MAX_MEMORY_ADDRESS || _state->_programCounter + offset == 0) {
		_state->_programState = PS_ADR;
		return addr;
	}
	return addr + 1 + 8 + offset;
}


