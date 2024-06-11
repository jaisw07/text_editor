//header file 'ctype' provides iscntrl() functionality
#include <ctype.h>
//header file 'stdio' provides printf() functionality
#include <stdio.h>
//header file 'stdlib' provides atexit() functionality
#include <stdlib.h>
//header file 'termios' provides struct termios, tcgetattr(), tcsetattr(), ECHO and TCSAFLUSH functionality
#include <termios.h>
//header file 'unistd' provides read() and STDIN_FILENO functionality
#include <unistd.h>

//global variable orig_termios is used to store the current terminal attr to restore them once program ends
struct termios orig_termios;
void disableRawMode() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

//to prevent input only being sent on pressing Enter, we need to switch from cooked/canonical mode to raw mode
//enableRawMode stops echoing(printing/displaying what we type in the terminal)
//flags(aka options) are settings to enable/disable a specific feature of the terminal/CLI
void enableRawMode() {
//tcgetattr reads terminal attributes into our pre-defined termios struct(named 'orig_termios) to store current terminal attributes
	tcgetattr(STDIN_FILENO, &orig_termios);
//executes disableRawMode() when program ends; atexit is executed when program exits via main or via the exit() function
	atexit(disableRawMode);
//copies current terminal attributes to another termios struct(named 'raw') to make our changes on the copy without disturbing the original attributes so we can safely restore them once program exits
	struct termios raw = orig_termios;
//this line modifies the terminal attribute by accessing via raw(our termios struct)
//c_lflag field means 'local flags' : local flags control higher level properties input processing like echoing, choice of canonical/non-canonical input, etc. as compared to the input flags
//other flags like c_iflag(input flags), c_oflag(output flags) and c_cflag(control flags) will be modified too
//ECHO is a bitflag; bitflags are a way to represent multiple boolean values as a single integer where the number of boolean states stored per integer depends upon it's memory size; for eg, a 32 bit int can store 32 boolean states
//ECHO's 32 bit bitflag(...1000) is complemented(...0111) by using ~(binary NOT operator). then (&=)AND with the local flags, which forces the 4th bit in the flags to become 0 and the rest to remain as is
//This is an example of flipping bits in C
//the ICANON flag is a local flag used to turn off canonical mode; we can finally read input byte-by-byte rather than line-by-line
//the ISIG flag is a local flag used to turn off commands like Ctrl+C and Ctrl+Z which terminate and suspend running program respectively(also turns off Ctrl+Y command for MacOS which is similar to Ctrl+Z but waits for program to read input before suspending it)
//the IXTEN flag is a local flag used to turn off Ctrl+V command that sends the next input character back as a literal(also turns off Ctrl+O command which is discarded in MacOS otherwise)
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
//the IXON flag is an input flag used to turn off commands like Ctrl+S and Ctrl+Q used for software flow control. Ctrl+S stops data from being transmitted to terminal until Ctrl+Q is pressed.
//the ICRNL flag is an input flag where I stands for Input and CR for Carriage Return. It fixes Ctrl+M and Enter being read as 10 instead of 13(the terminal translates carriage returns into newlines(10, '\n')
	raw.c_iflag &= ~(IXON | ICRNL);
//tcsetattr applies the changes to the terminal
//TCSAFLUSH argument specifies when the changes should be applies; here it waits for all pending output to be written to the terminal and also discards any input that has not been read
//TCSAFLUSH is the reason why any unread input(after q for example) is not fed as input to the terminal
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
	enableRawMode();
	char c;
//read() returns the number of bytes read; it will return 0 when it reaches end of file
//STDIN_FILENO points to the standard input file(input fromn terminal)
//this while loop keeps reading 1 byte from std input to var c until there are no more bytes to read or if a 'q' is typed
	while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
//iscntrl() tests whether character is a control char that is, it is a nonprintable character which we don't want to print on screen(ASCII 0-31 and 127)
		if(iscntrl(c)) {
			printf("%d\n", c);
		} else {
			printf("%d ('%c')\n", c, c);
		}
	}
	return 0;
}
