#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFLEN 256

/**
 * This is a helper function which determines if a character is printable or not.
 *
 * Param: c char to be check if it is printable or not
 * Return: An int to be interpretted as a bool.
 */
int isPrintable(unsigned char c) {
    if (c > 31 && c < 127) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * This is a helper function which will translate the given buffer to the output
 * ensuring all characters are printable or represented as hex.
 *
 * Param: fd the file descriptor to write this translation to
 * Param: buf[BUFLEN] The buffer to be translated
 * Param: len The length of the buffer to translate. NOTE: Do not exceed BUFLEN
 */
void translate(int fd, unsigned char buf[BUFLEN], int len) {
    char hex[5]; // 4 + 1 for null byte of sprintf
    for (int i = 0; i < len; i++) {
        if (isPrintable(buf[i])) {
            write(fd, &buf[i], 1);
        } else {
            sprintf(hex, "<%02x>", buf[i]);
            write(fd, hex, 4);
        }
    }
}

int main(int argc, char **argv) {
    int pipe_out[2];                    // Create arrays to store pipe read and write fds
    int pipe_err[2];
    pipe(pipe_out);                     // Create the pipes
    pipe(pipe_err);
    
    if (fork() == 0) {                  // Begin child process
        close(1);                       // Close the child's stdout and stderr
        close(2);
        dup2(pipe_out[1], 1);           // Duplicate the corresponding pipe write ends to child's stdout and stderr
        dup2(pipe_err[1], 2);
        close(pipe_out[1]);             // Close the uneeded pipe fds
        close(pipe_err[1]);
        execvp(argv[2], &argv[2]);      // Begin given command
    }
    close(pipe_out[1]);      // Close the uneeded pipe fds
    close(pipe_err[1]);

    char out_name[strlen(argv[1]) + 8]; // Make the char arrays to hold the name + corresponding file extension + null byte
    char err_name[strlen(argv[1]) + 8];
    int out_fd = 0;                     // Declare stdout and stderr file fds for later
    int err_fd = 0;

    int status;                         // Declare values to be used in the following `select()` loop
    unsigned char buf[BUFLEN] = {0};
    int rc_out = 0;
    int rc_err = 0;

    fd_set in_fd;                       // Declare the fd set to be used with the `select()`

    for (;;) {
        FD_ZERO(&in_fd);                                            // Zero out the set to reset it
        FD_SET(pipe_out[0], &in_fd);                                // Insert the read ends of the pipe to the set
        FD_SET(pipe_err[0], &in_fd);
        status = select(pipe_err[0]+1, &in_fd, NULL, NULL, NULL);   // Wait for set fds to be readable, waiting for child to output something
        if (status < 0) {
            write(1, "Select error!\n", 14);
            exit(0);
        }
        if (status > 0 && FD_ISSET(pipe_out[0], &in_fd)) {          // If something is readable and the stdout pipe is still in the set, print 256 bytes from this pipe
            rc_out = read(pipe_out[0], buf, BUFLEN);
            if (rc_out) {
                if (!out_fd) {
                    sprintf(out_name, "%s.stdout", argv[1]);
                    out_fd = open(out_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }
                translate(1, buf, rc_out);
                write(out_fd, buf, rc_out);
            }
        }
        if (status > 0 && FD_ISSET(pipe_err[0], &in_fd)) {          // If something is readable and the stderr pipe is still in the set, print 256 bytes from this pipe
            rc_err = read(pipe_err[0], buf, BUFLEN);
            if (rc_err) {
                if (!err_fd) {
                    sprintf(err_name, "%s.stderr", argv[1]);
                    err_fd = open(err_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }
                translate(2, buf, rc_err);
                write(err_fd, buf, rc_err);
            }
        }
        if (!rc_out && !rc_err) {                                   // If both pipes are at EOF exit the program, nothing more to be read
            break;
        }
    }
    return 0;
}
