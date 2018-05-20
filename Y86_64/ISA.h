#ifndef Y86_64_ISA_H
#define Y86_64_ISA_H
#include <cinttypes>

typedef uint64_t ADDRESS;

// Y86_64 指令集常量

// 最大寻址字节数
const int MAX_ADDRESS_BYTES = sizeof(uint64_t);
const int MAX_BYTES = 2 + MAX_ADDRESS_BYTES;
const int NUM_OF_REGISTER = 8;
const int QUAD_BYTES = 8;
const int DWORD_BYTES = 4;
const int WORD_BYTES = 2;
const int SINGLE_BYTE = 1;

const ADDRESS INVALID_ADDR = 0x00000;

// 1MB最大内存空间
const ADDRESS MAX_MEMORY_ADDRESS = 0xFFFFF;
const ADDRESS START_ADDRESS = 0x10000;

// 栈的位置
const ADDRESS INIT_STACK_POS = 0x80000;
const ADDRESS MAX_STACK_GROW = 0x04000;

// Y86_64 寄存器
typedef int64_t Reg_Value;

typedef void (*fpState)(void *);

// Y86_64 字节码
enum ISA_OPCode
{
	STOP = 0x00,
	NOP = 0x10,
	// 1 + 1（寄存器）
	RRMOV = 0x20,
	// 1 + 1（寄存器）+ 8（立即数）
	IRMOVQ = 0x30,
	IRMOVD = 0x31,
	IRMOVW = 0x32,
	IRMOVB = 0x33,
	// 1 + 1（寄存器）+ 8（内存地址）
	RMMOVQ = 0x40,
	RMMOVD = 0x41,
	RMMOVW = 0x42,
	RMMOVB = 0x43,
	MRMOVQ = 0x50,
	MRMOVD = 0x51,
	MRMOVW = 0x52,
	MRMOVB = 0x53,
	// OP 类指令
	// 1 + 1（寄存器）
	OP_ADD = 0x60,
	OP_SUB = 0x61,
	OP_MUL = 0x62,
	OP_DIV = 0x63,
	OP_AND = 0x64,
	OP_OR = 0x65,
	OP_NOT = 0x66,
	OP_XOR = 0x67,
	OP_SHR = 0x68,
	OP_SHL = 0x69,
	OP_INC = 0x6A,
	OP_DEC = 0x6B,
	OP_NEG = 0x6C,
	// JMP类指令
	// 1 + 8（目的地址）
	JMP = 0x70,
	JLE = 0x71,
	JL = 0x72,
	JE = 0x73,
	JNE = 0x74,
	JGE = 0x75,
	JG = 0x76,
	// 1 + 8（目的地址）
	CALL = 0x80,
	// 1
	RET = 0x90,
	// 1 + 1（寄存器）
	PUSH = 0xA0,
	// 1 + 8（立即数）
	PUSHI = 0xA1,
	// 1 + 1（寄存器）
	POP = 0xB0,
	// 1 + 1（功能）
	SYSCALL = 0xC0,
};

// Y86_64 寄存器代码
enum ISA_Register
{
	RAX = 0x0,
	RCX = 0x1,
	RDX = 0x2,
	RBX = 0x3,
	RSP = 0x4,
	RBP = 0x5,
	RSI = 0x6,
	RDI = 0x7,
	NOREG = 0x8,
};

enum ProgramState
{
	PS_OK = 0x0,
	PS_HALT = 0x1,
	PS_ADR = 0x2,
	PS_INS = 0x3,
	PS_STACK_OVERFLOW = 0x4,
	PS_STACK_UNDERFLOW = 0x5,
	PS_DIV_BY_ZERO = 0x6,
};

enum SysCall_Type
{
	Sys_TestFunc = 0x00,
	// 参数1：字符串地址
	// 功能： 向标准输出流打印字符
	Sys_PrintA = 0x10,
	// 参数1：整数的地址
	// 功能： 向标准输出流打印整数字符
	Sys_PrintD = 0x11,
	// 参数1：目标寄存器
	// 功能： 向标准输出流打印字符
	Sys_PrintAR = 0x12,
	// 参数1：目标寄存器
	// 功能： 向标准输出流打印整数字符
	Sys_PrintDR = 0x13,
	// 参数1：目标存储地址（char）
	// 功能： 从标准输入读取一个字符，存到地址里面
	Sys_GetA = 0x20,
	// 参数1：目标寄存器
	// 功能： 从标准输入读取一个字符，存到寄存器里面
	Sys_GetR = 0x21,
	// 无参数
	// 功能： 获取当前系统时间
	Sys_Time = 0x31,
};

enum ConditionBit
{
	CC_ZERO,
	CC_OVERFLOW,
	CC_SIGN,
	// Not used
	CC_CARRY,
};


#endif
