#include <stdint.h>
#include <stdio.h>
#include <iostream>

//simple define to get fopen working for some reason.
//#ifdef __unix
#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL
//#endif

using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 4)
	{
		printf("Usage: %s [binary file] [output header] [var name]\n", argv[0]);
		return 1;
	}

	FILE* fs;
	if(fopen_s(&fs, argv[1], "rb"))
	{
		printf("Couldn't open %s for read\n", argv[1]);
		return 1;
	}

	FILE* fsout;

	if(fopen_s(&fsout, argv[2], "w"))
	{
		printf("Couldn't open %s for write\n", argv[2]);
		fclose(fs);
		return 1;
	}

	size_t byteCount = 0;

	fprintf(fsout, "const uint8_t %s[] = {\n\t", argv[3]);

	while(!feof(fs))
	{
		uint8_t readByte;
		if(fread(&readByte, 1, 1, fs))
		{
			byteCount++;
			fprintf(fsout, "0x%x", readByte);
			if(!feof(fs))
			{
				fprintf(fsout, ",");
				if((byteCount % 40) == 0)
				{
					fprintf(fsout, "\n\t");
				}
			}
		}
		else break;
	}

	fprintf(fsout, "\n};\n");

	fclose(fsout);
	fclose(fs);

	return 0;
}
