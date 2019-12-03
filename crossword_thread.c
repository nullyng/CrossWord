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
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

#define oops(msg) {perror(msg); exit(1);}
#define HOSTNAME "54.180.7.174"
#define PORT 25043

struct info {
	int selection;
	char input_s[20];
};

void edge(); // 게임화면  테두리
void crossword_base();
void add_blank(int, int, int);
void clear_box();

void player1();
void player2();

void *thread_loop();

void first_page();
void select_action_page();
void select_across_down_page();
void add_page1(int);
void add_page2(struct info);
void add_across(int, int,  char*);
void add_down(int, int,  char*);

void submit_page();
void exit_page();

void set_cr_noecho_mode();
void set_nodelay_mode(void);
void tty_mode(int);
void screen_demensions();

//int selection = 1;
int cnt_across = 0;
int cnt_down = 0;
int flag = 0; //sumbit signal
//pthread_mutex_t counter_lock1 = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t counter_lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_t t1;
int sock_id;
pthread_mutex_t input_lock = PTHREAD_MUTEX_INITIALIZER;
int ws_row, ws_col; // window size

void main(){
	tty_mode(0); // save original mode
	set_cr_noecho_mode(); // canonical, echo mode OFF
	//set_nodelay_mode(); // blocking OFF
	screen_demensions();

	initscr();
	clear();

	first_page();
	crossword_base(); // 일단 1p일 때 넘어가게 만듦.
	select_action_page();	

	if(getchar()){
		endwin();
		tty_mode(1);
		return;
	}
}

