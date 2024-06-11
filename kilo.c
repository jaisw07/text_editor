//header file 'unistd' provides read() and STDIN_FILENO functionality
#include <unistd.h>

int main() {
char c;
//read() returns the number of bytes read; it will return 0 when it reaches end of file
//STDIN_FILENO points to the standard input file
//this while loop keeps reading 1 byte from std input to var c until there are no more bytes to read
while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
return 0;
}
