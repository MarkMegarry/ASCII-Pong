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
	int xmodifier;
	int ymodifier;
	int direction; //0 = straight right, 1 = straight left, 2 = <^, 3 = <v, 4 = ^>, 5 = v>
} Ball = {10,5,0,0,0};

//Set the home of each player
void setPlayerHomes();
//Draw each player
void drawPlayers();
//Generate and assign modifiers
void generateModifiers();
void main(void){
	//Set up ncurses
	cbreak();
	use_env(TRUE); //Use environment variables
	initscr(); //Initialise stdscr
	nodelay(stdscr, TRUE); //stdscr will not wait for inputs
	noecho(); //Inputs will not be echoed
	curs_set(0); //Cursor is hidden
	keypad(stdscr, TRUE); //Keypad enabled
	//Get screen size
	int maxx, maxy;
	getmaxyx(stdscr, maxy, maxx); //Get dimensions of stdscr
	wborder(stdscr, '|', '|', '=','=','+','+','+','+'); //Draw border
	playerHeight = maxy/4; //Set player height
	setPlayerHomes();
	generateModifiers(); //Generate and assign modifiers
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
		/*if(difference > speed/8){
			refresh();
		}*/ //Uncomment to increase smoothness
		if(difference > speed){
			switch(Ball.direction){
				case 0: //Left to right
					move(Ball.ypos, Ball.xpos);
                       			printw("%c", 32);
                        		Ball.xpos += Ball.xmodifier;
					move(Ball.ypos, Ball.xpos);
                        		//Does ball touch paddle?
                        		if(Ball.xpos<Player.home[1] - 1){
                        		   	printw("#");
						refresh();
                        		    	before = clock();
                        		}

                        		else if(Ball.ypos>=Player.ypos&&Ball.ypos<=Player.ypos+playerHeight - 1){
						generateModifiers();
                                		Ball.direction = 2;
                                		Ball.xpos--;
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
                        		}
				break;
				case 2: //Diagonally up left
					move(Ball.ypos, Ball.xpos);
                        		printw("%c", 32);
                        		Ball.xpos-=Ball.xmodifier;
                        		Ball.ypos-=Ball.ymodifier;
                        		move(Ball.ypos, Ball.xpos);
                        		if(Ball.xpos > Ball.xmodifier && Ball.ypos > Ball.ymodifier){ //Ball does not touch wall or ceiling
                                		printw("#");
						refresh();
                                		before = clock();
                        		}
					else{
                        			if(Ball.ypos <= Ball.ymodifier){ //Ball touches ceiling
							Ball.ypos = 1;
							move(Ball.ypos, Ball.xpos);
							printw("#");
							refresh();
                                			Ball.direction = 3;
							before = clock();
                        			}
                        			if(Ball.xpos <= Ball.xmodifier){ //Ball touches wall
							Ball.xpos =1;
							move(Ball.ypos, Ball.xpos);
							printw("#");
							refresh();
                                			Ball.direction = 4;
							before = clock();
                        			}
					}
				break;
				case 3: //Diagonally down left
					move(Ball.ypos, Ball.xpos);
                        		printw("%c", 32);
                       			 Ball.xpos-= Ball.xmodifier;
                        		Ball.ypos+= Ball.ymodifier;
                        		move(Ball.ypos, Ball.xpos);
                        		if(Ball.xpos > Ball.xmodifier && Ball.ypos <=  maxy - Ball.ymodifier - 1){ //Ball does not touch wall or floor
                                		printw("#");
						refresh();
                                		before = clock();
                        		}
					else{
                        			if(Ball.ypos > maxy- Ball.ymodifier - 1){ //Ball touches floor
							Ball.ypos = maxy - 2;
							move(Ball.ypos, Ball.xpos);
							printw("#");
							refresh();
                                			Ball.direction = 2;
							before = clock();
                        			}
                        			if(Ball.xpos <= Ball.xmodifier){ //Ball touches wall
							Ball.xpos = 1;
							move(Ball.ypos, Ball.xpos);
                                                        printw("#");
							refresh();
                                			Ball.direction = 5;
							before = clock();
                        			}
					}
				break;
					case 4: //Ball moving diagonally up right
					move(Ball.ypos, Ball.xpos);
                        		printw("%c", 32);
                        		Ball.xpos+= Ball.xmodifier;
                        		Ball.ypos-= Ball.ymodifier;
                        		move(Ball.ypos, Ball.xpos);
                        		if(!(Ball.xpos>=Player.home[1]-1&&Ball.ypos>=Player.ypos&&Ball.ypos<=Player.ypos+playerHeight-1) && Ball.ypos >  Ball.ymodifier){ //Ball does not touch paddle or ceiling
                                		printw("#");
						refresh();
                                		before = clock();
                        		}
					else{
                        			if(Ball.ypos <= Ball.ymodifier){ //Ball touches ceiling
							Ball.ypos = 1;
							move(Ball.ypos, Ball.xpos);
                                                        printw("#");
							refresh();
                                			Ball.direction = 5;
							before = clock();;
                        			}
                        			if(Ball.xpos>=Player.home[1]-1&&Ball.ypos>=Player.ypos&&Ball.ypos<=Player.ypos+playerHeight-1){ //Ball touches paddle
                                			Ball.direction = 2;
							Ball.xpos=Player.home[1] - 1;
							move(Ball.ypos, Ball.xpos);
                                                        printw("#");
							generateModifiers();
							refresh();
							before = clock();
                        			}
					}
				break;
				case 5: //Diagonally down right
					move(Ball.ypos, Ball.xpos);
					printw("%c", 32);
					Ball.xpos+= Ball.xmodifier;
					Ball.ypos+= Ball.ymodifier;
					move(Ball.ypos, Ball.xpos);
					if(Ball.ypos<=maxy-Ball.ymodifier-1&& !(Ball.xpos>=Player.home[1]-1&&Ball.ypos>=Player.ypos&&Ball.ypos<=Player.ypos+playerHeight-1)){ //Ball does not touch paddle or floor
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
							before = clock();
						}
						if(Ball.xpos>=Player.home[1]-1&&Ball.ypos>=Player.ypos&&Ball.ypos<=Player.ypos+playerHeight-1){ //Ball touches paddle
							Ball.xpos = Player.home[1]-1;
							move(Ball.ypos, Ball.xpos);
                                                        printw("#");
							generateModifiers();
							refresh();
							Ball.direction = 2;
							before = clock();
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
void generateModifiers(){
	Ball.xmodifier = (rand() % (2)) + 1;
	Ball.ymodifier = (rand() % (2)) + 1;
}

