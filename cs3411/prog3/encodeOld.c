#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#define DICLEN 15
#define TRUE 1
#define FALSE 0

_Bool inDictionary(char dic[DICLEN], char cur) {
    for (int i = 0; i < DICLEN; i++) {
        if (cur == dic[i]) { return TRUE; }
    }
    return FALSE;
}

void shift(char dic[DICLEN], int dicFreq[DICLEN], int index, char insert, int insertFreq) {
    if (index < DICLEN) {
        char tmp = dic[index];
        int tmpFreq = dicFreq[index];
        dic[index] = insert;
        dicFreq[index] = insertFreq;
        shift(dic, dicFreq, index + 1, tmp, tmpFreq);
    }
    return;
}

void orgDictionary(char dic[DICLEN], int dicFreq[DICLEN], char insert, int insertFreq) {
    for (int i = 0; i < DICLEN; i++) {
        if (!dic[i]) {
            dic[i] = insert;
            dicFreq[i] = insertFreq;
            return;
        } else if (insertFreq > dicFreq[i]) {
            shift(dic, dicFreq, i, insert, insertFreq);
            return;
        } else if (insertFreq == dicFreq[i]) {
            if (insert < dic[i]) {
                shift(dic, dicFreq, i, insert, insertFreq);
            } else {
                for (int j = i + 1; j < DICLEN; j++) {
                    if (!dic[j]) {
                        dic[j] = insert;
                        dicFreq[j] = insertFreq;
                        break;
                    }
                    if (insert < dic[j]) {
                        shift(dic, dicFreq, j, insert, insertFreq);
                        break;
                    }
                }
            }
            return;
        }
    }
}

void makeDictionary(int FLEN, char dic[DICLEN]) {
    int dicFreq[DICLEN] = {0};
    char cur;
    int curFreq;
    char tmp;

    for (int i = 0; i < FLEN; i++) {
        lseek(0, i, SEEK_SET);
        read(0, &cur, 1);
        lseek(0, 0, SEEK_SET);
        if (cur) {
            if (!inDictionary(dic, cur)) {
                curFreq = 1;
                for (int j = 0; j < FLEN; j++) {
                    read(0, &tmp, 1);
                    if (cur == tmp && j != i) { curFreq++; }
                }
                orgDictionary(dic, dicFreq, cur, curFreq);
            }
            //printf("\npass #%d, searching for %c, frequency counted: %d", i, cur, curFreq);
        }
    }
}

int main() {
    char dic[DICLEN] = {0};
    struct stat file;
    if (fstat(0, &file) == -1) {return 1;}

    makeDictionary(file.st_size, dic);
    write(1, dic, DICLEN);
    return 0;
}
