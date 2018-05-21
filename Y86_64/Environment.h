#ifndef Y86_64_ENVIRONMENT_H
#define Y86_64_ENVIRONMENT_H
#include <unordered_map>

#include "ISA.h"
#include "v_cpu.h"
#include "ExecutionState.h"

class Environment;
typedef void(Environment::*fpState)(void *);

class Environment
{
public:
	void stop_exec(void * args);
	void invalid_addr(void * args);
	void invalid_ins(void * args);
	void stack_overflow(void * args);
	void stack_underflow(void * args);
	Environment();
	~Environment();

	// 启动CPU
	void		Start();

	// 用于将代码加载的虚拟内存中
	void		Load(unsigned char * buffer, size_t num);

	void		SetMem(ADDRESS address, unsigned char * buffer, size_t num);

	// BEBUG，显示一段区域的内存
	void		DisplayMem(ADDRESS address, size_t num);
private:
	ExecutionState * _state;
	v_cpu *				_vCPU;

	std::unordered_map<ProgramState, fpState>			_globalStateTable;

	void init();
	void CheckState();

};
#endif

