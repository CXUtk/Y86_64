#ifndef Y86_64_ISA_H
#define Y86_64_ISA_H
#include <inttypes.h>

typedef uint64_t ADDRESS;

// Y86_64 指令集常量
const int MAX_ADDRESS_BYTES = 8;
const int MAX_BYTES = 2 + MAX_ADDRESS_BYTES;
const int NUM_OF_REGISTER = 8;

const ADDRESS INVALID_ADDR = 0x00000;

// 1MB内存空间
const ADDRESS MAX_MEMORY_ADDRESS = 0xFFFFF;
const ADDRESS START_ADDRESS = 0x10000;

// 栈的位置
const ADDRESS INIT_STACK_POS = 0x80000;
const ADDRESS MAX_STACK_GROW = 0x04000;

typedef void (*fpState)(void *);

// Y86_64 字节码
enum ISA_ByteCode
{
	STOP = 0x00,
	NOP = 0x10,
	RRMOV = 0x20,
	IRMOV = 0x30,
	RMMOV = 0x40,
	MRMOV = 0x50,
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
	JMP = 0x70,
	JLE = 0x71,
	JL = 0x72,
	JE = 0x73,
	JNE = 0x74,
	JGE = 0x75,
	JG = 0x76,
	CALL = 0x80,
	RET = 0x90,
	PUSH = 0xA0,
	PUSHI = 0xA1,
	POP = 0xB0,
	CMP = 0xC0,
	SYSCALL = 0xD0,
};

// Y86_64 寄存器代码
enum ISA_REGISTER
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

// Y86_64 寄存器
typedef int64_t Register;

enum ProgramStatus
{
	PS_OK = 0x0,
	PS_HALT = 0x1,
	PS_ADR = 0x2,
	PS_INS = 0x3,
	PS_STACK_OVERFLOW = 0x4,
	PS_STACK_UNDERFLOW = 0x5,
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


#endif
