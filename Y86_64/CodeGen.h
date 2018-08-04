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

class CodeGen
{
public:
	CodeGen();
	~CodeGen();
	// ��Debug�����������ָ��
	void AppendCode(ISA_OPCode code, Operand * operand);

	// ��Debug�������а�ָ�����ȫ����ȡ����
	Code_Buffer GenerateCode();
	int getSize() { 
		int i = 0;
		for (auto buf : _codeBufferList) {
			i += buf.size;
		}
		return i;
	}

private:
	std::vector<Code_Buffer> _codeBufferList;
	Code_Buffer _buffer;

	Code_Buffer alloc_resource(size_t size);
};

#endif // !Y86_64_DEBUG_H

