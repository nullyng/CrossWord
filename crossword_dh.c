#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <termio.h>
#include <fcntl.h>
#include <ctype.h>

void set_crmode();
void set_nodelay_mode(void);
void tty_mode(int);

void main(){
//	char m_start[] = "[1] GAME START";
//	char m_info[] = "[2] GAME INFO";
//	char m_exit[] = "[3] GAME EXIT";

	tty_mode(0); // save original mode
//	set_crmode(); // canonical mode OFF
//	set_nodelay_mode(); // nonblocking ON

	initscr();
	clear();
	
	attron(A_BOLD);

	move(5,5);
	printw(" __________________________________________________________________________");
	move(6,5);
	printw("|1   |2   |3   |    |4   |5   |6   |    |7   |8   |9   |    |10  |11  |12  |");
	move(7,5);
	printw("|    |    |    |    |    |    |    |    |    |    |    |    |    |    |    |");
	move(8,5);
	printw(" __________________________________________________________________________");

//	move(10, 10);
//	addstr(m_start);
//	move(11, 10);
//	addstr(m_info);
//	move(12, 10);
//	addstr(m_exit);
//	move(LINES-1, COLS-1);
	refresh();

	if(getchar() == 'q'){
		tty_mode(1);
		endwin();
		return;
	}
}

void set_crmode(){ // canonical mode OFF
	struct termios ttystate;

	tcgetattr(0, &ttystate);
	ttystate.c_lflag &= ~ICANON;
	ttystate.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &ttystate);
}

void set_nodelay_mode(){ // nonblocking mode
	int termflags;
	termflags = fcntl(0, F_GETFL);
	termflags |= O_NDELAY;
	fcntl(0, F_SETFL, termflags);
}

void tty_mode(int how){ // restore original mode
	static struct termios original_mode;
	static int original_flags;

	if(how == 0){
		tcgetattr(0, &original_mode);
		original_flags = fcntl(0, F_GETFL);
	}
	else{
		tcsetattr(0, TCSANOW, &original_mode);
		original_flags = fcntl(0, F_SETFL, original_flags);
	}
}
