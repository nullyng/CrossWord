#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <termio.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#define oops(msg) {perror(msg); exit(1);}
#define HOSTNAME "54.180.7.174" // 김주영 aws public IP
#define PORT 25044

struct info {
	int selection;
	char input_s[20];
};

/* pages */
void first_page();		// 시작화면 + 1p,2p 선택화면
void info_page();		// 게임설명, 개발자소개 화면
void select_action_page();	// add, exit 중 선택화면
void select_across_down_page();	// across, down, submit, exit 중 선택화면
void add_page1(int);		// 사용자로부터 input 받아옴
void add_page2(struct info);	// input이 맞으면 퍼즐에 출력
void add_across(int, int,  char*);	// 퍼즐에 across 단어 출력
void add_down(int, int,  char*);  	// 퍼즐에 down 단어 출력
void submit_page();		// 제출
void exit_page();		// 종료 (tty 복구)

/* functions */
void edge();			// 게임화면  테두리
void crossword_base();		// 퍼즐 디자인
void add_blank(int, int, int);	// 퍼즐의 단어가 없는 부분에 블럭 채워넣기
void clear_box();		// 답변 박스 지우기
void player2();			// 서버와 연결, thread 생성
void *thread_loop();		// 다른 클라이언트가 보낸 메세지를 서버로부터 받아옴
void set_cr_noecho_mode();
void tty_mode(int);

/* variables */
int cnt_across = 0;	// total 12
int cnt_down = 0;	// total 10
int flag = 0;		// submit signal
int sock_id;		// client active socket fd
pthread_t t1;
pthread_mutex_t input_lock = PTHREAD_MUTEX_INITIALIZER; // 퍼즐에 단어 추가할 때 lock 
int check_a[20]={0,};
int check_d[20]={0,};
struct timeval start_time, end_time;

void main(){
	void ctrl_c(int);	// declare the handler
	signal(SIGINT, ctrl_c); // install the handler

	tty_mode(0);		// save original mode
	set_cr_noecho_mode();	// canonical, echo mode OFF

	initscr();
	clear();

	first_page();

	gettimeofday(&start_time, NULL); // timer start
	crossword_base();
	select_action_page();

	if(getchar())
		exit_page();
}

void first_page(){
	char *menu[] = {"             [1] game start             ",
		"             [2] game info              ",
		"             [3] game exit              "};
	char line[] = "****************************************";
	char blank[] = "                                        ";
	char *choose[] = {"                 [1] 1p                 ",
		"                 [2] 2p                 "};
	int key, cur, dir_r, dir_c, i;

	if(has_colors())
		start_color();
	init_pair(1,COLOR_YELLOW,COLOR_BLACK);

	attron(COLOR_PAIR(1));
	attron(A_BOLD);
	move(6, 48);	printw("+------------------------+");
	move(7, 48);	printw("|    |    |    |    |    |");
	move(8, 48);	printw("|    |    |   W|    |    |");
	move(9, 48);	printw("--------------------------");
	move(10, 48);	printw("|    |    |    |    |    |"); 
	move(11, 48);	printw("|   C|   R|   O|   S|   S|"); 
	move(12, 48);	printw("--------------------------");
	move(13, 48);	printw("|    |    |    |    |    |");	
	move(14, 48);	printw("|    |    |   R|    |    |");	
	move(15, 48);	printw("--------------------------");
	move(16, 48);	printw("|    |    |    |    |    |");
	move(17, 48);	printw("|    |    |   D|    |    |");	
	move(18, 48);	printw("+------------------------+");
	attroff(COLOR_PAIR(1));
	attroff(A_BOLD);

	edge();

	move(LINES-1,COLS-1);
	refresh();
	sleep(1);

	move(21,41);
	addstr(line);
	move(25,41);
	addstr(line);

	keypad(stdscr,TRUE);
	dir_r = 22;
	dir_c = 41;
	cur = dir_r;

	while(1){
		for(i = 0;i<3;i++){
			move(dir_r+i*1,dir_c);
			if(i*1+dir_r == cur) attron(A_REVERSE);
			printw("%s",menu[i]);
			attroff(A_REVERSE);
			move(LINES-1, COLS -1);
			refresh();
		}

		switch(key = getch()){
			case KEY_DOWN:
				if(cur != dir_r+2) cur+=1;
				break;
			case KEY_UP:
				if(cur != dir_r) cur-=1;
				break;
			default:
				break;
		}
		if(key == 10)
			break;
	}
	clear();
	edge();	

	if(cur == dir_r+1) info_page();

	else if(cur == dir_r){ // game start
		move(12,41);
		addstr(line);
		move(19,41);
		addstr(line);

		dir_r -=9;
		dir_c = 41;
		cur = dir_r;

		while(1){
			for(i = 0;i<2;i++){
				move(dir_r+i*3,dir_c);
				if(i*3+dir_r == cur) attron(A_REVERSE);
				addstr(blank);
				move(dir_r+i*3+1,dir_c);
				printw("%s",choose[i]);
				move(dir_r+i*3+2,dir_c);
				addstr(blank);
				attroff(A_REVERSE);
				move(LINES-1,COLS-1);
				refresh();
			}
			switch(key = getch()){
				case KEY_DOWN:
					if(cur == dir_r) cur += 3;
					break;
				case KEY_UP:
					if(cur == dir_r+3) cur -=3;
					break;
				default:
					break;
			}
			if(key == 10)
				break;
		}		
		if(cur == dir_r+3)
			player2();
	}
	else exit_page();

	clear();
	// 이제 메인에서 게임시작으로 넘어감
}

