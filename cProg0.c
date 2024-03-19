#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 128

_Bool checkChar(int current_char, _Bool *inWord);
void checkIncrement(int *word_count, _Bool *inWord);

int main() {

    char buffer[BUFSIZE];           // Creating the line buffer, where the current line is stored
    char *input_string = NULL;      // Initializing the pointer to memory for the input string
    size_t input_string_len = 0;    // The current size of the input string

    while (fgets(buffer, BUFSIZE, stdin)) {                                     // While there is still characters being read in from the user
        size_t line_len = strlen(buffer);                                       // Length of the currently read line is saved
        char *temp = realloc(input_string, input_string_len + line_len + 1);    // Temp char array/pointer to memory is initialized to the starting address of the input string to +1 after the full input string
        if (!temp) {                                                            // If temp is equal to 0 then we are out of memory
            // Error: out of memory
            break;
        }
        input_string = temp;                                                    // Reassign the input string to the new temp string 
        memcpy(input_string + input_string_len, buffer, line_len + 1);          // Copy the buffer onto the enod of the current string
        input_string_len += line_len;                                           // Total string length is increased based on currently read string
    }
    
    int current_char;
    int word_count = 0;
    _Bool inWord = 1;

    for (unsigned i = 0; i <= input_string_len; i++) {
        current_char = input_string[i];
        if (checkChar(current_char, &inWord)) {
            checkIncrement(&word_count, &inWord);
        }
    }

    printf("\nWords Counted: %i\n", word_count);

    free(input_string);

    return 0;
}

_Bool checkChar(int current_char, _Bool *inWord) {
    if (current_char < 65) {
        if (current_char < 58 && current_char > 47) {
            return 0;
        }
        switch (current_char) {
        case 45:
            break;
        
        case 39:
            break;

        default:
            return 1;
        }
        return 0;
    }
    else if (current_char > 90 && current_char < 97) {
        return 1;
    }
    else if (current_char > 122) {
        return 1;
    }
    else {
        *inWord = 1;
    }
    return 0;
}

void checkIncrement(int *word_count, _Bool *inWord) {
    if (*inWord) {
        *word_count += 1;
        *inWord = 0;
    }
}
