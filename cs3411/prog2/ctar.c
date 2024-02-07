#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_LEN 500

typedef struct {
	int		magic;			// This must have the value 0x63746172.
	void	*eop;			// End of file pointer.
	short	block_count;	// Number of entries in the block which are in-use.
	int		file_size[4];	// File size in bytes for files 1..4.
	char	deleted[4];		// contains binary one at position i if the i-th entry was deleted.
	char	*file_name[4];	// Pointer to the name of the file.
	void	*next;			// Pointer to the next header block.
} hdr;

void writefile(char *file, hdr *hdrsrc) {
	
	if (hdrsrc->block_count == 4) {
		hdr hdr = {
			.magic			= 0x63746172,
			.eop			= 0,
			.block_count	= 0,
			.file_size		= {0, 0, 0, 0},
			.deleted		= {0, 0, 0, 0},
			.file_name		= {0, 0, 0, 0},
			.next			= 0
		};
		hdrsrc->next = &hdr;
		printf("New block created!!\n");
	}

	hdrsrc->block_count++;

	int fd = open(file, O_RDONLY);
/*
	if (fd == -1) {
		fprintf(stderr, "Invalid file argument: %s not found or invalid file\n", file);
		exit(EXIT_FAILURE);
	}
*/
	// char buf[BUF_LEN + 1];
}

int main(int argc, char **argv) {

	if (argc < 3
		|| argv[1][0] != '-'
		|| (argv[1][1] != 'a' && argv[1][1] != 'd')
		|| argv[1][2] != 0 
	) {
		fprintf(stderr, "Proper usage: \"-a\" or \"-d\" followed by file names.\n");
		exit(EXIT_FAILURE);
	}


	hdr hdr = {
		.magic			= 0x63746172,
		.eop			= 0,
		.block_count	= 0,
		.file_size		= {0, 0, 0, 0},
		.deleted		= {0, 0, 0, 0},
		.file_name		= {0, 0, 0, 0},
		.next			= 0
	};


	for(int i = 2; i < argc; i++) {
		printf("Writing in file: %s\n", argv[i]);
		writefile(argv[i], &hdr);
	}
	
	return 1;
}
