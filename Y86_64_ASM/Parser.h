#pragma once
#include <string>
#include <map>
#include "../Y86_64/ISA.h"
#include "../Y86_64/CodeGen.h"
class Parser
{
public:
	Parser();
	~Parser();

	void Run(const char * text);

	ISA_OPCode getOpCode() { return _code; }
	Operand getOperand() { return _operand; }
	Code_Buffer getCode() {
		return _generator.GenerateCode();
	}

private:
	std::string _text;
	ISA_OPCode _code;
	Operand _operand;
	std::map<std::string, ADDRESS> _symbolLookupTable;
	CodeGen _generator;

	ISA_OPCode getCode(const std::string& str);
	void parseOperand(const std::string& str, bool first);
};

