// Y86_64.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Environment.h"
#include "Debug.h"


int main(int argc, char ** argv)
{
	unsigned char mystr[] = "Hello World";
	//unsigned char irmov[10];
	//unsigned char rmmovq[10];
	//unsigned char pushIns[9];
	//pushIns[0] = PUSHI;
	//ADDRESS pstr = START_ADDRESS + 100;
	//memcpy(pushIns + 1, &pstr, sizeof(ADDRESS));


	//irmov[0] = IRMOV;
	//irmov[1] = RAX;
	//uint64_t word = 0x46464646;
	//memcpy(irmov + 2, &word, QUAD_BYTES);

	//rmmovq[0] = RMMOVQ;
	//rmmovq[1] = RAX;
	//ADDRESS address = START_ADDRESS + 100;
	//memcpy(rmmovq + 2, &address, sizeof(ADDRESS));


	//unsigned char sys[2];
	//sys[0] = SYSCALL;
	//sys[1] = Sys_PrintA;
	Debug debugger;
	Operand operand1;
	operand1.regA = RAX;
	operand1.value = 0x46464646;
	debugger.AppendCode(IRMOVD, &operand1);
	operand1.regA = RBX;
	operand1.value = 0x11;
	debugger.AppendCode(IRMOVB, &operand1); 
	operand1.regA = RBX;
	operand1.regB = RAX;
	debugger.AppendCode(OP_ADD, &operand1);
	operand1.regA = RAX;
	operand1.regB = NOREG;
	operand1.value = START_ADDRESS + 100;
	debugger.AppendCode(RMMOVD, &operand1);
	debugger.AppendCode(PUSHI, &operand1);
	operand1.regA = Sys_PrintA;
	debugger.AppendCode(SYSCALL, &operand1);
	/*
	 * 以上代码的Y86_64汇编表示形式：
	 * 
	 *	IRMOVD	$0x46464646,	RAX
	 *	IRMOVB	$0x11,			RBX
	 *	ADD		RBX,			RAX
	 *	RMMOVD	RAX,			0x1064()
	 *	PUSHI	$0x1064
	 *	SYSCALL	0x10
	 */
	
	Environment envir;
	envir.SetMem(START_ADDRESS + 100, mystr, sizeof(mystr));
	Code_Buffer buf = debugger.GenerateCode();
	envir.SetMem(START_ADDRESS, buf.content, buf.size);
	envir.Start();

	return 0;
}