void first_page(){
	char title[] = "Cross Word";
	char *menu[] = {"[1] game start","[2] game exit"};
	//char menu2[] = "2. game exit";
	char *choose[] = {"[1] 1p","[2] 2p"};
	int key, cur, dir_r,dir_c,i;

	if(has_colors())
		start_color();
	init_pair(1,COLOR_BLACK,COLOR_YELLOW);

	move(12,56);
	attron(COLOR_PAIR(1));
	addstr(title);
	attroff(COLOR_PAIR(1));

	edge();

	move(LINES-1,COLS-1);
	refresh();
	sleep(1);

	keypad(stdscr,TRUE);
	dir_r = 16;
	dir_c = 54;
	cur = dir_r;

	while(1){
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
	edge();	

	if(cur == dir_r){
		dir_r -=2;
		dir_c = 58;
		cur = dir_r;

		while(1){
			for(i = 0;i<2;i++){
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
		exit_page();

	if(cur == dir_r+4)
		player2();

	// 1p, 2p일 때 어떻게 할 것인가?? 일단 둘 다 게임화면 뜨게 해놨음
	clear();
}

void player2()
{
	struct sockaddr_in servadd;
	struct hostent *hp;

	clear();

	sock_id = socket(AF_INET,SOCK_STREAM,0);
	if(sock_id == -1)
		oops("socket");

	memset(&servadd, 0, sizeof(servadd));
	servadd.sin_family = AF_INET;
	servadd.sin_addr.s_addr = inet_addr(HOSTNAME);
	servadd.sin_port = htons(PORT);	

	if(connect(sock_id, (struct sockaddr *)&servadd, sizeof(servadd))!=0)
		oops("connect");

	pthread_create(&t1, NULL, thread_loop,NULL);

}

void *thread_loop(void){
	int i;
	char *temp[3];
	struct info data;	

	while(1){

		char buf[BUFSIZ];

		i = 0;
		read(sock_id,buf,sizeof(buf));

		temp[i] = strtok(buf," ");
		while(temp[i]!=NULL)
			temp[++i]=strtok(NULL," ");

		sprintf(data.input_s,"%s %s",temp[1],temp[2]);
		data.selection = atoi(temp[0]);

		add_page2(data);	
	}
}

void crossword_base() {
	edge();

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

void edge(){
	for(int i = 0; i <= 32; i++){
		move(i,122); printw("|");
	}
	move(33,0);
	printw("--------------------------------------------------------------------------------------------------------------------------+");
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
	char *selections[] = {"[1] Add", "[2] Exit"};

	keypad(stdscr, TRUE);
	while(1){
		for(int i = 0; i < 2; i++){
			move(24+i,80);
			if(i + 1 == selection)
				attron(A_REVERSE);
			printw("%s", selections[i]);
			attroff(A_REVERSE);
			move(LINES-1, COLS-1);
			refresh();
		}

		switch(key = getch()){
			case KEY_DOWN:
				if(++selection == 3) selection = 2;
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
	else exit_page();
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
	int selection = 1; // 1:Across, 2:Down, 3:Submit
	char *selections[] = { "[1] Across", "[2] Down", "[3] Submit" };

	clear_box();

	keypad(stdscr, TRUE);
	while (1) {
		for (i = 0; i < 3; i++) {
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
				if (++selection == 4) selection = 3;
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

	if (selection == 1) add_page1(1);
	else if(selection == 2) add_page1(2);
	else submit_page();
}

void add_page1(int selection){
	char *across[] = {"", "grill", "", "", "dig", "",  "our", "again", "ant", "dime", "",  "snow", "", "",  "can", "olive", "", "owl", "tar", "lolly"};
	char *down[] = {"", "guard", "IRA", "long", "drain", "get", "", "", "", "",  "minor", "", "weedy", "roll", "cat", "", "ill"};
	char input[20];
	int number; // Across, Down의 몇 번째 단어인가?
	char *pass; // input에서 word만을 뗀 것
	int i;

	char sendstr[BUFSIZ];
	struct info data;

	while(1){
		clear_box();

		if(cnt_across == 12 && cnt_down == 10){
			pthread_cancel(t1);
			move(20,58); printw("You filled all blanks. Please enter 'back' and submit!");
		}
		else{
			move(20,58); printw("Please write the answer. (example: 20 table || back)");
		}
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
		if(number < 10)	pass = input+2;
		else pass = input+3;

		if(t1!='\0'){
			if(selection == 1){ // Across
				if(strcmp(across[number], pass) == 0){
					sprintf(sendstr,"%d %s\n",selection,input);
					write(sock_id,sendstr,strlen(sendstr)+1);
				}
			}
			else{ // Down
				if(strcmp(down[number], pass) == 0){
					sprintf(sendstr,"%d %s\n",selection,input);
					write(sock_id,sendstr,strlen(sendstr)+1);
				}
			}
		}
		data.selection = selection;
		strcpy(data.input_s,input);
		add_page2(data);
	}
}

void add_page2(struct info input){
	char *across[] = {"", "grill", "", "", "dig", "",  "our", "again", "ant", "dime", "",  "snow", "", "",  "can", "olive", "", "owl", "tar", "lolly"};
	char *down[] = {"", "guard", "IRA", "long", "drain", "get", "", "", "", "",  "minor", "", "weedy", "roll", "cat", "", "ill"};
	pthread_mutex_lock(&input_lock);
	int number; // Across, Down의 몇 번째 단어인가?
	number = atoi(input.input_s);
	char *pass; // input에서 word만을 뗀 것

	if(number < 10)	pass = input.input_s+2;
	else pass = input.input_s+3;

	if(input.selection == 1){ // Across
		if(strcmp(across[number], pass) == 0){
			clear_box();
			move(20,58); printw("Yes! Correct answer!");
			move(LINES-1, COLS-1);
			refresh();
			sleep(1);

			cnt_across++;

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

			cnt_down++;

			// 퍼즐에 단어 추가하는 부분
			if(number == 1) add_down(5,9,pass);
			else if(number == 2) add_down(5,19,pass);
			else if(number == 3) add_down(5,29,pass);
			else if(number == 4) add_down(5,39,pass);
			else if(number == 5) add_down(5,49,pass);
			else if(number == 10) add_down(17,19,pass);
			else if(number == 12) add_down(17,49,pass);
			else if(number == 13) add_down(20,29,pass);
			else if(number == 14) add_down(23,9,pass);
			else if(number == 16) add_down(23,39,pass);
		}
		else{
			clear_box();
			move(20,58); printw("Wrong answer. Try again!");
			move(LINES-1, COLS-1);
			refresh();
			sleep(1);
		}
	}
	pthread_mutex_unlock(&input_lock);
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

void add_down(int x, int y, char *input){
	for(int i = 0; i < strlen(input); i++){
		move(x + i*3,y);
		attron(A_BOLD);
		printw("%c", input[i]);
		attroff(A_BOLD);
	}
	move(LINES-1, COLS-1);
	refresh();
}


void submit_page(){
	char message[] = "CLEAR! Congratulations!";

	clear();
	move(ws_row/2, ws_col/2 - strlen(message)/2);
	attron(A_BOLD);
	addstr(message);
	attroff(A_BOLD);
	move(LINES-1, COLS-1);
	refresh();
}

void exit_page(){
	endwin();
	tty_mode(1);
	exit(0);
}

void set_cr_noecho_mode(){ // canonical mode OFF
	struct termios ttystate;

	tcgetattr(0, &ttystate);
	ttystate.c_lflag &= ~ICANON;
	ttystate.c_lflag &= ~ECHO;
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

void screen_demensions(){
	struct winsize wbuf;

	if(ioctl(0, TIOCGWINSZ, &wbuf) != -1){
		ws_row = wbuf.ws_row;
		ws_col = wbuf.ws_col;
	}
}
