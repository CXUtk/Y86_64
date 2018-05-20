#ifndef Y86_64_VCPU_H
#define Y86_64_VCPU_H
#include <unordered_map>

#include "ISA.h"
#include "ExecutionState.h"

class v_cpu;

// 代码的执行函数，包含读取，译码，执行阶段，返回指令指针寄存器RIP的新地址
typedef ADDRESS(v_cpu::*Exex_func)(ADDRESS, void *);

class v_cpu
{
public:
	v_cpu();
	v_cpu(ExecutionState * state);
	~v_cpu();

	ADDRESS Execute();
	bool get_condition_bit(ConditionBit bit);
private:
	Reg_Value			_commonRegs[NUM_OF_REGISTER];
	ExecutionState *	_state;

	std::unordered_map<ISA_OPCode, Exex_func>		_execFuncTable;

	ADDRESS Stop(ADDRESS, void *);
	ADDRESS SysCall(ADDRESS, void *);
	ADDRESS PushI(ADDRESS, void *);
	ADDRESS Push(ADDRESS, void *);
	ADDRESS Pop(ADDRESS, void *);
	ADDRESS Nop(ADDRESS, void *);
	ADDRESS MovQuadR2R(ADDRESS, void *);
	ADDRESS MovQuadI2R(ADDRESS, void *);
	ADDRESS MovQuadR2M(ADDRESS, void *);
	ADDRESS MovDoubR2M(ADDRESS, void *);
	ADDRESS MovWordR2M(ADDRESS, void *);
	ADDRESS MovByteR2M(ADDRESS, void *);
	ADDRESS MovQuadM2R(ADDRESS, void *);
	ADDRESS MovDoubM2R(ADDRESS, void *);
	ADDRESS MovWordM2R(ADDRESS, void *);
	ADDRESS MovByteM2R(ADDRESS, void *);
	ADDRESS Op_add(ADDRESS, void *);
	ADDRESS Op_sub(ADDRESS, void *);
	uint64_t internal_pop();

	void init();
	void set_condition_bit(ConditionBit bit, bool value);
	void set_flags(Reg_Value reg);
	ADDRESS _internalR2M(ADDRESS addr, size_t size);
	ADDRESS _internalM2R(ADDRESS addr, size_t size);

};
#endif
