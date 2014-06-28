/**
 * PFS/0.9 file archive decoder
 * Author:  Domen Puncer <domen@coderock.org>
 * License: Public domain
 *
 * BUGS: carefully crafted file could run over your files
 **/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include <unistd.h>

#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t

struct pfs_header {
	char magic[8];	/* PFS/0.9, are there others? */
	u32 wtf1;
	u16 wtf2;
	u16 entries;
};

struct pfs_entry {
	char path[128];	/* i've seen 32 and 40-byte variants */
	u32 wtf;	/* what this be? it seems it's always non zero. path terminator? */
	u32 offset;
	u32 size;
};


/* so noone with BE machine complains! */
static u16 le16_to_cpu(u16 x)
{
	u16 lower = ((u8*)&x)[0];
	u16 upper = ((u8*)&x)[1];

	return lower + (upper << 8);
}

static u32 le32_to_cpu(u32 x)
{
	u32 b0 = ((u8*)&x)[0];
	u32 b1 = ((u8*)&x)[1];
	u32 b2 = ((u8*)&x)[2];
	u32 b3 = ((u8*)&x)[3];

	return b0 + (b1<<8) + (b2 << 16) + (b3 << 24);
}

/**
 * yes, I know, many checks are missing, and real/lseek is ugly,
 * CBA to clean it up
 **/
int main()
{
	int i;
	struct pfs_entry *entries;
	struct pfs_header header;
	void *filebuf;

	char buf[128];
	int path_len;
	char *tmp;

	/* read header */
	read(0, &header, sizeof(header));

	if (strncmp(header.magic, "PFS", 3) != 0) {
		fprintf(stderr, "no PFS signature, exiting");
		return 1;
	}
	header.wtf1 = le32_to_cpu(header.wtf1);
	header.wtf2 = le16_to_cpu(header.wtf2);
	header.entries = le16_to_cpu(header.entries);

	printf("sig:\t%s\n", header.magic);

	printf("wtf1:\t0x%08x\n", header.wtf1);
	printf("wtf2:\t0x%04x\n", header.wtf2);
	printf("entries: %i\n", header.entries);

	/* read directory entries */
	entries = malloc(sizeof(entries[0]) * header.entries);
#define MAX_SIZE (128*1024)
	filebuf = malloc(MAX_SIZE);
	assert(entries);
	assert(filebuf);

	read(0, buf, 128);
	tmp = &buf[strlen(buf)];
	while (*tmp == '\0')
		tmp++;
	path_len = tmp - buf;
	lseek(0, -128, SEEK_CUR);
	printf("detected path length of %i bytes\n", path_len);

	for (i=0; i<header.entries; i++) {
		read(0, entries[i].path, path_len);
		read(0, &entries[i].wtf, 12); /* remaining 3*4 bytes */

		entries[i].wtf = le32_to_cpu(entries[i].wtf);
		entries[i].offset = le32_to_cpu(entries[i].offset);
		entries[i].size = le32_to_cpu(entries[i].size);
	}

	/* read and write files */
	for (i=0; i<header.entries; i++) {
		FILE *file;
		int r;
		printf("%-36s", entries[i].path);
		printf("?1:0x%08x ", entries[i].wtf);
		printf("offset: %-5i ", entries[i].offset);
		printf("size: %i\n", entries[i].size);

		file = fopen(entries[i].path, "wb");
		assert(entries[i].size <= MAX_SIZE);
		r = read(0, filebuf, entries[i].size);
		fwrite(filebuf, 1, entries[i].size, file);
		fclose(file);
	}

	return 0;
}
