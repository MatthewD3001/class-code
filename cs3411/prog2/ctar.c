#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_LEN 500
#define ERROR_USAGE 1
#define ERROR_ARG 2

typedef struct {
	int		magic;			// This must have the value 0x63746172. Check first 4 bytes in the ctar file to see if they are this
	int		eop;			// End of file pointer. Check the next portion of the file which will be this to see if it is equal to the file size.
							// This is supposed to be the byte offset to the end of the file.
	int		block_count;	// Number of entries in the block which are in-use.
	int		file_size[4];	// File size in bytes for files 1..4.
	char	deleted[4];		// contains binary one at position i if the i-th entry was deleted.
	int		file_name[4];	// Pointer to the name of the file. Done in number of bytes offset
	int		next;			// Pointer to the next header block. Also in the number of bytes needed to reach the next header
} hdr;

							// offset is going to be done with SEEK

void writefile(char *file, hdr *hdrsrc, int tarfd) {
	

	hdrsrc->block_count++;

	int fd = open(file, O_RDONLY);
	
	if (fd == -1) {
		fprintf(stderr, "Invalid file argument: %s not found or invalid file\n", file);
		exit(EXIT_FAILURE);
		return ERROR_ARG;
	}

	printf("Writing in file: %s - eop: %d\n", file, hdrsrc->eop);

	close(fd);
	return 0;
}

void printerr(int code, void *err) {
	char buf[50] = {0};
	switch (code) {
		case ERROR_USAGE:
			break;
		case ERROR_ARG:
			break;
	}
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
	
	int tarfd = open(argv[2], O_WRONLY | O_CREAT, 0644);

	hdr hdr = {
		.magic			= 0x63746172,
		.eop			= 0,
		.block_count	= 0,
		.file_size		= {0, 0, 0, 0},
		.deleted		= {0, 0, 0, 0},
		.file_name		= {0, 0, 0, 0},
		.next			= 0
	};
	
	// This will update the eop to the end of the file in bytes
	write(tarfd, &hdr, sizeof(hdr));
	hdr.eop = lseek(tarfd, 0, SEEK_END);
	lseek(tarfd, 0, SEEK_SET);
	write(tarfd, &hdr, sizeof(hdr));
	


	for(int i = 3; i < argc; i++) {
		if (hdr.block_count == 4) {
			printf("New block needs to be created!!\n");
		}
		writefile(argv[i], &hdr, tarfd)
	}
	
	return 1;
}
