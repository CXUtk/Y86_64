#ifndef Y86_64_ENVIRONMENT_H
#define Y86_64_ENVIRONMENT_H
#include <stdbool.h>
#include <unordered_map>

#include "ISA.h"
#include "V_CPU.h"
#include "ExecutionState.h"

class Environment
{
public:
	Environment();
	~Environment();
	
	// 启动CPU
	void		Start();

	// 用于将代码加载的虚拟内存中
	void		Load(unsigned char * buffer, size_t num);

	void		SetMem(ADDRESS address, unsigned char * buffer, size_t num);
	void		DisplayMem(ADDRESS address, size_t num);
private:
	ExecutionState *	_state;
	V_CPU *				_vCPU;

	std::unordered_map<ProgramStatus, fpState>			_globalStateTable;

	void init();
	void CheckState();

};
#endif

