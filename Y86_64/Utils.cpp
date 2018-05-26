#include "stdafx.h"
#include "Utils.h"
#include <vector>

void get_reg_name(char name[4], ISA_Register reg)
{
	if (reg == RAX)
		strcpy_s(name, 4, "RAX");
	else if (reg == RBX)
		strcpy_s(name, 4, "RBX");
	else if (reg == RCX)
		strcpy_s(name, 4, "RCX");
	else if (reg == RDX)
		strcpy_s(name, 4, "RDX");
	else if (reg == RSP)
		strcpy_s(name, 4, "RSP");
	else if (reg == RBP)
		strcpy_s(name, 4, "RBP");
	else if (reg == RSI)
		strcpy_s(name, 4, "RSI");
	else if (reg == RDI)
		strcpy_s(name, 4, "RDI");
	else
		strcpy_s(name, 4, "XXX");
}

void get_ins_name(char name[8], ISA_OPCode code)
{
	switch (code)
	{
	case STOP:
		strcpy_s(name, 8, "STOP");
		break;
	case NOP:
		strcpy_s(name, 8, "NOP");
		break;
	case RRMOV:
		strcpy_s(name, 8, "RRMOV");
		break;
	case IRMOVQ:
		strcpy_s(name, 8, "IRMOVQ");
		break;
	case IRMOVD:
		strcpy_s(name, 8, "IRMOVD");
		break;
	case IRMOVW:
		strcpy_s(name, 8, "IRMOVW");
		break;
	case IRMOVB:
		strcpy_s(name, 8, "IRMOVB");
		break;
	case RMMOVQ:
		strcpy_s(name, 8, "RRMOVQ");
		break;
	case RMMOVD:
		strcpy_s(name, 8, "RRMOVD");
		break;
	case RMMOVW:
		strcpy_s(name, 8, "RRMOVW");
		break;
	case RMMOVB:
		strcpy_s(name, 8, "RRMOVB");
		break;
	case MRMOVQ:
		strcpy_s(name, 8, "MRMOVQ");
		break;
	case MRMOVD:
		strcpy_s(name, 8, "MRMOVD");
		break;
	case MRMOVW:
		strcpy_s(name, 8, "MRMOVW");
		break;
	case MRMOVB:
		strcpy_s(name, 8, "MRMOVB");
		break;
	case OP_ADD:
		strcpy_s(name, 8, "ADD");
		break;
	case OP_SUB:
		strcpy_s(name, 8, "SUB");
		break;
	case OP_MUL:
		strcpy_s(name, 8, "MUL");
		break;
	case OP_DIV:
		strcpy_s(name, 8, "DIV");
		break;
	case OP_AND:
		strcpy_s(name, 8, "AND");
		break;
	case OP_OR:
		strcpy_s(name, 8, "OR");
		break;
	case OP_NOT:
		strcpy_s(name, 8, "NOT");
		break;
	case OP_XOR:
		strcpy_s(name, 8, "XOR");
		break;
	case OP_SHR:
		strcpy_s(name, 8, "SHR");
		break;
	case OP_SHL:
		strcpy_s(name, 8, "SHL");
		break;
	case OP_INC:
		strcpy_s(name, 8, "INC");
		break;
	case OP_DEC:
		strcpy_s(name, 8, "DEC");
		break;
	case OP_NEG:
		strcpy_s(name, 8, "NEG");
		break;
	case OP_CMP:
		strcpy_s(name, 8, "CMP");
		break;
	case OP_TEST:
		strcpy_s(name, 8, "TEST");
		break;
	case JMP:
		strcpy_s(name, 8, "JMP");
		break;
	case JLE:
		strcpy_s(name, 8, "JLE");
		break;
	case JL:
		strcpy_s(name, 8, "JL");
		break;
	case JE:
		strcpy_s(name, 8, "JE");
		break;
	case JNE:
		strcpy_s(name, 8, "JNE");
		break;
	case JGE:
		strcpy_s(name, 8, "JGE");
		break;
	case JG:
		strcpy_s(name, 8, "JG");
		break;
	case CALL:
		strcpy_s(name, 8, "CALL");
		break;
	case RET:
		strcpy_s(name, 8, "RET");
		break;
	case PUSH:
		strcpy_s(name, 8, "PUSH");
		break;
	case PUSHI:
		strcpy_s(name, 8, "PUSHI");
		break;
	case POP:
		strcpy_s(name, 8, "POP");
		break;
	case PUSHN:
		strcpy_s(name, 8, "PUSHN");
		break;
	case POPN:
		strcpy_s(name, 8, "POPN");
		break;
	case SYSCALL:
		strcpy_s(name, 8, "SYSCALL");
		break;
	default:
		strcpy_s(name, 8, "???");
		break;
	}
}
