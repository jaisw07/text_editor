//header file 'ctype' provides iscntrl() functionality
#include <ctype.h>
//header file 'stdio' provides printf() functionality
#include <stdio.h>
//header file 'stdlib' provides atexit() functionality
#include <stdlib.h>
//header file 'termios' provides struct termios, tcgetattr(), tcsetattr(), ECHO, TCSAFLUSH and VMIN, VTIME functionality
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
//the ICRNL flag is an input flag where I stands for Input, CR for Carriage Return and NL for New Line. It fixes Ctrl+M and Enter being read as 10 instead of 13(the terminal translates carriage returns into newlines(10, '\n')
//when BRKINT is turned on, a break condition will cause a termination signal to be sent to terminal
//INPCK enables parity checking(doesn't apply to modern emulators)
//ISTRIP sets the 8th bit of every byte to 0
	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
//the OPOST flag is an output processing flag, disabling it turns off the translation of every printed new line('\n') into a carriage return followed by a newline('\r\n'). The carriage return('\r') brings the cursor to the start of the current line and the newline('\n') moves the cursor down one line.
	raw.c_oflag &= ~(OPOST);
//CS8 is not a flag but a bit mask with multiple bits which is set using OR operator unlike the other flags which use AND operator. It sets CS(Character Size) to 8 bits per byte.
	raw.c_cflag |= ~(CS8);
//VMIN and VTIME are indexes into the c_cc(Control Characters) field, an array of bytes which control terminal settings
//VMIN sets min number of bytes of input needed before read() can return; 0 bytes mean that that read() returns as soon as any input is read
//VTIME sets max time to wait before read() returns in tenth of seconds; 1 means that read() will be returned after no input has been given for 100ms
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
//tcsetattr applies the changes to the terminal
//TCSAFLUSH argument specifies when the changes should be applies; here it waits for all pending output to be written to the terminal and also discards any input that has not been read
//TCSAFLUSH is the reason why any unread input(after q for example) is not fed as input to the terminal
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
	enableRawMode();
	while (1) {
		char c = '\0';
//read() returns the number of bytes read; it reads from STDIN_FILENO(terminal input) to char c and returns after nbytes=1 byte has been read i.e. returns after every single char input
//STDIN_FILENO points to the standard input file(input from terminal)
		read(STDIN_FILENO, &c, 1);
//iscntrl() tests whether character is a control char that is, it is a nonprintable character which we don't want to print on screen(ASCII 0-31 and 127)
		if(iscntrl(c)) {
			printf("%d\r\n", c);
		} else {
			printf("%d ('%c')\r\n", c, c);
		}
		if (c == 'q') {break;}
	}
	return 0;
}
