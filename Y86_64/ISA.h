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
const uint32_t MAGIC_NUMBER = 0x2c42f75b;

const ADDRESS INVALID_ADDR = 0x00000;

// 1MB最大内存空间
const ADDRESS MAX_MEMORY_ADDRESS = 0xFFFFF;
const ADDRESS START_ADDRESS = 0x10000;

// 栈的位置
const ADDRESS INIT_STACK_POS = 0x80000;
const ADDRESS MAX_STACK_SIZE = 0x04000;

// 堆的位置
const ADDRESS INIT_HEAP_POS = 0xA0000;
const ADDRESS MAX_HEAP_SIZE = 0x20000;
const int HEAP_CHUNK_SIZE = 4096;

// 汇编代码解析符号常量
const char POS_INDICATION_TOKEN[] = ".pos";
const char QUAD_BYTE_DATA_TOKEN[] = ".quad";
const char DWORD_BYTE_DATA_TOKEN[] = ".dword";
const char WORD_BYTE_DATA_TOKEN[] = ".word";
const char BYTE_DATA_TOKEN[] = ".byte";
const char STRING_DATA_TOKEN[] = ".string";
const char HEX_NUMBER_HEAD_TOKEN[] = "0x";
const char IMMIDATE_NUMBER_TOKEN = '$';
const char COMMENT_TOKEN = '#';

// Y86_64 寄存器
typedef int64_t Reg_Value;

// Y86_64 字节码
enum ISA_OPCode
{
	STOP = 0x00,
	NOP = 0x10,
	// 1 + 1（寄存器）
	RRMOV = 0x20,
	// 把立即数放进寄存器中，按照字节大小
	// 1 + 1（寄存器）+ 8（立即数）
	IRMOVQ = 0x30,
	IRMOVD = 0x31,
	IRMOVW = 0x32,
	IRMOVB = 0x33,
	// 寄存器中的值放进内存中，按照字节大小
	// 1 + 1（寄存器）+ 8（内存地址）
	RMMOVQ = 0x40,
	RMMOVD = 0x41,
	RMMOVW = 0x42,
	RMMOVB = 0x43,
	MRMOVQ = 0x50,
	MRMOVD = 0x51,
	MRMOVW = 0x52,
	MRMOVB = 0x53,
	// OP 类指令，CPU运算
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
	OP_CMP = 0x6D,
	OP_TEST = 0x6E,
	// JMP类指令，跳转到当前地址结束+偏移地址
	// 1 + 8（目的地址）
	JMP = 0x70,
	JLE = 0x71,
	JL = 0x72,
	JE = 0x73,
	JNE = 0x74,
	JGE = 0x75,
	JG = 0x76,
	// 跳转到指定地址，并且执行函数
	// 1 + 8（目的地址）
	CALL = 0x80,
	// 函数返回，跳转回执行前地址
	// 1
	RET = 0x90,
	// 将寄存器中的值入栈，8字节
	// 1 + 1（寄存器）
	PUSH = 0xA0,
	// 将立即数入栈，8字节
	// 1 + 8（立即数）
	PUSHI = 0xA1,
	// 将一定字节的数据入栈，仅开辟栈空间
	// 1 + 8（立即数）
	PUSHN = 0xA2,
	// 将栈上的8字节数据弹入寄存器
	// 1 + 1（寄存器）
	POP = 0xB0,
	// 从栈中空弹一定字节的数据
	// 1 + 8（立即数）
	POPN = 0xB1,
	// 执行系统调用
	// 1 + 1（功能）
	SYSCALL = 0xC0,

	ERR = 0xFF,
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
	// 功能： 向标准输出流打印8字节整数字符
	Sys_PrintDQ = 0x11,
	// 参数1：目标寄存器
	// 功能： 向标准输出流打印字符
	Sys_PrintAR = 0x12,
	// 参数1：目标寄存器
	// 功能： 向标准输出流打印8字节整数字符
	Sys_PrintDR = 0x13,
	// 参数1：目标存储地址（char）
	// 功能： 从标准输入读取一个字符，存到地址里面
	Sys_GetA = 0x20,
	// 参数1：目标寄存器
	// 功能： 从标准输入读取一个字符，存到寄存器里面
	Sys_GetR = 0x21,
	// 无参数
	// 功能： 获取当前系统时间
	Sys_Time = 0x30,
	// 参数1：分配内存字节大小
	// 功能： 在堆中分配指定大小的内存，RAX返回指向内存区域的指针
	Sys_Malloc = 0x40,
	// 参数1：Malloc返回的内存起始地址指针
	// 功能： 在堆中释放之前分配过的内存
	Sys_Free = 0x41,
};

enum ConditionBit
{
	CC_ZERO,
	CC_OVERFLOW,
	CC_SIGN,
	// Not used
	CC_CARRY,
};

typedef struct {
	// 文件类型确认符
	uint32_t type_identifier;
	// 代码段偏移量（相对文件头）
	size_t code_seg_offset;
	// 代码段大小
	size_t code_size;
	// 数据段偏移量
	size_t data_seg_offset;
	// 数据段大小
	size_t data_size;
} ObjectFileHeader;


#endif
