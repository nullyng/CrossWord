#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <termio.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

void crossword_base();
void add_blank(int, int, int);
void clear_box();

void select_action_page();
void select_across_down_page();
void add_page(int);
void add_across(int, int,  char*);
void add_down(int, int,  char*);
void delete();
void submit();
void exit_puzzle();
void set_crmode();
void set_nodelay_mode(void);
void tty_mode(int);

void main(){
	tty_mode(0); // save original mode
	set_crmode(); // canonical mode OFF
	//	set_nodelay_mode(); // blocking OFF

	initscr();
	clear();

	crossword_base();
	select_action_page();

	

	if(getchar()){
		tty_mode(1);
		endwin();
		return;
	}
}

void crossword_base() {
	//                       6    11   16   21   26   31   36   41   46
	move(3, 5);	printw("*--------------------------------------------*");
	move(4, 5);	printw("|1   |    |2   |    |3   |    |4   |    |5   |"); add_blank(4, 31, 1);
	move(5, 5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(5, 31, 1);
	move(6, 5);	printw("----------------------------------------------");
	move(7, 5);	printw("|    |    |    |    |6   |    |    |    |    |"); add_blank(7, 11, 1); add_blank(7, 21, 1); add_blank(7, 41, 1);
	move(8, 5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(8, 11, 1); add_blank(8, 21, 1); add_blank(8, 41, 1);
	move(9, 5);	printw("----------------------------------------------");
	move(10, 5);	printw("|7   |    |    |    |    |    |8   |    |    |"); add_blank(10, 31, 1);
	move(11, 5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(11, 31, 1);
	move(12, 5);	printw("----------------------------------------------");
	move(13, 5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(13, 11, 3); add_blank(13, 31, 1); add_blank(13, 41, 2);
	move(14, 5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(14, 11, 3); add_blank(14, 31, 1); add_blank(14, 41, 2);
	move(15, 5);	printw("----------------------------------------------");
	move(16, 5);	printw("|9   |    |10  |    |    |11  |    |    |12  |"); add_blank(16, 26, 1);
	move(17, 5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(17, 26, 1);
	move(18, 5);	printw("----------------------------------------------");
	move(19, 5);	printw("|    |    |    |    |13  |    |    |    |    |"); add_blank(19, 6, 2); add_blank(19, 21, 1); add_blank(19, 31, 3);
	move(20, 5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(20, 6, 2); add_blank(20, 21, 1); add_blank(20, 31, 3);
	move(21, 5);	printw("----------------------------------------------");
	move(22, 5);	printw("|14  |    |    |    |15  |    |16  |    |    |"); add_blank(22, 21, 1);
	move(23, 5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(23, 21, 1);
	move(24, 5);	printw("----------------------------------------------");
	move(25, 5);	printw("|    |    |17  |    |    |    |    |    |    |"); add_blank(25, 11, 1); add_blank(25, 31, 1); add_blank(25, 41, 1);
	move(26, 5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(26, 11, 1); add_blank(26, 31, 1); add_blank(26, 41, 1);
	move(27, 5);	printw("----------------------------------------------");
	move(28, 5);	printw("|18  |    |    |    |19  |    |    |    |    |"); add_blank(28, 21, 1);
	move(29, 5);	printw("|    |    |    |    |    |    |    |    |    |"); add_blank(29, 21, 1);
	move(30, 5);	printw("*--------------------------------------------*");

	attron(A_BOLD | A_UNDERLINE);
	move(3, 56);	printw("Across");
	attroff(A_BOLD | A_UNDERLINE);
	move(5, 56);	printw("1. Cook on gridiron");
	move(6, 56);	printw("4. Work with shovel");
	move(7, 56);	printw("6. Belonging to us");
	move(8, 56);	printw("7. Repeated another time");
	move(9, 56);	printw("8. Small insect");
	move(10, 56);	printw("9. 10cent coin");
	move(11, 56);	printw("11. Frozen precipitation");
	move(12, 56);	printw("14. Food tin");
	move(13, 56);	printw("15. Grayish green");
	move(14, 56);	printw("17. Hooting bird");
	move(15, 56);	printw("18. Sticky black substance");
	move(16, 56);	printw("19. A lollipop");

	attron(A_BOLD | A_UNDERLINE);
	move(3, 87);	printw("Down");
	attroff(A_BOLD | A_UNDERLINE);
	move(5, 87);	printw("1. Protect");
	move(6, 87);	printw("2. Northern lreland army");
	move(7, 87);	printw("3. Not short");
	move(8, 87);	printw("4. Empty liquid from container");
	move(9, 87);	printw("5. Acquire");
	move(10, 87);	printw("10. Not major");
	move(11, 87);	printw("12. Fully of weeds");
	move(12, 87);	printw("13. Tootsie ____");
	move(13, 87);	printw("14. Feline");
	move(14, 87);	printw("16. Sick");

	attron(A_BOLD);
	move(19, 56);	printw("*************************************************************");
	move(20, 56);	printw("*                                                           *");
	move(21, 56);	printw("*                                                           *");
	move(22, 56);	printw("*                                                           *");
	move(23, 56);	printw("*                                                           *");
	move(24, 56);	printw("*                                                           *");
	move(25, 56);	printw("*                                                           *");
	move(26, 56);	printw("*                                                           *");
	move(27, 56);	printw("*                                                           *");
	move(28, 56);	printw("*                                                           *");
	move(29, 56);	printw("*                                                           *");
	move(30, 56);	printw("*************************************************************");
	attroff(A_BOLD);

	move(LINES - 1, COLS - 1);
	refresh();
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

void select_action_page(){
	int key;
	int selection = 1;
	char *selections[] = {"[1] Add",
		"[2] Delete",
		"[3] Submit",
		"[4] Exit"};

	keypad(stdscr, TRUE);
	while(1){
		for(int i = 0; i < 4; i++){
			move(23+i,80);
			if(i + 1 == selection)
				attron(A_REVERSE);
			printw("%s", selections[i]);
			attroff(A_REVERSE);
			move(LINES-1, COLS-1);
			refresh();
		}

		switch(key = getch()){
			case KEY_DOWN:
				if(++selection == 5) selection = 4;
				break;
			case KEY_UP:
				if(--selection == 0) selection = 1;
				break;
			default:
				break;
		}
		if(key == 10)
			break;
	}

	if(selection == 1) select_across_down_page();
	else if(selection == 2) delete();
	else if(selection == 3) submit();
	else exit_puzzle();
}

void clear_box(){
	char blank[] = "                                                           ";
	for(int i = 20; i <= 29; i++){
		move(i,57);
		addstr(blank);
	}
	refresh();
}

void select_across_down_page() {
	int i;
	int key;
	int selection = 1; // 1:Across, 2:Down
	char *selections[] = { "[1] Across", "[2] Down" };

	clear_box();

	keypad(stdscr, TRUE);
	while (1) {
		for (i = 0; i < 2; i++) {
			move(24 + i, 80);
			if (i + 1 == selection)
				attron(A_REVERSE);
			printw("%s", selections[i]);
			attroff(A_REVERSE);
			move(LINES - 1, COLS - 1);
			refresh();
		}

		switch (key = getch()) {
		case KEY_DOWN:
			if (++selection == 3) selection = 2;
			break;
		case KEY_UP:
			if (--selection == 0) selection = 1;
			break;
		default:
			break;
		}
		if (key == 10)
			break;
	}

	if (selection == 1) add_page(1);
	else add_page(2);
}

void add_page(int selection){
	int i;
	char *across[] = {"", "grill", "", "", "dig", "",  "our", "again", "ant", "dime", "",  "snow", "", "",  "can", "olive", "", "owl", "tar", "lolly"};
	char *down[] = {"", "guard", "IRA", "long", "drain", "get", "", "", "", "",  "minor", "", "weedy", "roll", "cat", "", "ill"};
	char input[20];
	int number; // Across, Down의 몇 번째 단어인가?

	while(1){
		clear_box();
		move(20,58); printw("Please write the answer. (example: 20 table || back)");
		move(21,58); printw(": ");
		refresh();
		
		i = 0;
		while(1){ // get the input
			input[i] = getch();
			if(input[i] == '\n'){
				input[i] = '\0';
				break;
			}
			i++;
		}
		clear_box();

		if (strcmp(input, "back") == 0) {
			select_across_down_page();
			return;
		}

		number = atoi(input);
		char *pass; // input에서 word만을 뗀 것
		if(number < 10)	pass = input+2;
		else pass = input+3;

		if(selection == 1){ // Across
			if(strcmp(across[number], pass) == 0){
				clear_box();
				move(20,58); printw("Yes! Correct answer!");
				move(LINES-1, COLS-1);
				refresh();
				sleep(1);
					
				// 퍼즐에 단어 추가하는 부분
				if(number == 1) add_across(5,9,pass);
				else if(number == 4) add_across(5,39,pass);
				else if(number == 6) add_across(8,29,pass);
				else if(number == 7) add_across(11,9,pass);
				else if(number == 8) add_across(11,39,pass);
				else if(number == 9) add_across(17,9,pass);
				else if(number == 11) add_across(17,34,pass);
				else if(number == 14) add_across(23,9,pass);
				else if(number == 15) add_across(23,29,pass);
				else if(number == 17) add_across(26,19,pass);
				else if(number == 18) add_across(29,9,pass);
				else if(number == 19) add_across(29,29,pass);
			}
			else{
				clear_box();
				move(20,58); printw("Wrong answer. Try again!");
				move(LINES-1, COLS-1);
				refresh();
				sleep(1);
			}
		}

		else{ // Down
			if(strcmp(down[number], pass) == 0){
				clear_box();
				move(20,58); printw("Yes! Correct answer!");
				move(LINES-1, COLS-1);
				refresh();
				sleep(1);
			}
			else{
				clear_box();
				move(20,58); printw("Wrong answer. Try again!");
				move(LINES-1, COLS-1);
				refresh();
				sleep(1);
			}
		}
	}
}

void add_across(int x, int y, char *input){
	for(int i = 0; i < strlen(input); i++){
		move(x,y + i*5);
		attron(A_BOLD);
		printw("%c", input[i]);
		attroff(A_BOLD);
	}
	move(LINES-1, COLS-1);
	refresh();
}

void delete(){
}
void submit(){
}
void exit_puzzle(){
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
