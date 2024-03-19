#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DICLEN 16
#define TRUE 1
#define FALSE 0

int main() {
    char dic[DICLEN] = {0};
    char dicFreq[DICLEN] = {0};
    char cur;
    char inDic = FALSE;
    short int readLen = read(0, &cur, 1);
    for (int i = 1; i < DICLEN; i++) {
        if (readLen) {
            if (cur) {
                dic[i] = cur;
                dicFreq[i]++;
                readLen = read(0, &cur, 1);
                while (readLen && cur == dic[i]) {
                    dicFreq[i]++;
                    readLen = read(0, &cur, 1);
                }
            } else {
                readLen = read(0, &cur, 1);
            }
        } else {
            dic[i] = 0;
        }
    }
    /*
    while (readLen) {
        if (cur) {
            for (int i = 1; i < DICLEN; i++) {
                if (dic[i] == cur) {
                    dicFreq[i]++;
                    inDic = TRUE;
                    break;
                }
            }
            if (!inDic) {
                
            }
            inDic = FALSE;
        }
        readLen = read(0, &cur, 1);
    }
    */
    write(1, dic, DICLEN);
    return 0;
}
