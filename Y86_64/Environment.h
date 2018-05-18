#ifndef Y86_64_ENVIRONMENT_H
#define Y86_64_ENVIRONMENT_H
#include <stdbool.h>

#include "ISA.h"
#include "V_CPU.h"
#include "ExecutionState.h"

class Environment
{
public:
	Environment();
	~Environment();

	int64_t		GetData(ADDRESS address);
	void		SetData(ADDRESS address, int64_t value);
	void		StopExec();
	void		Start();
	void		Load(char * buffer, size_t num);
private:
	ExecutionState *	_state;
	V_CPU *				_vCPU;

	void init();
};
#endif

