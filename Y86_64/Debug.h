#ifndef Y86_64_DEBUG_H
#define Y86_64_DEBUG_H
#include "ISA.h"
#include <vector>

typedef struct {
	unsigned char regA;
	unsigned char regB;
	uint64_t value;
} Operand;

typedef struct
{
	size_t size;
	unsigned char * content;
} Code_Buffer;

class Debug
{
public:
	Debug();
	~Debug();
	// ��Debug�����������ָ��
	void AppendCode(ISA_OPCode code, Operand * operand);

	// ��Debug�������а�ָ�����ȫ����ȡ����
	Code_Buffer GenerateCode();

private:
	std::vector<Code_Buffer> _codeBufferList;
	Code_Buffer _buffer;

	Code_Buffer register_resource(size_t size);
};

#endif // !Y86_64_DEBUG_H

