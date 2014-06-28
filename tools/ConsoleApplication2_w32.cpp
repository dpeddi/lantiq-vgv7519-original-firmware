// ConsoleApplication2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

//xor chars in str with xorchar
void xor(unsigned char* bytes, int len, char xorchar) {
    int i;
 
    for (i = 0; i < len; i++) {
        bytes[i] = bytes[i] ^ xorchar;
    }
}
 
//swap high and low bits in bytes in str
//0x12345678 -> 0x21436578
void hilobswap(unsigned char* bytes, int len) {
    int i;
 
    for (i = 0; i < len; i++) {
        bytes[i] = (bytes[i] << 4) + (bytes[i] >> 4);
    }
}
 
//swap byte[i] with byte[i+1]
//0x12345678 -> 0x34127856
void wswap(unsigned char* bytes, int len) {
    int i;
    unsigned char tmp;
 
    for (i = 0; i < len; i += 2) {
        tmp = bytes[i];
        bytes[i] = bytes[i + 1];
        bytes[i + 1] = tmp;
    }
}

int brn_import(unsigned char* buffer, size_t insize, FILE* infile) 
{
    unsigned char* tmpbuffer[0x400];

    fread(buffer + 4, 1, insize - 4, infile);
		buffer[0] = 0x12;
		buffer[1] = 0x34;
		buffer[2] = 0x56;
		buffer[3] = 0x78;
 
    printf("scrambling file ...\n");

    //xor HITECH
    xor(buffer + 0x404, 0x400, 0x48);
    xor(buffer + 0x804, 0x400, 0x49);
    xor(buffer + 0x4, 0x400, 0x54);
    xor(buffer + 0x404, 0x400, 0x45);
    xor(buffer + 0x804, 0x400, 0x43);
    xor(buffer + 0xC04, 0x400, 0x48);
 
    //swap 0x4 0x404
    memcpy(tmpbuffer, buffer + 0x4, 0x400);
    memcpy(buffer + 0x4, buffer + 0x404, 0x400);
    memcpy(buffer + 0x404, tmpbuffer, 0x400);
 
    //xor NET
    xor(buffer + 0x4, 0x400, 0x4E);
    xor(buffer + 0x404, 0x400, 0x45);
    xor(buffer + 0x804, 0x400, 0x54);
 
    //swap 0x4 0x804
    memcpy(tmpbuffer, buffer + 0x4, 0x400);
    memcpy(buffer + 0x4, buffer + 0x804, 0x400);
    memcpy(buffer + 0x804, tmpbuffer, 0x400);
 
    //xor BRN
    xor(buffer + 0x4, 0x400, 0x42);
    xor(buffer + 0x404, 0x400, 0x52);
    xor(buffer + 0x804, 0x400, 0x4E);
 
    //fix header #1
    memcpy(tmpbuffer, buffer + 0x4, 0x20);
    memcpy(buffer + 0x4, buffer + 0x68, 0x20);
    memcpy(buffer + 0x68, tmpbuffer, 0x20);
 
    //fix header #2
    hilobswap(buffer + 0x4, 0x20);
    wswap(buffer + 0x4, 0x20);
 
		return 0;
}

