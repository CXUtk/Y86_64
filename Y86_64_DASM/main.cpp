#include <cstdio>
#include <cstdlib>

#include "../Y86_64/ISA.h"

FILE * myFile = nullptr;
unsigned char * code_seg = nullptr;
ObjectFileHeader header;

int open_read(char * filename) {
	myFile = fopen(filename, "r");
	if (!myFile) {
		return -1;
	}

	if (fread(&header, sizeof(header), 1, myFile) != 1) {
		fprintf(stderr, "无法读取对象文件!\n");
		return -1;
	}
	if (header.type_identifier != MAGIC_NUMBER) {
		fprintf(stderr, "文件类型不是合法对象文件!\n");
		return -1;
	}
	code_seg = (unsigned char *)malloc(header.code_size);
	fseek(myFile, header.code_seg_offset, SEEK_SET);
	if (fread(code_seg, header.code_size, 1, myFile) != 1) {
		fprintf(stderr, "代码段读取失败!\n");
		return -1;
	}
}

int close() {
	if (myFile)
		fclose(myFile);
	return 0;
}

ADDRESS nextIns(ADDRESS addr) {
	ISA_OPCode code = (ISA_OPCode) code_seg[addr];
	ADDRESS offset = 0;
	ISA_OPCode type = (ISA_OPCode)(code & 0xF0);
	if (type == MRMOVQ || type == RMMOVQ || code == IRMOVQ) {
		offset = 10;
	}
	else if (code == PUSHI || code == CALL || type == JMP) {
		offset = 9;
	}
	else if (code == IRMOVD) {
		offset = 6;
	}
	else if (code == IRMOVW) {
		offset = 4;
	}
	else if (code == IRMOVB) {
		offset = 3;
	}
	else if (code == RRMOV || type == OP_ADD || code == PUSH || code == POP || code == SYSCALL) {
		offset = 2;
	}
	else {
		offset = 1;
	}
	fprintf(stdout, "0x%-8.8X: ", addr + START_ADDRESS);
	char tmp[32];
	sprintf(tmp, "%.2X ", code_seg[addr]);
	for (int i = 1; i < offset; i++) {
		sprintf(tmp + i * 3, "%.2X ", code_seg[addr + i]);
	}
	fprintf(stdout, "%-25s", tmp);
	fprintf(stdout, "%25s", "MOV EAX, EBX");
	fprintf(stdout, "\n");
	fflush(stdout);

	return offset;
}

void parse() {
	ADDRESS pos = 0;
	while (pos <= header.code_size) {
		ADDRESS offset = nextIns(pos);
		pos += offset;
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		fprintf(stderr, "用法: Y86_64_DASM <文件名>\n");
		return 0;
	}
	if (open_read(argv[1]) != -1) {
		parse();
		close();
	}
	getchar();
	return 0;
}