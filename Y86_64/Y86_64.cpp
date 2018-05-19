// Y86_64.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Environment.h"


int main(int argc, char ** argv)
{
	unsigned char mystr[] = "Hello World";
	unsigned char irmov[10];
	unsigned char rmmovq[10];
	unsigned char pushIns[9];
	pushIns[0] = PUSHI;
	ADDRESS pstr = START_ADDRESS + 100;
	memcpy(pushIns + 1, &pstr, sizeof(ADDRESS));


	irmov[0] = IRMOV;
	irmov[1] = RAX;
	uint64_t word = 0x46464646;
	memcpy(irmov + 2, &word, QUAD_BYTES);

	rmmovq[0] = RMMOVQ;
	rmmovq[1] = RAX;
	ADDRESS address = START_ADDRESS + 100;
	memcpy(rmmovq + 2, &address, sizeof(ADDRESS));


	unsigned char sys[2];
	sys[0] = SYSCALL;
	sys[1] = Sys_PrintA;
	Environment envir;
	envir.SetMem(START_ADDRESS + 100, mystr, sizeof(mystr));

	envir.SetMem(START_ADDRESS, irmov, sizeof(irmov));
	envir.SetMem(START_ADDRESS + sizeof(irmov), rmmovq, sizeof(rmmovq));
	envir.SetMem(START_ADDRESS + sizeof(irmov) + sizeof(rmmovq), pushIns, sizeof(pushIns));
	envir.SetMem(START_ADDRESS + sizeof(irmov) + sizeof(rmmovq) + sizeof(pushIns), sys, sizeof(sys));
	envir.Start();

	return 0;
}

