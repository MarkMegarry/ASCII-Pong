#include <stdbool.h>
#include <time.h>
#include <ncurses.h>
#include <stdlib.h>
int playerHeight; //Global player height variable
//Speed variable.
unsigned int speed = 100000;
//Player data structure
struct Player{
	//YX coords of top of player's home
	int home[2];
	int ypos;
} Player = {{0,0},0};
//Ball data structure
struct Ball{
	int ypos;
	int xpos;
	int direction; //0 = straight right, 1 = straight left, 2 = <^, 3 = <v, 4 = ^>, 5 = v>
} Ball = {10,5,0};

//Set the home of each player
void setPlayerHomes();
//Draw each player
void drawPlayers();
void main(void){
	//Set up ncurses
	cbreak();
	use_env(TRUE);
	initscr();
	nodelay(stdscr, TRUE);
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);
	//Get screen size
	int maxx, maxy;
	getmaxyx(stdscr, maxy, maxx);
	wborder(stdscr, '|', '|', '=','=','+','+','+','+'); //Draw border
	playerHeight = maxy/4; //Set player height
	setPlayerHomes();
	drawPlayers();
	refresh();
	//Seed RNG
	srand(time(NULL));
	clock_t before = clock(); //Start timer
	while(1){
		//Check player's input
		int  pressedKey = getch();
		//Player presses up
		if(pressedKey == KEY_UP && Player.ypos > 1){
			//Move player 2 up by deleting lowest byte and adding one on top
			move(Player.ypos+playerHeight-1, Player.home[1]);
			printw("%c", 32);
			Player.ypos--;
			move(Player.ypos, Player.home[1]);
			printw("[");
		}
		//Player presses down
		else if(pressedKey == KEY_DOWN && Player.ypos < maxy - playerHeight - 1){
			move(Player.ypos, Player.home[1]);
			printw("%c", 32); //Print space over top char
			Player.ypos++;
			move(Player.ypos + playerHeight-1, Player.home[1]);
			printw("[");
		}
		//Move ball
		int difference = clock() - before;
		if(difference > speed){
			int xmodifier = (rand() % 2) + 1; //Generate movement modifier
			int ymodifier = (rand()%2) + 1;
			switch(Ball.direction){
				case 0: //Left to right
					move(Ball.ypos, Ball.xpos);
                       			 printw("%c", 32);
                        		Ball.xpos++;
                        		//Does ball touch paddle?
                        		if(Ball.xpos>=Player.home[1]-1&&Ball.ypos>=Player.ypos&&Ball.ypos<=Player.ypos+playerHeight - 1){
                                		beep();
                                		Ball.direction = 2;
                                		Ball.xpos--;
                        		}

                       			 else {
                                		printw("#");
                                		before = clock();
                        		}
				break;
				case 1: //Right to left
                       			 move(Ball.ypos, Ball.xpos);
                        		printw("%c", 32);
                        		Ball.xpos--;
                        		move(Ball.ypos,Ball.xpos);
                        		if(Ball.xpos > 1){ //Does ball touch wall?  No
                                		printw("#");
						refresh();
                                		before = clock();
                        		}
                        		else if(Ball.xpos <= 1){ //Yes
                                		Ball.direction = 0;
                                		beep();
                        		}
				break;
				case 2: //Diagonally up left
					move(Ball.ypos, Ball.xpos);
                        		printw("%c", 32);
                        		Ball.xpos-=xmodifier;
                        		Ball.ypos-=ymodifier;
                        		move(Ball.ypos, Ball.xpos);
                        		if(Ball.xpos > xmodifier && Ball.ypos > ymodifier){ //Ball does not touch wall or ceiling
                                		printw("#");
						refresh();
                                		before = clock();
                        		}
					else{
                        			if(Ball.ypos <= ymodifier){ //Ball touches ceiling
							Ball.ypos = 1;
							move(Ball.ypos, Ball.xpos);
							printw("#");
							refresh();
                                			Ball.direction = 3;
                                			beep();
                        			}
                        			if(Ball.xpos <= xmodifier){ //Ball touches wall
							Ball.xpos =1;
							move(Ball.ypos, Ball.xpos);
							printw("#");
							refresh();
                                			Ball.direction = 4;
                               				 beep();
                        			}
					}
				break;
				case 3: //Diagonally down left
					move(Ball.ypos, Ball.xpos);
                        		printw("%c", 32);
                       			 Ball.xpos-= xmodifier;
                        		Ball.ypos+= ymodifier;
                        		move(Ball.ypos, Ball.xpos);
                        		if(Ball.xpos > xmodifier && Ball.ypos <=  maxy - ymodifier - 1){ //Ball does not touch wall or floor
                                		printw("#");
						refresh();
                                		before = clock();
                        		}
					else{
                        			if(Ball.ypos > maxy- ymodifier - 1){ //Ball touches floor
							Ball.ypos = maxy - 2;
							move(Ball.ypos, Ball.xpos);
							printw("#");
							refresh();
                                			Ball.direction = 2;
                                			beep();
                        			}
                        			if(Ball.xpos <= xmodifier){ //Ball touches wall
							Ball.xpos = 1;
							move(Ball.ypos, Ball.xpos);
                                                        printw("#");
							refresh();
                                			Ball.direction = 5;
                                			beep();
                        			}
					}
				break;
					case 4: //Ball moving diagonally up right
					move(Ball.ypos, Ball.xpos);
                        		printw("%c", 32);
                        		Ball.xpos+= xmodifier;
                        		Ball.ypos-= ymodifier;
                        		move(Ball.ypos, Ball.xpos);
                        		if(!(Ball.xpos>=Player.home[1]-1&&Ball.ypos>=Player.ypos&&Ball.ypos<=Player.ypos+playerHeight-1) && Ball.ypos >  ymodifier){ //Ball does not touch paddle or ceiling
                                		printw("#");
						refresh();
                                		before = clock();
                        		}
					else{
                        			if(Ball.ypos <= ymodifier){ //Ball touches ceiling
							Ball.ypos = 1;
							move(Ball.ypos, Ball.xpos);
                                                        printw("#");
							refresh();
                                			Ball.direction = 5;
                                			beep();
                        			}
                        			if(Ball.xpos>=Player.home[1]-1&&Ball.ypos>=Player.ypos&&Ball.ypos<=Player.ypos+playerHeight-1){ //Ball touches paddle
                                			Ball.direction = 2;
							Ball.xpos=Player.home[1] - 1;
							move(Ball.ypos, Ball.xpos);
                                                        printw("#");
							refresh();
                                			beep();
                        			}
					}
				break;
				case 5: //Diagonally down right
					move(Ball.ypos, Ball.xpos);
					printw("%c", 32);
					Ball.xpos+= xmodifier;
					Ball.ypos+= ymodifier;
					move(Ball.ypos, Ball.xpos);
					if(Ball.ypos<=maxy-ymodifier-1&& !(Ball.xpos>=Player.home[1]-1&&Ball.ypos>=Player.ypos&&Ball.ypos<=Player.ypos+playerHeight-1)){ //Ball does not touch paddle or floor
						printw("#");
						refresh();
						before = clock();
					}
					else{
						if(Ball.ypos > maxy - 2){ //Ball touches floor
							Ball.ypos = maxy - 2;
							move(Ball.ypos, Ball.xpos);
                                                        printw("#");
							refresh();
							Ball.direction = 4;
							beep();
						}
						if(Ball.xpos>=Player.home[1]-1&&Ball.ypos>=Player.ypos&&Ball.ypos<=Player.ypos+playerHeight-1){ //Ball touches paddle
							Ball.xpos = Player.home[1]-1;
							move(Ball.ypos, Ball.xpos);
                                                        printw("#");
							refresh();
							Ball.direction = 2;
							beep();
						}
					}
				break;
				}

		}
		/*if(difference > speed/2){
			refresh();
		}*/
	}
	endwin();
}

void setPlayerHomes(){
	int maxy, maxx;
	getmaxyx(stdscr, maxy, maxx);
	Player.home[0] = maxy/2 - playerHeight;
	Player.home[1] = maxx - 4;
}
void drawPlayers(){
	move(Player.home[0], Player.home[1]);
	vline('[', playerHeight);
	Player.ypos = Player.home[0]; //Store player 2's ypos
}

