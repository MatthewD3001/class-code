#include <stdio.h>
#include <unistd.h>

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

int main() {
    unsigned char c;
    int byte_val;
    int bitval;

    //while((byte_val = read_byte()) != 256) {
    //    write_byte(byte_val);
    //}
    //write_byte(256);

    while((bitval = read_bit()) != 2) {
        write_bit(bitval);
    }
    write_byte(256);

    // assign bit value to bitval
    /*for(int i = 7; i >= 0; i--) {
        bitval = (c >> i) & 1;
        printf("%d ", bitval);
    }*/
    //printf("\n");


    /*
    while(c = read_bit()) {
        write_bit(c);
    }*/
    return 0;
}
