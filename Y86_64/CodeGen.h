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
	// 向Debug缓冲区中添加指令
	void AppendCode(ISA_OPCode code, Operand * operand);

	// 从Debug缓冲区中把指令代码全部提取出来
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

