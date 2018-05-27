#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../Y86_64/ISA.h"
#include "../Y86_64/Utils.h"

#define INS_BUFFER_SIZE 30
#define TOTAL_BUFFER_SIZE 45

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
	return 0;
}

int close() {
	if (myFile)
		fclose(myFile);
	return 0;
}

void get_ins_operand(char str[INS_BUFFER_SIZE], ADDRESS addr)
{
	Operand operand;
	ISA_OPCode code = (ISA_OPCode)code_seg[addr];
	switch (code)
	{
	case STOP:
	case NOP:
	case RET:
		strcpy_s(str, INS_BUFFER_SIZE, "");
		break;
	case RRMOV: 
	{
		const unsigned char regByte = code_seg[addr + 1];
		operand.regA = regByte & 0xF;
		operand.regB = (regByte >> 4) & 0xF;
		char nameA[4];
		char nameB[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		get_reg_name(nameB, (ISA_Register)operand.regB);
		sprintf(str, "%s, %s", nameA, nameB);
		break; 
	}
	case IRMOVQ:
	{
		operand.regA = code_seg[addr + 1];
		operand.regB = 0;
		char nameA[4];
		get_reg_name(nameA, (ISA_Register)operand.regA); 
		int64_t value;
		memcpy(&value, &code_seg[addr + 2], QUAD_BYTES);
		sprintf(str, "$0x%X, %s", value, nameA);
		break;
	}
	case IRMOVD:
	{
		operand.regA = code_seg[addr + 1];
		operand.regB = 0;
		char nameA[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		int32_t value;
		memcpy(&value, &code_seg[addr + 2], DWORD_BYTES);
		sprintf(str, "$0x%X, %s", value, nameA);
		break;
	}
	case IRMOVW:
	{
		operand.regA = code_seg[addr + 1];
		operand.regB = 0;
		char nameA[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		int16_t value;
		memcpy(&value, &code_seg[addr + 2], WORD_BYTES);
		sprintf(str, "$0x%X, %s", value, nameA);
		break;
	}
	case IRMOVB:
	{
		operand.regA = code_seg[addr + 1];
		operand.regB = 0;
		char nameA[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		int8_t value;
		memcpy(&value, &code_seg[addr + 2], SINGLE_BYTE);
		sprintf(str, "$0x%X, %s", value, nameA);
		break;
	}
	case RMMOVQ:
	{
		const unsigned char regByte = code_seg[addr + 1];
		operand.regA = regByte & 0xF;
		operand.regB = (regByte >> 4) & 0xF;
		char nameA[4];
		char nameB[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		get_reg_name(nameB, (ISA_Register)operand.regB);
		ADDRESS val;
		memcpy(&val, &code_seg[addr + 2], MAX_ADDRESS_BYTES);
		sprintf(str, "%s, 0x%X(%s)", nameA, val, nameB);
		break;
	}
	case RMMOVD:
	{
		const unsigned char regByte = code_seg[addr + 1];
		operand.regA = regByte & 0xF;
		operand.regB = (regByte >> 4) & 0xF;
		char nameA[4];
		char nameB[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		get_reg_name(nameB, (ISA_Register)operand.regB);
		ADDRESS val;
		memcpy(&val, &code_seg[addr + 2], MAX_ADDRESS_BYTES);
		sprintf(str, "%s, 0x%X(%s)", nameA, val, nameB);
		break;
	}
	case RMMOVW:
	{
		const unsigned char regByte = code_seg[addr + 1];
		operand.regA = regByte & 0xF;
		operand.regB = (regByte >> 4) & 0xF;
		char nameA[4];
		char nameB[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		get_reg_name(nameB, (ISA_Register)operand.regB);
		ADDRESS val;
		memcpy(&val, &code_seg[addr + 2], MAX_ADDRESS_BYTES);
		sprintf(str, "%s, 0x%X(%s)", nameA, val, nameB);
		break;
	}
	case RMMOVB:
	{
		const unsigned char regByte = code_seg[addr + 1];
		operand.regA = regByte & 0xF;
		operand.regB = (regByte >> 4) & 0xF;
		char nameA[4];
		char nameB[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		get_reg_name(nameB, (ISA_Register)operand.regB);
		ADDRESS val;
		memcpy(&val, &code_seg[addr + 2], MAX_ADDRESS_BYTES);
		sprintf(str, "%s, 0x%X(%s)", nameA, val, nameB);
		break;
	}
	case MRMOVQ:
	{
		const unsigned char regByte = code_seg[addr + 1];
		operand.regA = regByte & 0xF;
		operand.regB = (regByte >> 4) & 0xF;
		char nameA[4];
		char nameB[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		get_reg_name(nameB, (ISA_Register)operand.regB);
		ADDRESS val;
		memcpy(&val, &code_seg[addr + 2], MAX_ADDRESS_BYTES);
		sprintf(str, "0x%X(%s), %s", nameB, val, nameA);
		break;
	}
	case MRMOVD:
	{
		const unsigned char regByte = code_seg[addr + 1];
		operand.regA = regByte & 0xF;
		operand.regB = (regByte >> 4) & 0xF;
		char nameA[4];
		char nameB[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		get_reg_name(nameB, (ISA_Register)operand.regB);
		ADDRESS val;
		memcpy(&val, &code_seg[addr + 2], MAX_ADDRESS_BYTES);
		sprintf(str, "0x%X(%s), %s", nameB, val, nameA);
		break;
	}
	case MRMOVW:
	{
		const unsigned char regByte = code_seg[addr + 1];
		operand.regA = regByte & 0xF;
		operand.regB = (regByte >> 4) & 0xF;
		char nameA[4];
		char nameB[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		get_reg_name(nameB, (ISA_Register)operand.regB);
		ADDRESS val;
		memcpy(&val, &code_seg[addr + 2], MAX_ADDRESS_BYTES);
		sprintf(str, "0x%X(%s), %s", nameB, val, nameA);
		break;
	}
	case MRMOVB:
	{
		const unsigned char regByte = code_seg[addr + 1];
		operand.regA = regByte & 0xF;
		operand.regB = (regByte >> 4) & 0xF;
		char nameA[4];
		char nameB[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		get_reg_name(nameB, (ISA_Register)operand.regB);
		ADDRESS val;
		memcpy(&val, &code_seg[addr + 2], MAX_ADDRESS_BYTES);
		sprintf(str, "0x%X(%s), %s", nameB, val, nameA);
		break;
	}
	case OP_ADD:
	case OP_SUB:
	case OP_MUL:
	case OP_DIV:
	case OP_AND:
	case OP_OR:
	case OP_XOR:
	case OP_SHR:
	case OP_SHL:
	case OP_CMP:
	case OP_TEST:
	{
		const unsigned char regByte = code_seg[addr + 1];
		operand.regA = regByte & 0xF;
		operand.regB = (regByte >> 4) & 0xF;
		char nameA[4];
		char nameB[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		get_reg_name(nameB, (ISA_Register)operand.regB);
		sprintf(str, "%s, %s", nameA, nameB);
		break;
	}
	case OP_NOT:
	case OP_INC:
	case OP_DEC:
	case OP_NEG:
	case PUSH:
	case POP:
	{
		operand.regA = code_seg[addr + 1];
		operand.regB = 0;
		char nameA[4];
		get_reg_name(nameA, (ISA_Register)operand.regA);
		sprintf(str, "%s", nameA);
		break;
	}
	case JMP:
	case JLE:
	case JL:
	case JE:
	case JNE:
	case JGE:
	case JG:
	{
		int64_t offset;
		memcpy(&offset, &code_seg[addr + 1], MAX_ADDRESS_BYTES);
		sprintf(str, "<At 0x%X>", START_ADDRESS + addr + 9 + offset);
		break;
	}
	case CALL: {
		int64_t pos;
		memcpy(&pos, &code_seg[addr + 1], MAX_ADDRESS_BYTES);
		sprintf(str, "<At 0x%X>", pos);
		break;
	}
	case POPN:
	case PUSHN:
	{
		int32_t value;
		memcpy(&value, &code_seg[addr + 1], DWORD_BYTES);
		sprintf(str, "$%d", value);
		break;
	}
	case PUSHI:
	{
		int64_t value;
		memcpy(&value, &code_seg[addr + 1], MAX_ADDRESS_BYTES);
		sprintf(str, "$%lld", value);
		break;
	}
	case SYSCALL:
	{
		uint8_t type = code_seg[addr + 1];
		sprintf(str, "$0x%X", type);
		break;
	}
	default:
		strcpy_s(str, INS_BUFFER_SIZE, "???");
		break;
	}

}

void get_Instruction(char name[TOTAL_BUFFER_SIZE], ADDRESS addr) {
	char ins_name[8];
	char str[INS_BUFFER_SIZE];
	get_ins_name(ins_name, (ISA_OPCode)code_seg[addr]);
	get_ins_operand(str, addr);
	sprintf(name, "%-8s %-8s", ins_name, str);
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
	fprintf(stdout, "%-34s", tmp);
	char name[TOTAL_BUFFER_SIZE];
	get_Instruction(name, addr);
	fprintf(stdout, "%-25s", name);
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