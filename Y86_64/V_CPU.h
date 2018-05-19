#ifndef Y86_64_VCPU_H
#define Y86_64_VCPU_H
#include <unordered_map>

#include "ISA.h"
#include "ExecutionState.h"

class V_CPU;

// 代码的执行函数，包含读取，译码，执行阶段，返回指令指针寄存器RIP的新地址
typedef ADDRESS(V_CPU::*Exex_func)(ADDRESS, void *);

class V_CPU
{
public:
	V_CPU();
	V_CPU(ExecutionState * state);
	~V_CPU();

	ADDRESS Execute();
private:
	Register			_commonRegs[NUM_OF_REGISTER];
	ExecutionState *	_state;

	std::unordered_map<ISA_ByteCode, Exex_func>		_execFuncTable;

	ADDRESS SysCall(ADDRESS, void *);
	ADDRESS PushI(ADDRESS, void *);
	ADDRESS Pop(ADDRESS, void *);
	uint64_t internal_pop();

	void Init();
};
#endif
