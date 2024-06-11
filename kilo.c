//header file 'termios' provides struct termios, tcgetattr(), tcsetattr(), ECHO and TCSAFLUSH functionality
#include <termios.h>
//header file 'unistd' provides read() and STDIN_FILENO functionality
#include <unistd.h>

//to prevent input only being sent on pressing Enter, we need to switch from cooked/canonical mode to raw mode
//enableRawMode stops echoing(printing/displaying what we type in the terminal)
//flags(aka options) are settings to enable/disable a specific feature of the terminal/CLI
void enableRawMode() {
	struct termios raw;
//tcgetattr reads terminal attributes into our pre-defined termios struct(named 'raw') to store current terminal attributes
	tcgetattr(STDIN_FILENO, &raw);
//this line modifies the terminal attribute by accessing via raw(our termios struct)
//c_lflag field means 'local flags' : local flags control higher level properties input processing like echoing, choice of canonical/non-canonical input, etc. as compared to the input flags
//other flags like c_iflag(input flags), c_oflag(output flags) and c_cflag(control flags) will be modified too
//ECHO is a bitflag; bitflags are a way to represent multiple boolean values as a single integer where the number of boolean states stored per integer depends upon it's memory size; for eg, a 32 bit int can store 32 boolean states
//ECHO's 32 bit bitflag(...1000) is complemented(...0111) by using ~(binary NOT operator). then (&=)AND with the local flags, which forces the 4th bit in the flags to become 0 and the rest to remain as is
//This is an example of flipping bits in C
	raw.c_lflag &= ~(ECHO);
//tcsetattr applies the changes to the terminal
//TCSAFLUSH argument specifies when the changes should be applies; here it waits for all pending output to be written to the terminal and also discards any input that has not been read
//TCSAFLUSH is the reason why any unread input(after q for example) is not fed as input to the terminal
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
	enableRawMode();
	char c;
//read() returns the number of bytes read; it will return 0 when it reaches end of file
//STDIN_FILENO points to the standard input file
//this while loop keeps reading 1 byte from std input to var c until there are no more bytes to read
	while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
	return 0;
}