void info_page(){
	char *info[] = {
		"INFO",
		"Hello guys! This is CrossWord Puzzle!",
		"You can deduce from given explanations,",
		"and fill in the blanks!",
		"Let's study some English words with this game!"};
	char *developers[] = {
		"Developers",
		"2018111947 Kim Dohee",
		"2018113910 Kim Juyoung",
		"2018111138 Baek Hyewon"};

	attron(A_BOLD|A_UNDERLINE);
	move(9, 61-strlen(info[0])/2);
	addstr(info[0]);
	attroff(A_UNDERLINE);

	for(int i = 1; i <= 4; i++){
		move(10+i, 61-strlen(info[i])/2);
		addstr(info[i]);
	}

	attron(A_UNDERLINE);
	move(17, 61-strlen(developers[0])/2);
	addstr(developers[0]);
	attroff(A_UNDERLINE);

	for(int i = 1; i <=3; i++){
		move(18+i, 61-strlen(developers[i])/2);
		addstr(developers[i]);
	}

	init_pair(2,COLOR_RED,COLOR_BLACK);

	for(int i = 8; i > 0; i--){
		move(24,61-16);
		attron(COLOR_PAIR(2));
		printw("Exit this page after %d seconds...", i);
		attroff(COLOR_PAIR(2));
		move(LINES-1, COLS-1);
		refresh();
		sleep(1);
	}
	attroff(A_BOLD);

	clear();
	first_page();
}

void player2(){
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

	pthread_create(&t1,NULL,thread_loop,NULL);
}

void *thread_loop(void){
	int i;
	char *temp[3];
	struct info data;

	while(1)
	{
		char buf[BUFSIZ];

		i=0;
		read(sock_id, buf,sizeof(buf));

		temp[i] = strtok(buf," ");
		while(temp[i] != NULL)
			temp[++i] = strtok(NULL," ");

		sprintf(data.input_s, "%s %s",temp[1],temp[2]);
		data.selection = atoi(temp[0]);
		add_page2(data);
	}
}


