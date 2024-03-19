#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#define DICLEN 16
#define TRUE 1
#define FALSE 0

_Bool inDictionary(char dic[DICLEN], char cur) {
    for (int i = 1; i < DICLEN; i++) {
        if (cur == dic[i]) { return TRUE; }
    }
    return FALSE;
}

void orgDictionary(char dic[DICLEN], int dicFreq[DICLEN], char insert, short int insertFreq) {
    for (int i = 1; i < DICLEN; i++) {
        if (!dic[i]) {
            dic[i] = insert;
            dicFreq[i] = insertFreq;
            return;
        } else if (insertFreq > dicFreq[i]) {
            char cur = dic[i];
            int curFreq = dicFreq[i];
            dic[i] = insert;
            dicFreq[i] = insertFreq;
            for (int j = i + 1; j < DICLEN; j++) {
                char tmp = dic[j];
                int tmpFreq = dicFreq[j];
                dic[j] = cur;
                dicFreq[j] = curFreq;
                cur = tmp;
                curFreq = tmpFreq;
            }
            return;
        } else if (insertFreq == dicFreq[i]) {
            if (insert < dic[i]) {
                char cur = dic[i];
                int curFreq = dicFreq[i];
                dic[i] = insert;
                dicFreq[i] = insertFreq;
                for (int j = i + 1; j < DICLEN; j++) {
                    char tmp = dic[j];
                    int tmpFreq = dicFreq[j];
                    dic[j] = cur;
                    dicFreq[j] = curFreq;
                    cur = tmp;
                    curFreq = tmpFreq;
                }
            } else {
                for (int j = i + 1; j < DICLEN; j++) {
                    if (insert < dic[j]) {
                        char cur = dic[j];
                        int curFreq = dicFreq[j];
                        dic[j] = insert;
                        dicFreq[j] = insertFreq;
                        for (int k = j + 1; k < DICLEN; k++) {
                            char tmp = dic[k];
                            int tmpFreq = dicFreq[k];
                            dic[k] = cur;
                            dicFreq[k] = curFreq;
                            cur = tmp;
                            curFreq = tmpFreq;
                        }
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

    for (int i = 1; i <= FLEN; i++) {
        lseek(0, i - 1, SEEK_SET);
        read(0, &cur, 1);
        lseek(0, 0, SEEK_SET);
        if (cur) {
            if (!inDictionary(dic, cur)) {
                curFreq = 1;
                for (int j = 1; j < FLEN; j++) {
                    read(0, &tmp, 1);
                    if (cur == tmp && j != i) { curFreq++; }
                }

            }
            printf("pass #%d, searching for %c, frequency counted: %d\n", i, cur, curFreq);
            orgDictionary(dic, dicFreq, cur, curFreq);
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