int brn_extract(unsigned char* buffer, size_t insize, FILE* outfile) 
{
    unsigned char* tmpbuffer[0x400];
 
    printf("descrambling file ...\n");

    //xor HITECH
    xor(buffer + 0x404, 0x400, 0x48);
    xor(buffer + 0x804, 0x400, 0x49);
    xor(buffer + 0x4, 0x400, 0x54);
    xor(buffer + 0x404, 0x400, 0x45);
    xor(buffer + 0x804, 0x400, 0x43);
    xor(buffer + 0xC04, 0x400, 0x48);
 
    //swap 0x4 0x404
    memcpy(tmpbuffer, buffer + 0x4, 0x400);
    memcpy(buffer + 0x4, buffer + 0x404, 0x400);
    memcpy(buffer + 0x404, tmpbuffer, 0x400);
 
    //xor NET
    xor(buffer + 0x4, 0x400, 0x4E);
    xor(buffer + 0x404, 0x400, 0x45);
    xor(buffer + 0x804, 0x400, 0x54);
 
    //swap 0x4 0x804
    memcpy(tmpbuffer, buffer + 0x4, 0x400);
    memcpy(buffer + 0x4, buffer + 0x804, 0x400);
    memcpy(buffer + 0x804, tmpbuffer, 0x400);
 
    //xor BRN
    xor(buffer + 0x4, 0x400, 0x42);
    xor(buffer + 0x404, 0x400, 0x52);
    xor(buffer + 0x804, 0x400, 0x4E);
 
    //fix header #1
    memcpy(tmpbuffer, buffer + 0x4, 0x20);
    memcpy(buffer + 0x4, buffer + 0x68, 0x20);
    memcpy(buffer + 0x68, tmpbuffer, 0x20);
 
    //fix header #2
    hilobswap(buffer + 0x4, 0x20);
    wswap(buffer + 0x4, 0x20);
 
    fwrite(buffer + 4, 1, insize - 4, outfile);
		return 0;
}

#define FLAG_LZMA 1
#define FLAG_BRN  2

struct _file_parts {
	char *label;
	size_t offset;
	size_t size;
	int flags;
} file_parts[2][11] = {
	{	{ "boot0",					0,       0x1000,   0 },
		{ "boot1",					0x1000,  0xF000,   0 },
		{ "uboot",					0x10000, 0x30000,  FLAG_LZMA },  // 5D000080 == LZMA
		{ "bootparams",			0x40000, 0x400,    0 },
		{ "bootparams_brn", 0x40400, 0x1000,   0 },
		{ "bootparams_clr",	0x41400, 0xEC00,   0 }, // empty blocks
		{ "certificate",		0x50000, 0x10000,  0 },
		{ "special",				0x60000, 0x10000,  0 },
		{ "reserve_0",			0x70000, 0x10000,  0 },
		{ "code_image_0",		0x80000, 0x780000, FLAG_BRN | FLAG_LZMA }, // brn, lzma
		{ NULL } },
	{	{ "primary_setting",	0,				0x10000,  0 },
		{ "configuration",		0x10000,	0x60000,  FLAG_BRN },
		{ "reserve_1",				0x70000,	0x10000,  0 },
		{ "code_image_1",			0x80000,	0x780000, FLAG_BRN | FLAG_LZMA }, // brn, lzma
	{ NULL } }
};
struct _files {
	char *filename;
	size_t filesize;
} file_list[2] = {
	{ "vgv7519_flash1.bin", 0x800000 },
	{ "vgv7519_flash2.bin", 0x800000 }
};

