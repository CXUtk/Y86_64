// Y86_64.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Environment.h"


int main(int argc, char ** argv)
{
	unsigned char mystr[] = "Hello World";
	unsigned char pushIns[9];
	pushIns[0] = PUSHI;
	ADDRESS pstr = START_ADDRESS + 100;
	memcpy(pushIns + 1, &pstr, sizeof(ADDRESS));
	unsigned char sys[2];
	sys[0] = SYSCALL;
	sys[1] = Sys_PrintA;
	Environment envir;
	envir.SetMem(START_ADDRESS + 100, mystr, sizeof(mystr));
	envir.SetMem(START_ADDRESS, pushIns, sizeof(pushIns));
	envir.SetMem(START_ADDRESS + sizeof(pushIns), sys, 2);
	envir.Start();

	return 0;
}

