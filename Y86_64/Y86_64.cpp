// Y86_64.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#define CRTDBG_MAP_ALLOC    

#include <crtdbg.h>
#include "Environment.h"
#include "CodeGen.h"


int main(int argc, char** argv)
{
#ifdef WIN32
	int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

	_CrtSetDbgFlag(tmpFlag);

#endif

	unsigned char mystr[] = "Hello World";

	CodeGen gen;
	Operand operand1;
	operand1.regA = RAX;
	operand1.value = 0x46464646;
	gen.AppendCode(IRMOVD, &operand1);
	operand1.regA = RBX;
	operand1.value = 0x11;
	gen.AppendCode(IRMOVB, &operand1);
	operand1.regA = RBX;
	operand1.regB = RAX;
	gen.AppendCode(OP_ADD, &operand1);
	operand1.regA = RAX;
	operand1.regB = NOREG;
	operand1.value = START_ADDRESS + 100;
	gen.AppendCode(RMMOVD, &operand1);
	gen.AppendCode(PUSHI, &operand1);
	operand1.regA = Sys_PrintA;
	gen.AppendCode(SYSCALL, &operand1);
	operand1.regA = RBX;
	operand1.regB = 0;
	gen.AppendCode(OP_DEC, &operand1);
	operand1.value = -22;
	gen.AppendCode(JNE, &operand1);
	operand1.value = 0x32;
	gen.AppendCode(PUSHN, &operand1);
	operand1.value = 32;
	gen.AppendCode(PUSHI, &operand1);
	operand1.regA = Sys_Malloc;
	gen.AppendCode(SYSCALL, &operand1);
	operand1.regA = RAX;
	gen.AppendCode(PUSH, &operand1);
	operand1.regB = RBX;
	gen.AppendCode(RRMOV, &operand1);
	operand1.value = 0x9675;
	operand1.regA = RBX;
	gen.AppendCode(MRMOVD, &operand1);
	//operand1.regA = Sys_Free;
	//gen.AppendCode(SYSCALL, &operand1);


	//CodeGen gen2;
	//operand1.regA = RBX;
	//operand1.regB = RAX;
	//gen2.AppendCode(OP_ADD, &operand1);
	//operand1.regA = RCX;
	//operand1.value = 7;
	//gen2.AppendCode(IRMOVB, &operand1);
	//operand1.regA = RCX;
	//operand1.regB = RAX;
	//gen2.AppendCode(OP_SHL, &operand1);
	//gen2.AppendCode(RET, &operand1);

	/*
	 * 以上代码的Y86_64汇编表示形式：
	 * 
	 *	IRMOVD	$0x46464646,	RAX
	 *	IRMOVB	$0x11,			RBX
	 *	ADD		RBX,			RAX
	 *	RMMOVD	RAX,			0x1064()
	 *	loop:	
	 *	PUSHI	$0x1064
	 *	SYSCALL	0x10
	 *	DEC		RBX
	 *	JNE		loop
	 */
	/*
	 * 等效C语言代码
	 * char str[] = "Hello World";
	 * int a = 0x46464646;
	 * int b = 0x11;
	 * a += b;
	 * *(int *)str = a;
	 * while(b){
	 *     printStr(str);
	 *     b--;  
	 * }
	 */
	Environment envir;
	envir.SetMem(START_ADDRESS + 100, mystr, sizeof(mystr));
	Code_Buffer buf = gen.GenerateCode();
	//Code_Buffer buf2 = gen2.GenerateCode();
	envir.SetMem(START_ADDRESS, buf.content, buf.size);
	//envir.SetMem(START_ADDRESS + 80, buf2.content, buf2.size);
	envir.Start();


	FILE* file = fopen("out.o", "w");
	ObjectFileHeader header;
	header.type_identifier = MAGIC_NUMBER;
	header.code_seg_offset = sizeof(header);
	header.code_size = buf.size;
	fwrite(&header, sizeof(header), 1, file);
	fwrite(buf.content, buf.size, 1, file);
	fclose(file);

	_CrtDumpMemoryLeaks();
	return 0;
}
