#include <stdio.h>

#define BUF_LEN 500

typedef struct hdr {
	int magic;		// This must have the value 0x63746172.
	char *eop;		// End of file pointer.
	short block_count;	// Number of entries in the block which are in-use.
	int file_size[4];	// File size in bytes for files 1..4.
	char deleted[4];	// contains binary one at position i if the i-th entry was deleted.
	char *file_name[4];	// Pointer to the name of the file.
	char *next;		// Pointer to the next header block.
} hdr;



int main(int argc, char **argv){

	// Validate the arguments before here
	
	struct hdr hdr0 = {
		.magic		= 0x63746172,
		.eop		= 0,
		.block_count	= 0,
		.file_size	= {0, 0, 0, 0},
		.deleted	= {0, 0, 0, 0},
		.file_name	= {0, 0, 0, 0},
		.next		= 0
	};

	// char buf[BUF_LEN + 1] = {0};
	
	printf("argc: %d\nargv: %s\n", argc, argv[1]);
	// int magic_number = 69;
	// hdr0.magic = &magic_number;
	printf("Magic value: %d\n", hdr0.magic);
	return 1;
}
