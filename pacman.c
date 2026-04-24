#include <stdio.h>
#include <ncurses.h> //Ncurses library ported in
#include <stdbool.h>
#include <time.h> //Time to handle start screen. https://stackoverflow.com/questions/17167949/how-can-i-use-a-timer-in-c 
#include <stdlib.h> //Needed for exit and random.
//Splash screen might be added later.
#define WIDTH 60 //Because it looked incorrect
#define HEIGHT 30 
#define WALL '#' //Needed for collision purposes.
#define WALL2 '-'//Same reason as above, to make the ghost logic easier.
#define DOT '.' //Needed to populate the dots.
#define SPECIAL '@'
#define CHERRY 'C'
#define EATEN ' '
/*  Width and height from: https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/keys.html
 Pac-Man
• Objective: navigate a maze and eat all Pac-Dots.
• Game Design:
– The maze must be 30 → 30 characters in size.
– Pac-Man begins in the top left corner of the maze.
– Four Ghosts must appear at random locations and move in random directions all the
time.
– Ghosts have the ability to eat Pac-Man if a ghost encounters Pac-Man.
– Four Power Pellets appear randomly in the maze. Pac-Man can consume them to gain
the ability to eat the ghosts for 10 seconds.
– The maze contains 150 Pac-Dots.
• Game Play: The game ends with a win if Pac-Man eats all the Pac-Dots. The game ends in
lost if Pac-Man is eaten by a ghost

https://pacman.fandom.com/wiki/Fruits
The fruits feature different mechanics in certain games, but across all of them, they can appear twice in every round.
In the original Pac-Man, the fruits are located below the Regeneration Chamber and won't move at all. 
The fruit appears after 70 dots are eaten and again after 170 dots are eaten unless the first fruit is still there. They will disappear if they are not eaten after 9-10 seconds.*/

/*SCORING SYSTEM:
 * Pac-Man scores points by consuming dots (10 pts), power pellets (50 pts), fruits (100–5,000 pts), and frightened ghosts (200–1,600 pts). 
 * The maximum score, achieved by eating everything on all 256 levels without losing a life, is 3,333,360 points. 
 * Points increase by chaining ghost kills, and a bonus life is awarded at 10,000 points.*/
 
 /* Lives System:
  * In the original 1980 Pac-Man arcade game, the player starts with 3 lives (one active Pac-Man and two in reserve). 
  * The game allows for configuration changes, but the default setting is 3, with an extra life awarded at 10,000 points.*/
