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

void printerr(int code, void *err) {
	char buf[100] = {0};
	switch (code) {
		case ERROR_USAGE:
			sprintf(buf, "Proper usage: \"-a\" or \"-d\" followed by file names.\n");
			write(2, buf, 100);
			exit(EXIT_FAILURE);
			break;
		case ERROR_ARG:
			sprintf(buf, "Invalid file argument: %s not found or invalid file\n", (char *)err);
			write(2, buf, 100);
			exit(EXIT_FAILURE);
			break;
	}
}

/**
 * This is a helper function to return the length of a string EXCLUDING the null byte.
 *
 * Param input: this is the input char pointer for the beginning of a string.
 */
short int mystrlen(char *input) {
    char *begin = input;
    while(*input++);
    return --input - begin;
}

/**
* This function will write into a given file using the given header as the main header. If a next header exists then it will move ot that one first.
*
* Param file: This is a null terminated file name from the current working directory.
* Param hdr: This is the first header of the archive file.
* Param TAR_FD: this is the file descriptor for the archive file. This should never change.
*/
void writefile(char *file, hdr *hdr, int TAR_FD) {

    // currhdr is going to be the header that will have the file information that is currently being stored added to it.
    // Note this can also be just the first header if the block count has yet to reach 4.
    hdr *currhdr = hdr;

    if(currhdr->block_count == 4) {
        currhdr->next = hdr->eop;
        currhdr = 
    }

    while(currhdr->next) {
        lseek(TAR_FD, currhdr->next, SEEK_SET);
        read(TAR_FD, currhdr, sizeof(currhdr));
    }
    lseek(TAR_FD, 0, SEEK_SET);

	const int CURR_FD = open(file, O_RDONLY);
	if (CURR_FD == -1) {
		printerr(ERROR_ARG, file);
	}

    const int BLOCK_COUNT = hdr->block_count;
	currhdr->file_name[BLOCK_COUNT] = lseek(TAR_FD, hdr->eop, SEEK_SET);
    
    const short int NAME_LEN = mystrlen(file);
	write(TAR_FD, &NAME_LEN, 2);
	write(TAR_FD, file, mystrlen(file));

	char writeBuf[BUF_LEN];
	int readCount = read(CURR_FD, writeBuf, BUF_LEN);
	while (readCount) {
		write(TAR_FD, writeBuf, readCount);
		readCount = read(CURR_FD, writeBuf, BUF_LEN);
	}

	hdr->eop = lseek(TAR_FD, 0, SEEK_END);
    int fileSize = hdr->eop - currhdr->file_name[BLOCK_COUNT];
    currhdr->file_size[BLOCK_COUNT] = fileSize;
    lseek(TAR_FD, 0, SEEK_SET);
	write(TAR_FD, currhdr, sizeof(currhdr));

	currhdr->block_count++;

	printf("Writing in file: %s - eop: %d\n", file, currhdr->eop);

	close(CURR_FD);
}

int main(int argc, char **argv) {

	if (argc < 3
		|| argv[1][0] != '-'
		|| (argv[1][1] != 'a' && argv[1][1] != 'd')
		|| argv[1][2] != 0 
	) {
		printerr(ERROR_USAGE, 0);
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
		writefile(argv[i], &hdr, tarfd);
	}

    for(int i = 0; i < 4; i++) {
        printf("File %d size stored: %d\n", i, hdr.file_size[i]);
    }
	
	return 1;
}