void extract_flashes()
{
	CreateDirectory(L"extract", NULL);
	for (int i = 0; i < 2; i++) {
		char filename_base[MAX_PATH] = {0};
		strcpy(filename_base, file_list[i].filename);
		*strchr(filename_base, '.') = 0;
		
		// read the file
		unsigned char* file_buffer = (unsigned char*)malloc(file_list[i].filesize);
		FILE* fIn = fopen(file_list[i].filename, "rb");
		assert(fIn);
		fread(file_buffer, 1, file_list[i].filesize, fIn);
		fclose(fIn);

		struct _file_parts* pFileParts = file_parts[i];
		int j = 0;
		while (pFileParts->label != NULL) {
			char filename[MAX_PATH];
			if (pFileParts->flags & FLAG_LZMA) {
				sprintf(filename, "extract\\%s_%d_%s.lzma", filename_base, j, pFileParts->label);
			} else {
				sprintf(filename, "extract\\%s_%d_%s.bin", filename_base, j, pFileParts->label);
			}

			FILE * fOut = fopen(filename, "wb");
			if (pFileParts->flags & FLAG_BRN) {
				brn_extract(&file_buffer[pFileParts->offset], pFileParts->size, fOut);
			} else {
				fwrite(&file_buffer[pFileParts->offset], 1, pFileParts->size, fOut);
			}
			fclose(fOut);
			if (pFileParts->flags & FLAG_LZMA) {
				PROCESS_INFORMATION pi;
				STARTUPINFO si;
				memset(&si, 0, sizeof(si));
				memset(&pi, 0, sizeof(pi));
				wchar_t sCommandLine[MAX_PATH];
				wsprintf(sCommandLine, L"lzma.exe d %S extract\\%S_%d_%S.bin", filename, filename_base, j, pFileParts->label);
				BOOL bReturn = CreateProcess(L"lzma.exe", sCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
				CloseHandle(si.hStdInput);
				CloseHandle(si.hStdOutput);
				CloseHandle(si.hStdError);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				if (bReturn == 0) {
					DWORD dwError = GetLastError();
					dwError = dwError;
				}
			}
			pFileParts++; j++;
		}
		free(file_buffer);
	}
}


void pack_flashes()
{
	// First convert the lzma files back to normal
	for (int i = 0; i < 2; i++) {
		char filename_base[MAX_PATH] = {0};
		strcpy(filename_base, file_list[i].filename);
		*strchr(filename_base, '.') = 0;

		struct _file_parts* pFileParts = file_parts[i];
		int j = 0;
		while (pFileParts->label != NULL) {
			if ((pFileParts->flags & FLAG_LZMA) == 0) {
				pFileParts++; j++;
				continue;
			}
				
			char filename[MAX_PATH];
			sprintf(filename, "modified\\%s_%d_%s.lzma", filename_base, j, pFileParts->label);

			PROCESS_INFORMATION pi;
			STARTUPINFO si;
			memset(&si, 0, sizeof(si));
			memset(&pi, 0, sizeof(pi));
			wchar_t sCommandLine[MAX_PATH];
			wsprintf(sCommandLine, L"lzma.exe e modified\\%S_%d_%S.bin %S", filename_base, j, pFileParts->label, filename);
			BOOL bReturn = CreateProcess(L"lzma.exe", sCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
			CloseHandle(si.hStdInput);
			CloseHandle(si.hStdOutput);
			CloseHandle(si.hStdError);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			if (bReturn == 0) {
				DWORD dwError = GetLastError();
				dwError = dwError;
			}
			pFileParts++; j++;
		}
	}

	// read all files and pack them together
	for (int i = 0; i < 2; i++) {
		char filename_base[MAX_PATH] = {0};
		strcpy(filename_base, file_list[i].filename);
		*strchr(filename_base, '.') = 0;

		unsigned char* file_buffer = (unsigned char*)calloc(file_list[i].filesize, 1);

		struct _file_parts* pFileParts = file_parts[i];
		int j = 0;
		while (pFileParts->label != NULL) {
			char filename[MAX_PATH];
			if (pFileParts->flags & FLAG_LZMA) {
				sprintf(filename, "modified\\%s_%d_%s.lzma", filename_base, j, pFileParts->label);
			} else {
				sprintf(filename, "modified\\%s_%d_%s.bin", filename_base, j, pFileParts->label);
			}

			FILE * fIn = fopen(filename, "rb");
			if (pFileParts->flags & FLAG_BRN) {
				brn_import(&file_buffer[pFileParts->offset], pFileParts->size, fIn);
			} else {
				fread(&file_buffer[pFileParts->offset], 1, pFileParts->size, fIn);
			}
			fclose(fIn);
			pFileParts++; j++;
		}

		// write the file
		char filename[MAX_PATH];
		sprintf(filename, "%s_packed.bin", filename_base);
		FILE* fOut = fopen(filename, "wb");
		assert(fOut);
		fwrite(file_buffer, 1, file_list[i].filesize, fOut);
		fclose(fOut);

		free(file_buffer);
	}
}

int main(int argc, char *argv[])
{
	//if (*argv[1] == 'p') {
		//pack_flashes();
	//} else {
		extract_flashes();
	//}
  return 0;
}
