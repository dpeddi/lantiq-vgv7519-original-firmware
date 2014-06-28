#include<stdio.h>
#include<stdlib.h>
#include<string.h>
 
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
 
int main(int argc, char *argv[]) {
    unsigned char* buffer;
    unsigned char* tmpbuffer[0x400];
    size_t insize;
    FILE *infile, *outfile;
 
    if (argc != 3) {
        printf("usage: easybox_deobfuscate infile outfile.bin.lzma\n");
        return -1;
    }
 
    //read obfuscated file
    infile = fopen(argv[1], "rb");
 
    if (infile == NULL) {
        fputs("cant open infile", stderr);
        return -1;
    }
 
    fseek(infile, 0, SEEK_END);
    insize = ftell(infile);
    rewind(infile);
 
    buffer = (unsigned char*) malloc(insize);
    if (buffer == NULL) {
        fputs("memory error", stderr);
        exit(2);
    }
 
    printf("read \t%i bytes\n", fread(buffer, 1, insize, infile));
    fclose(infile);
 
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
 
    //write deobfuscated file
    outfile = fopen(argv[2], "wb");
 
    if (outfile == NULL) {
        fputs("cant open outfile", stderr);
        return -1;
    }
 
    printf("wrote \t%i bytes\n", fwrite(buffer + 4, 1, insize - 4, outfile));
    fclose(outfile);
 
    printf("all done! - use lzma to unpack\n");
 
    return 0;
}
