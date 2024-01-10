#include <stdio.h>

int mystrlen(char *input) {
	char *begin = input;
	while(*input++);
	return --input - begin;
}

int mystrlenold(char *input) {
	int i = 0;
	while(*input++)
		i++;
	return i;
}

int main() {
	char *string = "Hello world!";
	printf("String: %s Length: %d\n", string, mystrlenold(string));
}

