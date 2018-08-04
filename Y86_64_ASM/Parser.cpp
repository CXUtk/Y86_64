#include "Parser.h"

uint64_t getImmdiate(const std::string& text) {
	const char * str = text.c_str();
	str++;
	return strtol(str, nullptr, 16);
}

ISA_Register getRegNum(const std::string& reg) {
	if (reg == "RAX")
		return RAX;
	else if (reg == "RBX")
		return RBX;
	else if (reg == "RCX")
		return RCX;
	else if (reg == "RDX")
		return RDX;
	else if (reg == "RBP")
		return RBP;
	else if (reg == "RSI")
		return RSI;
	else if (reg == "RSP")
		return RSP;
	else if (reg == "RDI")
		return RDI;
	else
		return NOREG;
}

void Parser::Run(const char * text) {
	_text = std::string(text);
	_code = NOP;
	memset(&_operand, 0, sizeof(Operand));
	int size = _text.size();
	int argindex = 0;
	bool symbol = false;
	for (int i = 0; i < size; i++) {
		if (_text[i] == ' ' || _text[i] == '\t')
			continue;
		int j = 0;
		while (i + j < size && _text[i + j] != ' ' && _text[i + j] != '\t') {
			j++;
		}
		std::string str = _text.substr(i, j);
		if (argindex == 0) {
			if (str[str.size() - 1] == ':') {
				symbol = true;
				break;
			}
			ISA_OPCode code = getCode(str);
			if (code != ERR) {
				_code = code;
			}
			else {
				fprintf(stderr, "%s: %s\n", text, "指令码错误！");
			}
		}
		else if (argindex == 1) {
			if(str[str.size() - 1] == ',')
				str = str.substr(0, str.size() - 1);
			parseOperand(str, true);
		}
		else {
			parseOperand(str, false);
		}
		argindex++;
		i += j;
	}
	if (!symbol) {
		auto code = getOpCode();
		auto operand = getOperand();
		_generator.AppendCode(code, &operand);
	}
	else {
		_symbolLookupTable[_text.substr(0, _text.size() - 1)] = _generator.getSize();
	}
}


Parser::Parser()
{
}

Parser::~Parser()
{
}

ISA_OPCode Parser::getCode(const std::string & str)
{
	if (str == "STOP")
		return STOP;
	else if (str == "NOP")
		return NOP;
	else if (str == "RRMOV")
		return RRMOV;
	else if (str == "IRMOVQ")
		return IRMOVQ;
	else if (str == "IRMOVD")
		return IRMOVD;
	else if (str == "IRMOVW")
		return IRMOVW;
	else if (str == "IRMOVB")
		return IRMOVB;
	else if (str == "RMMOVQ")
		return RMMOVQ;
	else if (str == "RMMOVD")
		return RMMOVD;
	else if (str == "RMMOVW")
		return RMMOVW;
	else if (str == "RMMOVB")
		return RMMOVB;
	else if (str == "MRMOVQ")
		return MRMOVQ;
	else if (str == "MRMOVD")
		return MRMOVD;
	else if (str == "MRMOVW")
		return MRMOVW;
	else if (str == "MRMOVB")
		return MRMOVB;
	else if (str == "ADD")
		return OP_ADD;
	else if (str == "SUB")
		return OP_SUB;
	else if (str == "MUL")
		return OP_MUL;
	else if (str == "DIV")
		return OP_DIV;
	else if (str == "AND")
		return OP_AND;
	else if (str == "OR")
		return OP_OR;
	else if (str == "XOR")
		return OP_XOR;
	else if (str == "SHR")
		return OP_SHR;
	else if (str == "SHL")
		return OP_SHL;
	else if (str == "CMP")
		return OP_CMP;
	else if (str == "TEST")
		return OP_TEST;
	else if (str == "NOT")
		return OP_NOT;
	else if (str == "INC")
		return OP_INC;
	else if (str == "DEC")
		return OP_DEC;
	else if (str == "NEG")
		return OP_NEG;
	else if (str == "PUSH")
		return PUSH;
	else if (str == "POP")
		return POP;
	else if (str == "JMP")
		return JMP;
	else if (str == "JLE")
		return JLE;
	else if (str == "JL")
		return JL;
	else if (str == "JE")
		return JE;
	else if (str == "JNE")
		return JNE;
	else if (str == "JGE")
		return JGE;
	else if (str == "JG")
		return JG;
	else if (str == "CALL")
		return CALL;
	else if (str == "POPN")
		return POPN;
	else if (str == "PUSHN")
		return PUSHN;
	else if (str == "PUSHI")
		return PUSHI;
	else if (str == "SYSCALL")
		return SYSCALL;
	else
		return ERR;
}

void Parser::parseOperand(const std::string & str, bool first)
{
	switch (_code)
	{
	case RRMOV: {
		if(first)
			_operand.regA = getRegNum(str);
		else
			_operand.regB = getRegNum(str);
		break;
	}
	case IRMOVQ:
	case IRMOVD:
	case IRMOVW:
	case IRMOVB: {
		if (first)
			_operand.value = getImmdiate(str);
		else
			_operand.regA = getRegNum(str);
		break;
	}
	case RMMOVQ:
	case RMMOVD:
	case RMMOVW:
	case RMMOVB: {
		//if (first)
		//	_operand.regA = getRegNum(str);
		//else
		//	XXXXXXXXX
		break;
	}
	case MRMOVQ:
	case MRMOVD:
	case MRMOVW:
	case MRMOVB: {
		break;
	}
	case OP_ADD:
	case OP_SUB:
	case OP_MUL:
	case OP_DIV:
	case OP_AND:
	case OP_OR:
	case OP_NOT:
	case OP_XOR:
	case OP_SHR:
	case OP_SHL:
	case OP_CMP:
	case OP_TEST:
	{
		if (first)
			_operand.regA = getRegNum(str);
		else
			_operand.regB = getRegNum(str);
		break;
	}
	case OP_INC:
	case OP_DEC:
	case OP_NEG: {
		_operand.regA = getRegNum(str);
		break;
	}
	case JMP:
	case JLE:
	case JL:
	case JE:
	case JNE:
	case JGE:
	case JG: {
		if (_symbolLookupTable.find(str) != _symbolLookupTable.end()) {
			auto pos = _symbolLookupTable[str];
			_operand.value = pos - _generator.getSize() - 9;
		}
		else {
			fprintf(stderr, "%s: %s\n", "未识别的标识符", str.c_str());
		}
		break;
	}
	case CALL: {
		break;
	}
	case PUSHI:
	case PUSHN:
	case POPN:
	{
		_operand.value = getImmdiate(str);
		break;
	}
	case PUSH: 
	case POP: {
		_operand.regA = getRegNum(str);
		break;
	}

	case SYSCALL: {
		_operand.regA = getImmdiate(str);
	}
		break;
	default:
		fprintf(stderr, "%s\n", "指令码后面不应该有参数");
		break;	
	}
}
