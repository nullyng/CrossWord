#include <stdio.h>
#include <ncurses.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define HOSTNAME 123 
#define PORT 123

int ws_row, ws_col;

void screen_demensions();
void set_cr_noecho_mode();
void tty_mode(int);

void player1();
void player2();

int main()
{
	char title[] = "Cross Word";
	char *menu[] = {"1. game start","2. game exit"};
	//char menu2[] = "2. game exit";
	char *choose[] = {"1. 1p","2. 2p"};
	int key, cur, dir_r,dir_c,i;

	tty_mode(0);
	set_cr_noecho_mode();
	screen_demensions();

	initscr();
	clear();

	if(has_colors())
		start_color();
	init_pair(1,COLOR_BLACK,COLOR_YELLOW);

	move(ws_row/4, ws_col/2-strlen(title));
	attron(COLOR_PAIR(1));
	addstr(title);
	attroff(COLOR_PAIR(1));
	move(LINES-1,COLS-1);

	refresh();
	sleep(1);

	keypad(stdscr,TRUE);
	dir_r = ws_row/2;
	dir_c = ws_col/2-strlen(menu[1]);
	cur = ws_row/2;

	while(1)
	{

		for(i = 0;i<2;i++){
			move(dir_r+i*2,dir_c);
			if(i*2+dir_r == cur) attron(A_REVERSE);
			printw("%s",menu[i]);
			attroff(A_REVERSE);
			move(LINES-1, COLS -1);
			refresh();
		}

		
		switch(key = getch()){
			case KEY_DOWN:
				if(cur == dir_r) cur+=2;
				break;
			case KEY_UP:
				if(cur == dir_r+2) cur-=2;
				break;
			default:
				break;
		}

		if(key == 10)
			break;

	}
	clear();

	if(cur == dir_r){
		dir_r -=2;
		dir_c = ws_col/2-strlen(choose[1]);
		cur = dir_r;

			while(1)
			{

				for(i = 0;i<2;i++)
				{
					move(dir_r+i*4,dir_c);
					if(i*4+dir_r == cur) attron(A_REVERSE);
					printw("%s",choose[i]);
					attroff(A_REVERSE);
					move(LINES-1,COLS-1);
					refresh();
				}

				switch(key = getch()){
					case KEY_DOWN:
						if(cur == dir_r) cur += 4;
						break;
					case KEY_UP:
						if(cur == dir_r+4) cur -=4;
						break;
					default:
						break;
				}
				if(key == 10)
					break;
			}		
	}
	else
	{
		endwin();
		tty_mode(1);
		exit(0);
	}	

	if(cur = dir_r+4)
		player2();


	endwin();
	tty_mode(1);
}

void player2()
{
	struct sockaddr_in servadd;
	struct hostent *hp;
	int sock_id;
	char message[BUFSIZ];
	int messlen;

	clear();
	
	sock_id = socket(AF_INET,SOCK_STREAM,0);
	if(sock_id == -1)
		oops("socket");
	
	memset(&servadd, 0,sizeof(servadd));
	servadd.sin_family = AF_INET;
	servadd.sin_addr.s_addr = inet_addr(HOSTNAME);
	servadd.sin_port = htons(PORT);

	if(connect(sock_id, (struct sokaddr *)&servadd, sizeof(servadd))!=0)
		oops("connect");

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
