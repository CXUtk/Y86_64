#ifndef Y86_64_VCPU_H
#define Y86_64_VCPU_H

#include "ISA.h"
#include "ExecutionState.h"

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

	void SysCall()
};
#endif
