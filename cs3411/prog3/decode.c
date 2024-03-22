#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

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
    char c;
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

void write_eof() {
    printf("Reached the end of the file.\n");
    // TODO implement eof writing.
}

void write_encoded(unsigned char dic[DICLEN]) {
    int count_bits[2] = {0};
    count_bits[1] = read_bit();
    count_bits[0] = read_bit();
    int count = ((count_bits[1] * 2) + count_bits[0] + 1); // `count` now holds the number of the encoded byte to write.
    
    int index_bits[4] = {0};
    index_bits[3] = read_bit();
    index_bits[2] = read_bit();
    index_bits[1] = read_bit();
    index_bits[0] = read_bit();
    int index = ( index_bits[0] +     
                (2 * index_bits[1]) +
                (4 * index_bits[2]) +
                (8 * index_bits[3]) - 1 // NOTE decrementing the index by one to account for dictionary offset and EOF condition.
                );
    if (index == -1) {
        write_eof();
        return;
    }

    for (int i = 0; i <= count; i++) {
        // TODO Write the byte in the dictionary at the indicated `index` `count` times.
        printf("Writing encoded byte: %c\n", dic[index]);
    }
}

void write_zero() {
    printf("Writing zero byte.\n");
}

void write_infrequent() {
    printf("Writing infrequent byte.\n");
}

int main() {

    unsigned char dic[DICLEN] = {0};
    read(0, dic, DICLEN);

    int bit_read = read_bit();
    while (bit_read != 2) {
        if (bit_read) {
            bit_read = read_bit();
            if (bit_read) {
                write_encoded(dic);
            } else {
                write_zero();
            }
        } else {
            write_infrequent();
        }
        bit_read = read_bit();
    }
    // EOF
    return 0;
}