int pacman_x = 1;
int pacman_y = 1;
int next_direction_x=1;
int next_direction_y=1;
int score = 0;
int lives = 3;
int cherry_end_time = 0;
int seconds = 10; //For cherry timer.
bool going_once = true;
bool start_timer_cherry = false;
void ghost_time() {
	//Will need timer, aswell as cool effect and other things.
}
int main(int argc, char * argv[]) { 
   char stage[HEIGHT][WIDTH + 1] = { 
    "############################################################", 
    "#    .......                ###                ......      #", 
    "#  #########   ###########  ###  ############  ########## .#",
    "#  #########   ###########  ###  ############  ##########  #",
    "#  #########   ###########  ###  ############  ########## .#",
    "#                                                          #",
    "#. #########  ... ######################## ... ########## .#",
    "#. #########   #. ######################## .#  ########## .#",
    "#.......       #. ######################## .#        ......#",
    "#############  #............####............#  #############",
    "           ##  ###########  ####  ###########  ##           ",
    "           ##  ###########  ####  ###########  ##           ",
    "           ##  #                            #  ##           ",
    "#############  #  ##########----##########  #  #############",
    "                  #                      #                  ",
    "#############  #  #                      #  #  #############",
    "           ##  #  ########################  #  ##           ",
    "           ##  #                            #  ##           ",
    "           ##  #  ########################  #  ##           ",
    "#############  #  ########################  #  #############",
    "#.........      .           ####           .     ..........#",
    "#. #######  ##############  ####  ##############  ####### .#",
    "#. #######  ##############  ####  ##############  ####### .#",
    "# .....###                                        ###..... #",
    "##### .###  #####  ######################  #####  ###. #####",
    "##### .###  #####  ######################  #####  ###. #####",
    "#           #####           ####           #####           #",
    "#. ####################### .####. ####################### .#",
    "#..........      ...........    ...........       .........#", 
    "############################################################"  
};	

//WIDTH = 60, HEIGHT = 30
//GHOST SPAWN IN MIDDLE					
								
   WINDOW *game_win;
   int pacman_move;
   int direction = KEY_RIGHT; //https://pacmancode.com/start-positions
   bool running = true;
   initscr();
   start_color(); //Needed to initialize color pairs. 
   //Needed for codespaces to not highlight text or show grey as the background
   use_default_colors();
   init_pair(1, COLOR_GREEN, -1); //Key 1, Green Character, Black Background.
   init_pair(1, COLOR_YELLOW, -1); //Key 1, Yellow Character, Black Background.
   init_pair(2, COLOR_BLUE, -1);   //Key 2, Blue Character, Black Background.
   init_pair(3, COLOR_CYAN, -1); //Key 3, Magenta Character, Black Background.
   init_pair(4, COLOR_RED, -1);    //Key 4, Red Character, Black Background.
   init_pair(5, COLOR_GREEN, -1); //Key 5, Green Character, Black Background.
   
   //NOTE: While in a normal pacman game, the dots would be yellow. I dislike how much the yellow blends
   //with pacman because you cannot change the opacity of a color in ncurses. Or if is, it adds unnecessary bloat.
   if(has_colors() == FALSE){	
	    endwin(); //Handles if color is not supported on the terminal.
		printf("Your terminal does not support color\n");
		exit(1);
   } //https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/color.html
   clear();
   noecho();
   cbreak();
  

   
   game_win = newwin(HEIGHT, WIDTH, 10, 90); //Window placed at X:90, and Y: 10.
  
   curs_set(0); //Cursor hidden from terminal, because that breaks game flow.
   wrefresh(game_win); //Game window refreshed.
   keypad(game_win, TRUE);
   nodelay(game_win, TRUE);//Had to add this, because input is normally blocking.
   
   while (running) {
	mvprintw(8, 90, "SCORE: %d                                           LIVES: %d", score, lives); //https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/printw.html
	refresh();
    werase(game_win); //Game window is cleared of previous screen.
    mvwaddch(game_win, pacman_y, pacman_x, ' '); //Needed to clear trailing output, similar to pygame.
    //Timer is also like pygame, I couldn't use sleep since sleep blocks the CPU.
    if (start_timer_cherry) {
		if (time(NULL) >= cherry_end_time) {
			stage[17][29] = EATEN;
			start_timer_cherry = false;
		}
	}
    if (score > 70 && going_once) {
		stage[17][29] = CHERRY;
		start_timer_cherry = true;
		going_once = false;
		cherry_end_time = time(NULL) + 10; //Takes the current time and adds 10 seconds, like in the original game.
	}
    for (int y = 0; y < HEIGHT; y++) { //This loop draws the stage as it evolves. Starting with the y axis, or the columns.
		for (int x = 0; x < WIDTH; x++) { //Same as above, but for x axis
			if (stage[y][x] == WALL) {
			    mvwaddch(game_win, y, x, stage[y][x] | COLOR_PAIR(2)); //Stage drawn.
			} else if (stage[y][x] ==  DOT) {
				mvwaddch(game_win, y, x, stage[y][x] | COLOR_PAIR(3)); //Stage drawn.	
			} else if (stage[y][x] == CHERRY) {
				mvwaddch(game_win, y, x, stage[y][x] | COLOR_PAIR(4)); //Stage drawn.	
			} else if (stage[y][x] == SPECIAL) {
				mvwaddch(game_win, y, x, stage[y][x] | COLOR_PAIR(5)); //Stage drawn.	
			} else {
				mvwaddch(game_win, y, x, stage[y][x]); //Stage drawn.
			}
		}
	}
	pacman_move = ERR; //Without this, the program would get confused and pacman would get stuck on walls.
	int ch;
	while ((ch = wgetch(game_win)) != ERR) { //Provided there is no error, then the movement will be captured.
		pacman_move = ch; 
	}

    next_direction_x = pacman_x;
    next_direction_y = pacman_y;
    switch(pacman_move) {
        case KEY_UP: 
        case KEY_DOWN:
        case KEY_LEFT:
        case KEY_RIGHT:
             direction = pacman_move;
             break;
        case 27: 
             running = false;
             break;
    }
    switch(direction) { //Switched because of collision conflicts, the program needs to predict what happens next rather
		//than just moving and then calculating conflict.
        case KEY_UP:    next_direction_y--; break;
        case KEY_DOWN:  next_direction_y++; break;
        case KEY_LEFT:  next_direction_x--; break;
        case KEY_RIGHT: next_direction_x++; break;
    }
    //Prevents an out of bound error for the program.
    if (next_direction_y >= 0 && next_direction_y < HEIGHT &&  next_direction_x >= 0 && next_direction_x < WIDTH) {
		if ((stage[next_direction_y][next_direction_x] != WALL) && (stage[next_direction_y][next_direction_x] != WALL2)) { //This will only update pacman x and pacman y if its not a wall.
            pacman_y = next_direction_y;
            pacman_x = next_direction_x;
		}
		if (stage[next_direction_y][next_direction_x] == DOT || stage[next_direction_y][next_direction_x] == CHERRY || stage[next_direction_y][next_direction_x] == SPECIAL) {
			if (stage[next_direction_y][next_direction_x] == DOT) {
				score+=10;
			} else if (stage[next_direction_y][next_direction_x] == CHERRY) {
				score+=100;
			} else {
				ghost_time();
			}
			stage[next_direction_y][next_direction_x] = EATEN;
		}
		
	}
	//This is how pacman can move through the tunnel and teleport.
	if (next_direction_y == 14) {
		if (next_direction_x < 0) { 
			pacman_x = WIDTH - 1;
			pacman_y = 14; 
			direction = KEY_LEFT;
    } else if (next_direction_x >= WIDTH) { 
        pacman_x = 0;
        pacman_y = 14;
        direction = KEY_RIGHT;
    }
	}
	
    mvwaddch(game_win, pacman_y, pacman_x, 'P' | COLOR_PAIR(1)); //https://docs.oracle.com/cd/E86824_01/html/E54767/mvwaddch-3curses.html
    wrefresh(game_win); //Game window refreshed.
    napms(100); 
   }
   endwin();
   return 0;
}

//Note, to compile gcc -o pacman pacman.c -lncurses