void crossword_base() {
	edge();

	// 좌측: 퍼즐
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

	// 우측 상단: 낱말 설명
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

	// 우측 하단: 답변 박스
	attron(A_BOLD);
	move(19, 56);	printw("*************************************************************");
	for(int i = 20; i <= 29; i++){
		move(i, 56);	printw("*                                                           *");
	}
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

	// row의 중간: 16
	// col의 중간: 61
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
	char *selections[] = {"[1] Add ", "[2] Exit"};

	clear_box();

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
	char *selections[] = { "[1] Across", "[2] Down  ", "[3] Submit", "[4] Exit  " };

	clear_box();

	keypad(stdscr, TRUE);
	while (1) {
		for (i = 0; i < 4; i++) {
			move(23 + i, 80);
			if (i + 1 == selection)
				attron(A_REVERSE);
			printw("%s", selections[i]);
			attroff(A_REVERSE);
			move(LINES - 1, COLS - 1);
			refresh();
		}

		switch (key = getch()) {
			case KEY_DOWN:
				if (++selection == 5) selection = 4;
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
	else if(selection == 3) submit_page();
	else exit_page();
}

void add_page1(int selection){
	char *across[] = {"0 empty", "grill", "2 empty", "3 empty", "dig", "5 empty",  "our", "again", "ant", "dime", "10 empty",  "snow", "12 empty", "13 empty",  "can", "olive", "16 empty", "owl", "tar", "lolly"};
	char *down[] = {"0 empty", "guard", "IRA", "long", "drain", "get", "6 empty", "7 empty", "8 empty", "9 empty", "minor", "11 empty", "weedy", "roll", "cat", "15 empty", "ill"};
	char input[20];
	int number; // Across, Down의 몇 번째 단어인가?

	char sendstr[500];
	struct info data;

	while(1){
		clear_box();

		if(cnt_across == 12 && cnt_down == 10){ // 퍼즐 완성 시 submit 페이지를 선택하도록
			move(20,58); printw("You filled all blanks.");
			move(21,58); printw("Let's summit now!!");
			move(LINES-1, COLS-1);
			refresh();
			sleep(2);
			select_across_down_page();
			break;
		}
		else{
			move(20,58); printw("Please write the answer. (example: 20 table || back)");
		}
		move(21,58); printw(": ");
		refresh();

		getstr(input);

		clear_box();

		if (strcmp(input, "back") == 0) {
			select_across_down_page();
			return;
		}

		number = atoi(input);
		char *pass; // input에서 word만을 뗀 것
		if(number<10) pass = input+2;
		else pass = input+3;


		if((selection == 1&& check_a[number] !=1) ||(selection == 2 && check_d[number] != 1)){
			if((selection == 1 && (strcmp(across[number],pass) == 0))||(selection == 2 &&(strcmp(down[number],pass)==0)))
			{
				clear_box();
				move(20,58); printw("Yes! Correct answer!");
				move(LINES-1, COLS-1);
				refresh();
				sleep(1);
				if(t1!='\0'){
					sprintf(sendstr,"%d %s",selection,input);
					write(sock_id,sendstr,strlen(sendstr)+1);
				}
				data.selection = selection;
				strcpy(data.input_s,input);
				add_page2(data);
			}
			else{
				clear_box();
				move(20,58); printw("Wrong answer. Try again!");
				move(LINES-1, COLS-1);
				refresh();
				sleep(1);
			}
		}
		else{
			clear_box();
			move(20,58); printw("Already answered!");
			move(LINES -1, COLS -1);
			refresh();
			sleep(1);
		}
	}
}

void add_page2(struct info input){
	int number;
	char *pass;
	int cur_y, cur_x;

	pthread_mutex_lock(&input_lock);
	getyx(stdscr, cur_y, cur_x);

	number = atoi(input.input_s);
	if(number < 10)	pass = input.input_s+2;
	else pass = input.input_s+3;

	if(input.selection == 1){ // Across
		check_a[number] =1;
		if(cnt_across < 12) cnt_across++; // 12 이상으로 못 올라가게

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

	else{ // Down
		check_d[number] =1;
		if(cnt_down < 10) cnt_down++; // 10 이상으로 못 올라가게

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

	move(cur_y, cur_x);
	refresh();
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
	char success[] = "CLEAR! Congratulations!";
	char success2[] = "Enter the any key to exit.";
	char timer[50];
	double operating_time;
	int min;
	int sec;

	init_pair(3, COLOR_GREEN, COLOR_BLACK);

	if(cnt_across == 12 && cnt_down == 10){ // success
		gettimeofday(&end_time, NULL); // timer end
		operating_time =
			(double)(end_time.tv_sec)+(double)(end_time.tv_usec)/1000000.0-
			(double)(start_time.tv_sec)-(double)(start_time.tv_usec)/1000000.0;
		
		min = ((int)operating_time/60);
		sec = (int)(operating_time-min*60);

		clear();
		edge();

		attron(A_BOLD);
		move(14, 61-strlen(success)/2);
		addstr(success);
		move(15, 61-strlen(success2)/2);
		addstr(success2);

		attron(COLOR_PAIR(3));
		sprintf(timer, "Your elapsed time: %d minutes %d seconds", min, sec);
		move(17, 61-strlen(timer)/2);
		addstr(timer);
		attroff(A_BOLD);
		attroff(COLOR_PAIR(3));

		move(LINES-1, COLS-1);
		refresh();
	}
	else{ // fail
		clear_box();
		attron(A_BOLD);
		move(20,58); printw("The blanks are not filled yet!");
		move(21,58); printw("Go back and fighting!");
		attroff(A_BOLD);

		move(LINES-1, COLS-1);
		refresh();

		sleep(2);
		select_across_down_page();
	}
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

void tty_mode(int how){ // restore original mode
	static struct termios original_mode;

	if(how == 0){
		tcgetattr(0, &original_mode);
	}
	else{
		tcsetattr(0, TCSANOW, &original_mode);
	}
}

void ctrl_c(int signum){
	exit_page();
}
