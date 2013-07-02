#if 0
gcc -Wall -Ofast -march=native -o blendump $0 || exit 1
./blendump factory.blend
rm -f blendump
exit 0
#endif

// compile and run: sh blendump.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




#define CLR_NAME		"\e[0;32m"
#define CLR_VALUE		"\e[1;32m"
#define CLR_NAME_SUB	"\e[0;33m"
#define CLR_VALUE_SUB	"\e[1;33m"
#define CLR_NAME_ERR	"\e[0;31m"
#define CLR_VALUE_ERR	"\e[1;31m"
#define CLR_NAME_WARN	"\e[0;35m"
#define CLR_VALUE_WARN	"\e[1;35m"
#define CLR_NAME_PR		"\e[0;36m"
#define CLR_VALUE_PR	"\e[1;36m"
#define CLR_NAME_INFO	"\e[0;37m"
#define CLR_VALUE_INFO	"\e[1;37m"
#define CLR_RESET		"\e[0m"
#define EOL				CLR_RESET"\n"

#define CREATE(type, var)	type * var = malloc(sizeof(type))
#define READ(var, fp)		fread(&(var), sizeof(var), 1, fp);



typedef struct Block
{
	char   code[4];
	size_t size;
	void * oldptr;
	size_t sdna_id;
	size_t nstruct;
}
Block;

typedef struct FileBlock
{
	size_t pos;
	struct FileBlock * next;
	Block block;
}
FileBlock;

FileBlock * file_blocks;
size_t nblock;

FileBlock * FB_find(const char code[4], size_t nth);
FileBlock * FB_find_by_id(size_t id);
FileBlock * FB_find_and_goto(const char code[4], size_t nth, FILE * fp);




static void error(const char * msg);
static void warn (const char * msg);




int main(int argc, const char * argv[])
{
	if (argc != 2) error("parameter error.");

	FILE * fp = fopen(argv[1], "r");
	if (!fp) error("unable to open file.");

	//---------------- dump the header
	char header[12];
	fread(header, 12, 1, fp);
	printf(CLR_NAME"HEADER: "CLR_VALUE"%.12s"EOL, header);

	if (strncmp(header, "BLENDER", 7)) error("not a blender file.");
	printf("\t"CLR_NAME_SUB"identifier:   "CLR_VALUE_SUB"%.7s"EOL, header);
	printf("\t"CLR_NAME_SUB"pointer size: "CLR_VALUE_SUB"%d bits"EOL, (header[7] == '_' ? 32 : 64));
	printf("\t"CLR_NAME_SUB"endianness:   "CLR_VALUE_SUB"%s endian"EOL, (header[8] == 'v' ? "little" : "big"));
	printf("\t"CLR_NAME_SUB"version:      "CLR_VALUE_SUB"%c.%.2s"EOL, header[9], &header[10]);
	if (header[7] != '_') error("only support 32 bits.");
	if (header[8] != 'v') error("only support little endian.");
	warn("DATA blocks are represented as dot(.).");

	//---------------- file blocks
	while (1) {
		CREATE(FileBlock, fblock);
		fblock->pos  = ftell(fp);
		fblock->next = file_blocks;
		file_blocks  = fblock;

		READ(fblock->block, fp);
		Block * b = &fblock->block;
		if (strncmp(b->code, "DATA", 4)) {
			printf("\n"CLR_NAME"BLOCK HEADER: "CLR_VALUE"[%d] at %Xh"EOL, nblock, fblock->pos);
			printf("\t"CLR_NAME_SUB"code:    "CLR_VALUE_SUB"%.4s"EOL, b->code);
			printf("\t"CLR_NAME_SUB"size:    "CLR_VALUE_SUB"%Xh"EOL, b->size);
			printf("\t"CLR_NAME_SUB"old ptr: "CLR_VALUE_SUB"%p"EOL, b->oldptr);
			printf("\t"CLR_NAME_SUB"sdna id: "CLR_VALUE_SUB"%u"EOL, b->sdna_id);
			printf("\t"CLR_NAME_SUB"nstruct: "CLR_VALUE_SUB"%u"EOL, b->nstruct);
		}
		else {
			printf(".");
		}

		nblock++;

		if (!strncmp(b->code, "ENDB", 4)) break;
		fseek(fp, b->size, SEEK_CUR);
	}

	// reverse the block list so that the index is correct.
	{
		FileBlock * fblock = file_blocks;
		file_blocks = NULL;

		while (fblock) {
			FileBlock * next = fblock->next;
			fblock->next = file_blocks;
			file_blocks = fblock;
			fblock = next;
		}
	}

	printf("%p %p %p %p", FB_find("DNA1", 0), FB_find("DATA", 0), FB_find("DATA", 1), FB_find("DATA", 2));

	warn("TODO: print sDNA.");
	warn("TODO: print data structure internals.");

	fclose(fp);
	return 0;
}


static void error(const char * msg)
{
	fprintf(stderr, CLR_NAME_ERR"error: "CLR_VALUE_ERR"%s"EOL, msg);
	exit(1);
}


static void warn(const char * msg)
{
	fprintf(stderr, CLR_NAME_WARN"warning: "CLR_VALUE_WARN"%s"EOL, msg);
}




FileBlock * FB_find(const char code[4], size_t nth)
{
	FileBlock * fblock = file_blocks;
	size_t n = 0;
	while (fblock) {
		Block * b = &fblock->block;
		if (!strncmp(b->code, code, 4)) {
			if (n == nth) return fblock;
			n++;
		}
		fblock = fblock->next;
	}
	return NULL;
}

FileBlock * FB_find_by_id(size_t id)
{
	FileBlock * fblock = file_blocks;
	while (id-- && fblock) fblock = fblock->next;
	return fblock;
}


FileBlock * FB_find_and_goto(const char code[4], size_t nth, FILE * fp)
{
	FileBlock * fblock = FB_find(code, nth);
	if (fblock) fseek(fp, fblock->pos, SEEK_SET);
	return fblock;
}


// vim: noet ts=4 sw=4 sts=0
