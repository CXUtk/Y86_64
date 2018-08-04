#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <setjmp.h>


#include "Parser.h"
#include "../Y86_64/ISA.h"
#include "../Y86_64/CodeGen.h"

#define MAX_LINE_NUM 256


FILE * textFile = nullptr;
FILE * writeFile = nullptr;
Code_Buffer out_buffer;


int lineNum;

typedef struct {
	char * str;
	size_t strlen;
} StrLine;

StrLine * lines = nullptr;

int open(const char * filename)
{
	textFile = fopen(filename, "r+");
	if (!textFile) {
		return -1;
	}
	return 0;
}

int countLines()
{
	int cnt = 0;
	int c;
	while((c = fgetc(textFile)) != EOF)
	{
		if (c == '\n')
			cnt++;
	}
	fseek(textFile, 0, SEEK_SET);
	return cnt + 1;
}

void read()
{
	// 读取文件所有行，并且放进lines数组里
	char line[MAX_LINE_NUM];
	int count = 0;
	lineNum = countLines();
	lines = (StrLine *) malloc(sizeof(StrLine) * lineNum);
	while(fgets(line, MAX_LINE_NUM, textFile) != nullptr)
	{
		if(line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = '\0';
		lines[count].str = (char *)malloc(sizeof(line) + 1);
		strcpy_s(lines[count].str, MAX_LINE_NUM, line);
		lines[count].strlen = strlen(line);
		count++;
	}
}

void close()
{
	fclose(textFile);

}

void parse()
{

	Parser parser;
	// TODO: 你的代码位置

	for(int i = 0; i < lineNum; i++)
	{
		char * target = lines[i].str;
		parser.Run(target);

		printf("%s\n", target);
	}


	out_buffer = parser.getCode();
}

int write()
{
	writeFile = fopen("a.out", "w");
	if (!writeFile) {
		return -1;
	}
	ObjectFileHeader header;
	header.type_identifier = MAGIC_NUMBER;
	header.code_seg_offset = sizeof(header);
	header.code_size = out_buffer.size;
	fwrite(&header, sizeof(ObjectFileHeader), 1, writeFile);
	fwrite(out_buffer.content, out_buffer.size, 1, writeFile);
	fclose(writeFile);
	return 0;
}

int main(int argc, char* argv[])
{
	if(open(argv[1]) != -1)
	{
		read();
		close();
		parse();
		write();
		for (int i = 0; i < lineNum; i++)
		{
			free(lines[i].str);
		}
		free(lines);
		getchar();
	}
	delete[] out_buffer.content;
	return 0;
}
