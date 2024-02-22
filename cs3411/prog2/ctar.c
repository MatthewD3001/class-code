#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_LEN 8000
#define ERROR_USAGE 1
#define ERROR_ARG 2
#define ERROR_TAR 3
#define ERROR_DUP 4
#define MAGIC 0x63746172

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
        case ERROR_TAR:
            sprintf(buf, "Invalid tar file with same name: %s\n", (char *)err);
            write(2, buf, 100);
            exit(EXIT_FAILURE);
            break;
		case ERROR_DUP:
			sprintf(buf, "Duplicate argument: %s is already in the archive or duplicate argument\n", (char *)err);
			write(2, buf, 100);
			exit(EXIT_FAILURE);
			break;
	}
}

/**
 * This function will verify all of the arguments prior to anything being done. If all tests pass then the tar fd is returned.
 *
 * Param argc - This is the number of args passed
 * Param argv - This is the array of all args passed
 * Param hdr - This is the pointer to the current hdr struct
 * Return - The fd for the tar archive
 */
int verify(int argc, char **argv, hdr *hdr) {
    // Verify the args are correct
	if (argc < 3
		|| argv[1][0] != '-'
		|| (argv[1][1] != 'a' && argv[1][1] != 'd')
		|| argv[1][2] != 0 
	) {
		printerr(ERROR_USAGE, 0);
	}

    // Verify no duplicate args in the given args
    for (int i = 3; i < argc - 1; i++) {
        for (int j = i; j < argc; j++) {
            if (strcmp(argv[i], argv[j])) {
                printerr(ERROR_DUP, argv[i]);
            }
        }
    }

    // Verify that the archive file is valid.
    int tarfd = open(argv[2], O_RDWR, 0644);
    if (tarfd == -1) {
        // file does not exist
        printf("file does not already exist\n");
        tarfd = open(argv[2], O_RDWR | O_CREAT, 0644);

        // Init the first header in the new file.
	    write(tarfd, &hdr, sizeof(hdr));
	    hdr.eop = lseek(tarfd, 0, SEEK_END);
	    lseek(tarfd, 0, SEEK_SET);
	    write(tarfd, &hdr, sizeof(hdr));
    } else {
        // verify it is a valid archive
        read(tarfd, &hdr, sizeof(hdr));
        if (hdr.magic != MAGIC) {
            printf("number read: %d\n", hdr.magic);
            printerr(ERROR_TAR, argv[2]);
        }

        // TODO - Make sure that there are no duplicates in the actual archive file already with the given args
    }

    return tmpfd
}

void resethdr(hdr *hdr) {
	hdr->magic			= 0x63746172;
    hdr->eop			= 0;
	hdr->block_count	= 0;
    for (int i = 0; i < 4; i++) {
        hdr->file_size[i]	= 0;
        hdr->deleted[i]		= 0;
	    hdr->file_name[i]	= 0;
    }
	hdr->next			= 0;
}

/**
 * This is a helper function to return the length of a string EXCLUDING the null byte.
 *
 * Param input: this is the input char pointer for the beginning of a string.
 */
int mystrlen(char *input) {
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

    // First reset the cursor in the archive file.
    lseek(TAR_FD, 0, SEEK_SET);

    read(TAR_FD, hdr, sizeof(*hdr));

    int hdrOffset = 0;
    char firsthdr = 1;
    while(hdr->next) {
        hdrOffset = lseek(TAR_FD, hdr->next, SEEK_SET);
        read(TAR_FD, hdr, sizeof(*hdr));
        firsthdr = 0;
    }
    lseek(TAR_FD, 0, SEEK_SET);

	const int CURR_FD = open(file, O_RDONLY);
	if (CURR_FD == -1) {
		printerr(ERROR_ARG, file);
	}
    int bu;
    for (bu = 0; bu < 4; bu++) {
        if (hdr->file_name[bu]) {
            break;
        }
    }
    printf("blocks used: %d\n", bu);

    const int BLOCK_COUNT = hdr->block_count;
	hdr->file_name[BLOCK_COUNT] = lseek(TAR_FD, 0, SEEK_END);
    
    const short int NAME_LEN = mystrlen(file);
	write(TAR_FD, &NAME_LEN, 2);
	write(TAR_FD, file, NAME_LEN);

	char writeBuf[BUF_LEN];
	int readCount = read(CURR_FD, writeBuf, BUF_LEN);
	while (readCount) {
		write(TAR_FD, writeBuf, readCount);
		readCount = read(CURR_FD, writeBuf, BUF_LEN);
	}
    const int END = lseek(TAR_FD, 0, SEEK_END);
    int fileSize = END - hdr->file_name[BLOCK_COUNT];
    hdr->file_size[BLOCK_COUNT] = fileSize;
	hdr->block_count++;
    lseek(TAR_FD, hdrOffset, SEEK_SET);
	write(TAR_FD, hdr, sizeof(*hdr));

	close(CURR_FD);
/*
    printf("curr hdr start: %d\n", hdrOffset);
    printf("hdr magic: %d\n", hdr->magic);
    printf("hdr eop: %d\n", hdr->eop);
    printf("hdr block_count: %d\n", hdr->block_count);
    printf("hdr file size: %d, %d, %d, %d\n", hdr->file_size[0], hdr->file_size[1], hdr->file_size[2], hdr->file_size[3]);
    printf("hdr deleted: %d, %d, %d, %d\n", hdr->deleted[0], hdr->deleted[1], hdr->deleted[2], hdr->deleted[3]);
    printf("hdr file name: %d, %d, %d, %d\n", hdr->file_name[0], hdr->file_name[1], hdr->file_name[2], hdr->file_name[3]);
    printf("hdr next: %d\n", hdr->next);
*/
    if(hdr->block_count == 4) {
        hdr->next = lseek(TAR_FD, 0, SEEK_END);
        printf("NEW BLOCK hdr next: %d\n", hdr->next);
        lseek(TAR_FD, hdrOffset, SEEK_SET);
	    write(TAR_FD, hdr, sizeof(*hdr));
        //printf("------writing new block and offset = %d\n", hdrOffset);
        resethdr(hdr);
        lseek(TAR_FD, 0, SEEK_END);
        write(TAR_FD, hdr, sizeof(*hdr));
    }

	if (firsthdr) {
        hdr->eop = END;
    } else {        
        lseek(TAR_FD, 0, SEEK_SET);
        read(TAR_FD, hdr, sizeof(*hdr));
        hdr->eop = END;
        lseek(TAR_FD, 0, SEEK_SET);
	    write(TAR_FD, hdr, sizeof(*hdr));
    }

	printf("Wrote file: %s - eop: %d\n-------------------------------------------------\n", file, hdr->eop);
}

int main(int argc, char **argv) {


	hdr hdr = {
		.magic			= MAGIC,
		.eop			= 0,
		.block_count	= 0,
		.file_size		= {0, 0, 0, 0},
		.deleted		= {0, 0, 0, 0},
		.file_name		= {0, 0, 0, 0},
		.next			= 0
	};
    
    // Check to see if the archive file already exists, if it does verify magic number.

    // TODO - check if the files to be inserted are already in the archive
    


    // DO ALL ERROR CHECKING BEFORE HERE
	
	const int TAR_FD = tmpfd;
	
    // TMP RETURN HERE
    return 0;
	
	for(int i = 3; i < argc; i++) {
		writefile(argv[i], &hdr, TAR_FD);
	}
    
	return 1;
}
