#include <stdio.h>
#include <ncurses.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>

int ws_row, ws_col;

void screen_demensions();
void set_cr_noecho_mode();
void tty_mode(int);

void player1();
void player2();

int main()
{
	char title[] = "Cross Word";
	char menu1[] = "1. game start";
	char menu2[] = "2. game exit";
	char choose1[] = "1. 1p";
	char choose2[] = "2. 2p";
	int key;

	tty_mode(0);
	set_cr_noecho_mode();
	screen_demensions();

	initscr();
	clear();

	move(ws_row/4, ws_col/2-strlen(title));
	attron(A_BOLD);
	addstr(title);
	attroff(A_BOLD);

	refresh();
	sleep(1);

	move(ws_row/2, ws_col/2-strlen(menu1));
	addstr(menu1);
	move(ws_row/2+2, ws_col/2-strlen(menu1));
	addstr(menu2);

	refresh();

	while(1)
	{
		key = getchar(); // getch -> noecho mode don't execute!

		if(key == 'q')
			break;
		else if(key == '1')
		{
			clear();
			move(ws_row/2-2, ws_col/2-strlen(choose1));
			addstr(choose1);
			move(ws_row/2+2, ws_col/2-strlen(choose1));
			addstr(choose2);
			refresh();
		}
		else if(key == '2')
		{
			endwin();
			echo();
			tty_mode(1);
			exit(0);
		}
	}

	endwin();
	tty_mode(1);
}

void screen_demensions()
{
	struct winsize wbuf;

	if(ioctl(0, TIOCGWINSZ, &wbuf) != -1)
	{
		ws_row = wbuf.ws_row;
		ws_col = wbuf.ws_col;
	}
}

void set_cr_noecho_mode()
{
	struct termios ttystate;

	tcgetattr(0, &ttystate);
	ttystate.c_lflag &= ~ICANON;
	ttystate.c_lflag &= ~ECHO;
	ttystate.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &ttystate);
}

void tty_mode(int how)
{
	static struct termios original_mode;

	if(how == 0)
		tcgetattr(0, &original_mode);
	else
		tcsetattr(0, TCSANOW, &original_mode);
}
