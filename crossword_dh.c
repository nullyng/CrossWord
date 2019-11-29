#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <termio.h>
#include <fcntl.h>
#include <ctype.h>

void add_blank(int, int, int);
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
//                               6    11   16   21   26   31   36   41   46
	move(3,5);	printw("*--------------------------------------------*");
	move(4,5);	printw("|1   |    |2   |    |3   |    |4   |    |5   |"); add_blank(4,31,1);
	move(5,5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(5,31,1);
	move(6,5);	printw("----------------------------------------------");
	move(7,5);	printw("|    |    |    |    |6   |    |    |    |    |"); add_blank(7,11,1); add_blank(7,21,1); add_blank(7,41,1);
	move(8,5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(8,11,1); add_blank(8,21,1); add_blank(8,41,1);
	move(9,5);	printw("----------------------------------------------");
	move(10,5);	printw("|7   |    |    |    |    |    |8   |    |    |"); add_blank(10,31,1);
	move(11,5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(11,31,1);
	move(12,5);	printw("----------------------------------------------");
	move(13,5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(13,11,3); add_blank(13,31,1); add_blank(13,41,2);
	move(14,5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(14,11,3); add_blank(14,31,1); add_blank(14,41,2);
	move(15,5);	printw("----------------------------------------------");
	move(16,5);	printw("|9   |    |10  |    |    |11  |    |    |12  |"); add_blank(16,26,1);
	move(17,5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(17,26,1);
	move(18,5);	printw("----------------------------------------------");
	move(19,5);	printw("|    |    |    |    |13  |    |    |    |    |"); add_blank(19,6,2); add_blank(19,21,1); add_blank(19,31,3);
	move(20,5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(20,6,2); add_blank(20,21,1); add_blank(20,31,3);
	move(21,5);	printw("----------------------------------------------");
	move(22,5);	printw("|14  |    |    |    |15  |    |16  |    |    |"); add_blank(22,21,1);
	move(23,5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(23,21,1);
	move(24,5);	printw("----------------------------------------------");
	move(25,5);	printw("|    |    |17  |    |    |    |    |    |    |"); add_blank(25,11,1); add_blank(25,31,1); add_blank(25,41,1);
	move(26,5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(26,11,1); add_blank(26,31,1); add_blank(26,41,1);
	move(27,5);	printw("----------------------------------------------");
	move(28,5);	printw("|18  |    |    |    |19  |    |    |    |    |"); add_blank(28,21,1);
	move(29,5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(29,21,1);
	move(30,5);	printw("*--------------------------------------------*");

	move(3,56);	printw("Across");
	move(5,56);	printw("1. Cook on gridiron");
	move(6,56);	printw("4. Work with shovel");
	move(7,56);	printw("6. Belonging to us");
	move(8,56);	printw("7. Repeated another time");
	move(9,56);	printw("8. Small insect");
	move(10,56);	printw("9. 10cent coin");
	move(11,56);	printw("11. Frozen precipitation");
	move(12,56);	printw("14. Food tin");
	move(13,56);	printw("15. Grayish green");
	move(14,56);	printw("17. Hooting bird");
	move(15,56);	printw("18. Sticky black substance");
	move(16,56);	printw("19. A lollipop");

	move(3,87);	printw("Down");
	move(5,87);	printw("1. Protect");
	move(6,87);	printw("2. Northern lreland army");
	move(7,87);	printw("3. Not short");
	move(8,87);	printw("4. Empty liquid from container");
	move(9,87);	printw("5. Acquire");
	move(10,87);	printw("10. Not major");
	move(11,87);	printw("12. Fully of weeds");
	move(12,87);	printw("13. Tootsie ____");
	move(13,87);	printw("14. Feline");
	move(14,87);	printw("16. Sick");


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

void add_blank(int x, int y, int n){
	move(x,y);
	standout();
	for(int i = 0; i < n; i++){
		printw("    ");
		if(i != n-1){
			standend();
			printw("|");
			standout();
		}
	}
	standend();
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
