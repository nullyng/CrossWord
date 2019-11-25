#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <ncurses.h>
#include <sys/ioctl.h>

int ws_row, ws_col;
int ws_xpixel, ws_ypixel;

void screen_demensions();
void set_cr_noecho_mode();
void tty_mode(int);

int main()
{
	int i, row = 10, col = 30, standout_check = 1;
	char start_comment[] = "Cross Word";
	char menu1[] = "1. game start";
	char menu2[] = "2. game exit";
	char choose1[] = "1. 1p";
	char choose2[] = "2. 2p";
	int key;

	tty_mode(0);
	screen_demensions();
       	start_color();	
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	
	initscr();
	clear();

	move(ws_row/4, ws_col/2-strlen(start_comment));
	
	//attron(COLOR_PAIR(1));
//	attron(A_BOLD);
	chgat(-1, A_REVERSE, 0, NULL);
	printw("%s", start_comment);
//	attroff(A_BOLD);
	//attroff(COLOR_PAIR(1));


	/*for(i=0; i<strlen(start_comment); i++)
	{
		if(standout_check == 1)
			standout();

		move(row, col);
		addch(start_comment[i]);

		if(start_comment[i] == ' ')
			col += 4;
		else
			col += 2;

		if(standout_check == 1)
			standend();

		standout_check *= -1;
	}*/
	
	refresh();
	sleep(1);

	move(ws_row/2, ws_col/2-strlen(menu1));
	addstr(menu1);
	move(ws_row/2+2, ws_col/2-strlen(menu1));
	addstr(menu2);

	refresh();

	while((key = getchar()) != NULL)
	{
		if(key == 'q')
		{
			endwin();
			break;
		}
		
		if(key == '1')
		{
			clear();
			move(ws_row/2-2, ws_col/2-strlen(choose1));
			addstr(choose1);
			move(ws_row/2+2, ws_col/2-strlen(choose1));
			addstr(choose2);
			refresh();
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
		ws_xpixel = wbuf.ws_xpixel;
		ws_ypixel = wbuf.ws_ypixel;
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
