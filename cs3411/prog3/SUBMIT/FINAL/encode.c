#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

#define DICLEN 15

int read_byte() {
    unsigned char c;
    static char buf[256];
    static int buf_count = 0;
    static int buf_index = 0;

    if (buf_count == 0) {
        buf_count = read(0, buf, sizeof(buf));
        buf_index = 0;
    }

    // EOF
    if (buf_count == 0) {
        return 256;
    }

    c = buf[buf_index];
    buf_index++;
    buf_count--;

    return c;
}

int write_byte(int input) {
    //char c;
    static char buf[256];
    static int buf_count = 0;
    static int buf_index = 0;

    if (input == 256) {
        write(1, buf, buf_count);
        buf_count = 0;
        buf_index = 0;
        return 0;
    }

    if (buf_count < 256) {
        buf[buf_index] = (char) input;
        buf_index++;
        buf_count++;
    }

    // buffer is full, flush to file
    if (buf_count == 256) {
        write(1, buf, 256);
        buf_count = 0;
        buf_index = 0;
    }

    return 0;
}

int read_bit() {
    static unsigned char c;
    static int bit_count = 0;
    int retcheck;

    if (bit_count == 0) {
        retcheck = read_byte();
        if (retcheck == 256) {
            return 2;
        }
        c = (unsigned char) retcheck;
        bit_count = 8;
    }

    int bit = (c >> (bit_count-1)) & 1;
    bit_count--;

    return bit;
}

int write_bit(int bit) {
    static unsigned char c = 0;
    static int bit_count = 0;

    if (bit == 2) { // flush out existing byte
        while(bit_count < 8) {
            c = (c << 1) | 0;
            bit_count++;
        }
        write_byte(c);
        bit_count = 0;
        return 0;
    }

    c = (c << 1) | (bit & 1);
    bit_count++;

    if(bit_count == 8) {
        write_byte(c);
        bit_count = 0;
    }

    return 0;
}

/**
* This method shifts all the elements in the array to the right by 1 position and inserts the given byte in the location.
* 
* Param dic: This is the dictionary array.
* Param byte: This is the byte to be inserted.
* Param index: where the byte should be inserted.
*/
void shift(unsigned char dic[DICLEN], unsigned char byte, int index) {
    if (index < DICLEN) {
        unsigned char tmp = dic[index];
        dic[index] = byte;
        shift(dic, tmp, index + 1);
    }
    return;
}

/**
* This method creates the dictionary after the frequency array has been made.
* 
* Param dic: This is the dictionary array.
* Param freq: This is the array of all byte values and their frequencies in the file.
*/
void makeDictionary(unsigned char dic[DICLEN], int freq[256]) {
    unsigned char byte = 1;
    do {
        if (freq[byte] < 1) {
            continue;
        }
        for (int i = 0; i < DICLEN; i++) {
            if (!dic[i]) {
                dic[i] = byte;
                break;
            } else if (freq[byte] > freq[dic[i]]) {
                shift(dic, byte, i);
                break;
            } else if (freq[byte] == freq[dic[i]]) {
                if (byte < dic[i]) {
                    shift(dic, byte, i);
                    break;
                } else {
                    for (int j = i + 1; j < DICLEN; j++) {
                        if (!dic[j]) {
                            dic[j] = byte;
                            break;
                        }
                        if (byte < dic[j]) {
                            shift(dic, byte, j);
                            break;
                        }
                    }
                    break;
                }
            }
        }
    } while (byte++ < 255);
    return;
}

/**
* This method checks to see if a given element is in the dictionary, if so it returns its index.
* 
* Param dic: This is the dictionary array.
* Param byte: This is the byte to be checked.
*
* Return int: The index where the byte resides in the dictionary + 1 for encode. -1 is returned if the byte is not found. 
*/
int inDictionary(unsigned char dic[DICLEN], unsigned char byte) {
    for (int i = 0; i < DICLEN; i++) {
        if (dic[i] == byte) {
            return i + 1;
        }
    }
    return -1;
}

/**
* This method checks how many sequntial bytes up to 4 there are of a given byte.
* 
* Param byte: This is the byte to be checked.
*
* Return int: The number of sequential bytes. NOTE: this is exclusive of the first byte.
*/
int check_run(unsigned char byte) {
    int run = 0;
    unsigned char next;
    for (int i = 0; i < 3; i++) {
        if (!read(0, &next, 1)) {
            return run;
        }
        if (next == byte) {
            run++;
        } else {
            lseek(0, -1, SEEK_CUR);
            return run;
        }
    }
    return run;
}

int main() {
    struct stat file;
    if (fstat(0, &file) == -1) { return 1; }    // Make sure file is valid.
    unsigned char dic[DICLEN] = {0};
    int freq[256] = {0};                        // The array to hold all byte frequencies.
    unsigned char cur_byte;
    for (int i = 0; i < file.st_size; i++) {
        read(0, &cur_byte, 1);
        freq[cur_byte]++;
    }
    makeDictionary(dic, freq);
    write(1, dic, DICLEN);                      // Write the dictionary to the file.

    lseek(0, 0, SEEK_SET);                      // Back to the beginning to begin the encode.
    int index;
    for (int i = 0; i < file.st_size; i++) {    // For each byte in the file, attempt to encode it.
        read(0, &cur_byte, 1);
        index = inDictionary(dic, cur_byte);
        if (index != -1) {                      // Encode the frequent byte accordingly.
            write_bit(1);
            write_bit(1);
            int byte_run = check_run(cur_byte);
            i += byte_run;
            write_bit((byte_run >> 1) & 1);
            write_bit((byte_run) & 1);
            for (int j = 3; j >= 0; j--) {
                write_bit((index >> j) & 1);
            }
        } else if (cur_byte == 0) {             // Encode the zero byte accordingly.
            write_bit(1);
            write_bit(0);
        } else {                                // Else encode the infrequent byte accordingly.
            write_bit(0);
            for (int j = 7; j >= 0; j--) {
                write_bit((cur_byte >> j) & 1);
            }
        }
    }

    // Write the EOF encode
    write_bit(1);
    write_bit(1);
    write_bit(0);
    write_bit(0);
    write_bit(0);
    write_bit(0);
    write_bit(0);
    write_bit(0);
    
    // Flush the remaning byte and then flush the unwritten bytes to the file.
    write_bit(2);
    write_byte(256);
    return 0;
}
