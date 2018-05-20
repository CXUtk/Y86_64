#include "stdafx.h"
#include "Debug.h"

Debug::Debug()
{
	_buffer.content = nullptr;
}

Debug::~Debug()
{
	for(auto buf : _codeBufferList)
	{
		delete[] buf.content;
	}
	delete[] _buffer.content;
}

void Debug::AppendCode(ISA_OPCode code, Operand * operand)
{
	Code_Buffer ret;
	if(code == IRMOVQ)
	{
		ret = register_resource(10);
		memcpy(ret.content, &code, 1);
		memcpy(ret.content + 1, &operand->regA, 1);
		memcpy(ret.content + 2, &operand->value, QUAD_BYTES);
	}
	else if (code == IRMOVD)
	{
		ret = register_resource(6);
		memcpy(ret.content, &code, 1);
		memcpy(ret.content + 1, &operand->regA, 1);
		memcpy(ret.content + 2, &operand->value, DWORD_BYTES);
	}
	else if (code == IRMOVW)
	{
		ret = register_resource(4);
		memcpy(ret.content, &code, 1);
		memcpy(ret.content + 1, &operand->regA, 1);
		memcpy(ret.content + 2, &operand->value, WORD_BYTES);
	}
	else if (code == IRMOVB)
	{
		ret = register_resource(3);
		memcpy(ret.content, &code, 1);
		memcpy(ret.content + 1, &operand->regA, 1);
		memcpy(ret.content + 2, &operand->value, SINGLE_BYTE);
	}
	else if(code == RRMOV)
	{
		ret = register_resource(2);
		unsigned char regByte = ((operand->regB & 0xF) << 4) | (operand->regA & 0xF);
		memcpy(ret.content, &code, 1);
		memcpy(ret.content + 1, &regByte, 1);
	}
	else if (code == RMMOVQ || code == RMMOVD || code == RMMOVW || code == RMMOVB
		|| code ==MRMOVQ || code == MRMOVD || code == MRMOVW || code == MRMOVB)
	{
		ret = register_resource(10);
		unsigned char regByte = ((operand->regB & 0xF) << 4) | (operand->regA & 0xF);
		memcpy(ret.content, &code, 1);
		memcpy(ret.content + 1, &regByte, 1);
		memcpy(ret.content + 2, &operand->value, 8);
	}
	else if ((code & 0xF0) == OP_ADD)
	{
		ret = register_resource(2);
		unsigned char regByte = ((operand->regB & 0xF) << 4) | (operand->regA & 0xF);
		memcpy(ret.content, &code, 1);
		memcpy(ret.content + 1, &regByte, 1);
	}
	else if (code == PUSH || code == POP)
	{
		ret = register_resource(2);
		memcpy(ret.content, &code, 1);
		memcpy(ret.content + 1, &operand->regA, 1);
	}
	else if (code == PUSHI)
	{
		ret = register_resource(9);
		memcpy(ret.content, &code, 1);
		memcpy(ret.content + 1, &operand->value, 8);
	}
	else if (code == SYSCALL)
	{
		ret = register_resource(2);
		memcpy(ret.content, &code, 1);
		memcpy(ret.content + 1, &operand->regA, 2);
	}	
	_codeBufferList.push_back(ret);
}

Code_Buffer Debug::GenerateCode()
{
	size_t totalSize = 0;
	size_t pos = 0;
	for(auto buf : _codeBufferList)
	{
		totalSize += buf.size;
	}
	_buffer.size = totalSize;
	if (_buffer.content) delete[] _buffer.content;
	_buffer.content = new unsigned char[totalSize];
	for (auto buf : _codeBufferList)
	{
		memcpy(_buffer.content + pos, buf.content, buf.size);
		pos += buf.size;
	}
	return _buffer;
}

Code_Buffer Debug::register_resource(size_t size)
{
	Code_Buffer buffer;
	unsigned char * ret = new unsigned char[size];
	buffer.size = size;
	buffer.content = ret;
	return buffer;
}

