#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#define DICLEN 15

void shift(unsigned char dic[DICLEN], unsigned char byte, int index) {
    if (index < DICLEN) {
        unsigned char tmp = dic[index];
        dic[index] = byte;
        shift(dic, tmp, index + 1);
    }
    return;
}

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

int main() {
    struct stat file;
    if (fstat(0, &file) == -1) { return 1; } // Make sure file is valid

    unsigned char dic[DICLEN] = {0};
    int freq[256] = {0};
    unsigned char cur_byte;
    for (int i = 0; i < file.st_size; i++) {
        read(0, &cur_byte, 1);
        freq[cur_byte]++;
    }
    makeDictionary(dic, freq);
    write(1, dic, DICLEN);

    // TODO Write the encoded file.


    return 0;
}
