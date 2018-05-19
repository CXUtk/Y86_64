#ifndef Y86_64_VCPU_H
#define Y86_64_VCPU_H
#include <unordered_map>

#include "ISA.h"
#include "ExecutionState.h"

class V_CPU;

// �����ִ�к�����������ȡ�����룬ִ�н׶Σ�����ָ��ָ��Ĵ���RIP���µ�ַ
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
