#ifndef Y86_64_ENVIRONMENT_H
#define Y86_64_ENVIRONMENT_H
#include <unordered_map>

#include "ISA.h"
#include "v_cpu.h"
#include "ExecutionState.h"

class Environment
{
public:
	Environment();
	~Environment();

	// ����CPU
	void		Start();

	// ���ڽ�������ص������ڴ���
	void		Load(unsigned char * buffer, size_t num);

	void		SetMem(ADDRESS address, unsigned char * buffer, size_t num);

	// BEBUG����ʾһ��������ڴ�
	void		DisplayMem(ADDRESS address, size_t num);
private:
	ExecutionState * _state;
	v_cpu *				_vCPU;

	std::unordered_map<ProgramState, fpState>			_globalStateTable;

	void init();
	void CheckState();

};
#endif

