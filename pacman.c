#include <stdio.h>
#include <ncurses.h> //Ncurses library ported in
#include <stdbool.h>
#include <time.h> //Time to handle start screen. https://stackoverflow.com/questions/17167949/how-can-i-use-a-timer-in-c 
#include <stdlib.h> //Needed for exit.
//Splash screen might be added later.
#define WIDTH 60 //Because it looked incorrect
#define HEIGHT 30 
#define WALL '#' //Needed for collision purposes.
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

int pacman_x = 1;
int pacman_y = 1;
int main(int argc, char * argv[]) { 
char stage[HEIGHT][WIDTH + 1] = { //WIDTH + 1 because of the C null terminator, which is why this was wreaking havoc LOL!
    "############################################################", 
    "#..........................................................#", 
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#",
    "#..........................................................#", 
    "############################################################"  
};						
								
   WINDOW *game_win;
   int pacman_move;
   int direction = KEY_RIGHT; //https://pacmancode.com/start-positions
   bool running = true;
   initscr();
   start_color(); //Needed to initialize color pairs. 
   init_pair(1, COLOR_YELLOW, COLOR_BLACK); //Key 1, Yellow Character, Black Background.
   init_pair(2, COLOR_BLUE, COLOR_BLACK);   //Key 2, Blue Character, Black Background.
   init_pair(3, COLOR_CYAN, COLOR_BLACK); //Key 3, Magenta Character, Black Background.
   init_pair(4, COLOR_RED, COLOR_BLACK);    //Key 4, Red Character, Black Background.
   init_pair(5, COLOR_GREEN, COLOR_BLACK); //Key 5, Green Character, Black Background.
   
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
   keypad(stdscr, TRUE);
   game_win = newwin(HEIGHT, WIDTH, 0, 0); //Window initialized with size 60 x 30. See comment for reason.
  
   curs_set(0); //Cursor hidden from terminal, because that breaks game flow.
   wrefresh(game_win); //Game window refreshed.
   nodelay(stdscr, TRUE); //Had to add this, because input is normally blocking.
   
   while (running) {
    werase(game_win); //Game window is cleared of previous screen.
    mvwaddch(game_win, pacman_y, pacman_x, ' '); //Needed to clear trailing output, similar to pygame.
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
    pacman_move = getch();
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
    switch(direction) {
        case KEY_UP: 
             if (pacman_y > 1 && stage[pacman_y][pacman_x]!=WALL) { //Needed to avoid an out of bounds bug.
                pacman_y--; //Pacman moves up by decrementing y axis by one.
             }
             break;
        case KEY_DOWN:
             if (pacman_y < HEIGHT - 2) { //Needed to avoid an out of bounds bug.
                pacman_y++; //Pacman moves down by incrementing y axis by one.
             }
             break;
        case KEY_LEFT:
             if (pacman_x > 1) {
                pacman_x--;
             }
             break;
        case KEY_RIGHT:
             if (pacman_x < WIDTH - 2) {
                pacman_x++;
             }
             break;
    }
    
    mvwaddch(game_win, pacman_y, pacman_x, 'P' | COLOR_PAIR(1)); //https://docs.oracle.com/cd/E86824_01/html/E54767/mvwaddch-3curses.html
    wrefresh(game_win); //Game window refreshed.
    napms(100); 
   }
   endwin();
   return 0;
}

//Note, to compile gcc -o pacman pacman.c -lncurses

