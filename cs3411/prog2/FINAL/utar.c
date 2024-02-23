#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_LEN 8000

#define ERROR_USAGE 1
#define ERROR_TAR 2
#define ERROR_FILE 3

#define FNF 4               // File Not Found

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

/**
* This function handles a given error
*
* Param code - the code which caused the error
* Param err - the string, if any, relating to the error
*/
void printerr(int code, void *err) {
	char buf[100] = {0};
	switch (code) {
		case ERROR_USAGE:
			sprintf(buf, "Proper usage: only the archive name\n");
			write(2, buf, 100);
			exit(EXIT_FAILURE);
			break;
        case ERROR_TAR:
            sprintf(buf, "Invalid tar file with name: \"%s\"\n", (char *)err);
            write(2, buf, 100);
            exit(EXIT_FAILURE);
            break;
		case ERROR_FILE:
			sprintf(buf, "File: \"%s\" is already in the current directory move away to proceed\n", (char *)err);
			write(2, buf, 100);
			exit(EXIT_FAILURE);
			break;
	}
}

/**
 * This function verifies that a given file is not in the current directory
 * 
 * Param TAR_FD - the fd for the current archive
 * Param fileOffset - the offset to the file name
 */ 
void verifyfile(const int TAR_FD, int fileOffset) {
    lseek(TAR_FD, fileOffset, SEEK_SET);
    short int nameLen;
    read(TAR_FD, &nameLen, 2);
    char fileName[nameLen + 1];
    read(TAR_FD, fileName, nameLen);
    fileName[nameLen] = 0;

    int try = open(fileName, O_RDONLY);
    if (try != -1) {
        close(try);
        printerr(ERROR_FILE, fileName);
    }
}

/**
* This function verifies the program will run successfully
*
* Param argc - num of args
* Param argv - the given args
* Param hdr - this is the pointer to the current header
* Param NUM_FILES - this is the number of files in the archive
*/
int verify(int argc, char **argv, hdr *hdr, int *NUM_FILES) {
    // Make sure the program was called correctly formatted
    if (argc > 2) {
        printerr(ERROR_USAGE, 0);
    }
    
    // Make sure the file can be opened/exists
    int tarfd = open(argv[1], O_RDONLY, 0644);
    if (tarfd == -1) {
        printerr(ERROR_TAR, argv[1]);
    }

    // Make sure that the file has a valid magic number
    read(tarfd, hdr, sizeof(*hdr));
    if (hdr->magic != MAGIC) {
        printerr(ERROR_TAR, argv[1]);
    }

    // *NUM_FILES = numfiles(tarfd, hdr);

    lseek(tarfd, 0, SEEK_SET);
    read(tarfd, hdr, sizeof(*hdr));
    int total = 0;
    while (1) {
        for (int i = 0; i < 4; i++) {
            if (hdr->file_name[i]) {
                verifyfile(tarfd, hdr->file_name[i]);
                total++;
            } else {
                *NUM_FILES = total;
                break;
            }
        }
        if (hdr->next) {
            lseek(tarfd, hdr->next, SEEK_SET);
            read(tarfd, hdr, sizeof(*hdr));
        } else {
            break;
        }
    }
    return tarfd;
}

/**
 * This function outputs all files in the archve
 *
 * Param TAR_FD - this is the fd for the archive
 * Param NUM_FILES - this is the number of files in the archive
 * Param hdr - this is the header struct to be used
 */ 
void outputfiles(const int TAR_FD, int NUM_FILES, hdr *hdr) {

    lseek(TAR_FD, 0, SEEK_SET);
    read(TAR_FD, hdr, sizeof(*hdr));

    char outputBuf[BUF_LEN];
    int currfd = 0;
    int currFileSize = 0;
    int currRemSize = 0;
    int currReadCount = 0;
    short int currNameLen = 0;
    for (int k = 0; k < NUM_FILES; k += 4) {
        for (int i = 0; hdr->file_name[i] && i < 4; i++) {
            if (!hdr->deleted[i]) {
                lseek(TAR_FD, hdr->file_name[i], SEEK_SET);
                read(TAR_FD, &currNameLen, 2);
                char currName[currNameLen + 1];
                read(TAR_FD, currName, currNameLen);
                currName[currNameLen] = 0;
                currfd = open(currName, O_WRONLY | O_CREAT, 0644);
                currFileSize = hdr->file_size[i];
                currRemSize = currFileSize;
                if (currFileSize <= BUF_LEN) {
                    read(TAR_FD, outputBuf, currFileSize);
                    write(currfd, outputBuf, currFileSize);
                } else {
                    currReadCount = read(TAR_FD, outputBuf, BUF_LEN);
                    while (currRemSize) {
                        currRemSize -= write(currfd, outputBuf, currReadCount);
                        if (currRemSize <= BUF_LEN) {
                            currReadCount = read(TAR_FD, outputBuf, currRemSize);
                        } else {
                            currReadCount = read(TAR_FD, outputBuf, BUF_LEN);
                        }
                    }
                }
            }
            close(currfd);
        }
        lseek(TAR_FD, hdr->next, SEEK_SET);
        read(TAR_FD, hdr, sizeof(*hdr));
    }
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
    int NUM_FILES;
    const int TAR_FD = verify(argc, argv, &hdr, &NUM_FILES);
    
    outputfiles(TAR_FD, NUM_FILES, &hdr);
    close(TAR_FD);
}
