#include <stdio.h>
#include <ncurses.h> //Ncurses library ported in
#include <stdbool.h>
#include <time.h> //Time to handle start screen. https://stackoverflow.com/questions/17167949/how-can-i-use-a-timer-in-c 
#include <stdlib.h> //Needed for exit and random.
#include <math.h> //Needed for distance
//Splash screen might be added later.
#define WIDTH 60 //Because it looked incorrect
#define HEIGHT 30 
#define WALL '#' //Needed for collision purposes.
#define WALL2 '-'//Same reason as above, to make the ghost logic easier.
#define DOT '.' //Needed to populate the dots.
#define SPECIAL '@'
#define CHERRY 'C'
#define STRAWBERRY 'S'
#define ORANGE 'O'
#define APPLE 'A'
#define MELON 'M'
//Galaxian and Bell ommited since G is used for ghosts, and is bell ommited for higher stakes.
#define KEY 'K'
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

https://pacman.fandom.com/wiki/fruits
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
char fruit = CHERRY;
int pacman_x = 1;
int pacman_y = 1;
int pacman_old_x = 1;
int pacman_old_y = 1;
int next_direction_x=1;
int next_direction_y=1;
int score = 0;
int lives = 3;
int fruit_end_time = 0;
int start_time = 0;
int seconds = 10; //For cherry timer.
int pellets_collected = 0; //For the reset logic.
bool going_once = true;
bool start_timer_fruit = false;
int pacman_move;int previous_direction_x = KEY_RIGHT;
int previous_direction_y = KEY_DOWN;
int direction = KEY_RIGHT; //https://pacmancode.com/start-positions
int levels_beaten = 0;
bool running = true;
char original_stage[HEIGHT][WIDTH + 1] = { 
    "############################################################", 
    "#    ........              #####              .......      #", 
    "# ############ ########### ##### ############ ############.#",
    "# ############ ########### ##### ############ ############ #",
    "# ############ ########### ##### ############ ############.#",
    "#                                                          #", 
    "#.############.#.##########################.#.############.#",
    "#.############ #.##########################.# ############.#",
    "#.......       #.##########################.#        ......#",
    "############## #............####............# ##############",
    "           ### ############ #### ############ ###           ",
    "           ### ############ #### ############ ###           ",
    "           ### ##                          ## ###           ",
    "############## ## ##########----########## ## ##############",
    "                  #                      #                  ",
    "############## ## #                      # ## ##############",
    "           ### ## ######################## ## ###           ",
    "           ### ##                          ## ###           ",
    "           ### ## ######################## ## ###           ",
    "############## ## ######################## ## ##############",
    "#.........      .           ####           .     ..........#",
    "#.######### ############### #### ############### #########.#",
    "#.######### ############### #### ############### #########.#",
    "# .....####                                      ####..... #",
    "######.#### ##### ######################## ##### ####.######",
    "######.#### ##### ######################## ##### ####.######",
    "#           #####           ####           #####           #",
    "#.#########################.####.#########################.#",
    "#..........      ...........    ...........       .........#", 
    "############################################################"  
};	
char stage[HEIGHT][WIDTH + 1];

//Defines how the ghosts behave
typedef struct ghosts{
    int x, y; // Where the ghost is located
    int next_x, next_y; //Next location of the ghost
    int target_x, target_y; //Goal position
    int speed;
    int dir; //Up(0), Down(1), Left(2), Right(3)
    int mode; //Determines if they are chasing, scattering, or frightened
    bool ghostTime;
    bool trapped; //Determine if ghost is in spawn
    bool free; //Allows ghost to bypass WALL2
} Ghost;
Ghost blinky, pinky, inky, clyde; //Moved here to allow reset logic to occur unlike before.
void ghost_time() {
	//Will need timer, aswell as cool effect and other things.
    
}
void reset() {
    clear();
    pacman_x = 1;
    pacman_y = 1;
    pacman_old_x = 1;
    pacman_old_y = 1;
    next_direction_x=1;
    next_direction_y=1;
    score = 0;
    fruit_end_time = 0;
    start_time = 0;
    seconds = 10; //For cherry timer.
    going_once = true;
    start_timer_fruit = false;
    pacman_move = ERR;
    previous_direction_x = KEY_RIGHT;
    previous_direction_y = KEY_DOWN;
    direction = KEY_RIGHT; //https://pacmancode.com/start-positions
    for (int i = 0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH + 1; j++) {
            stage[i][j] = original_stage[i][j];
        }
    }
    //Declare ghosts
   
    blinky.x = 30; 
    blinky.y = 12;
    blinky.next_x = 30;
    blinky.next_y = 12;
    blinky.mode = 1; //Set to scatter

    pinky.x = 30;
    pinky.y = 15;
    pinky.next_x = 30;
    pinky.next_y = 15;
    pinky.mode = 1;
    pinky.trapped = true;
    

    inky.x = 28;
    inky.y = 15;
    inky.next_x = 28;
    inky.next_y = 15;
    inky.mode = 1;
    inky.trapped = true;

    clyde.x = 32;
    clyde.y = 15;
    //-----------------------------------------------//
    pellets_collected = 0; //For the reset logic.
    switch (levels_beaten) {
     case 0: fruit = CHERRY;
     break;
     case 1: fruit = STRAWBERRY;
     break;
     case 2: fruit = ORANGE;
     break;
     case 3: fruit = APPLE;
     break;
     case 4: fruit = MELON;
     break;
     default:
     fruit = KEY;
   }
    start_time = time(NULL);
}
int main(int argc, char * argv[]) { 
    for (int i = 0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH + 1; j++) {
            stage[i][j] = original_stage[i][j];
        }
    }
//Workaround idea, to ensure that pacman only stops at 3 intersection corns when going up or down,
//I will use a previous x and previous and if that is invalid, then keep moving in his respective direction.
   //WIDTH = 60, HEIGHT = 30
   WINDOW *game_win;
   initscr();
   start_color(); //Needed to initialize color pairs. 
   //Needed for codespaces to not highlight text or show grey as the background
   use_default_colors();
   init_pair(1, COLOR_YELLOW, -1); //Key 1, Yellow Character, Black Background.
   init_pair(2, COLOR_BLUE, -1);   //Key 2, Blue Character, Black Background.
   init_pair(3, COLOR_CYAN, -1); //Key 3, Magenta Character, Black Background.
   init_pair(4, COLOR_RED, -1);    //Key 4, Red Character, Black Background.
   init_pair(5, COLOR_GREEN, -1); //Key 5, Green Character, Black Background.
   init_pair(6, COLOR_MAGENTA, -1); // Key 6, "Pinkish" Character, Black Background.
   //Only 8 colors, so fruit is all red.

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
  

   
   game_win = newwin(HEIGHT, WIDTH, 10, 45); //Window placed at X:90, and Y: 10.
  
   curs_set(0); //Cursor hidden from terminal, because that breaks game flow.
   wrefresh(game_win); //Game window refreshed.
   keypad(game_win, TRUE);
   nodelay(game_win, TRUE);//Had to add this, because input is normally blocking.
   
   
    blinky.x = 30; 
    blinky.y = 12;
    blinky.next_x = 30;
    blinky.next_y = 12;
    blinky.mode = 1; //Set to scatter

    
    pinky.x = 30;
    pinky.y = 15;
    pinky.next_x = 30;
    pinky.next_y = 15;
    pinky.mode = 1;
    pinky.trapped = true;
    
    
    inky.x = 28;
    inky.y = 15;
    inky.next_x = 28;
    inky.next_y = 15;
    inky.mode = 1;
    inky.trapped = true;

    
    clyde.x = 32;
    clyde.y = 15;
   //-----------------------------------------------//
   start_time = time(NULL);
   while (running) {
    if (pellets_collected == 150) {
        levels_beaten++;
        reset();
        flushinp(); //https://pubs.opengroup.org/onlinepubs/7908799/xcurses/flushinp.html, Removes all input
    }
    //Score is white
    mvprintw(8, 45, "SCORE: %d", score);
    attron(COLOR_PAIR(1));
    //Lives are yellow.
    mvprintw(8, 97, "LIVES: %d", lives);
    attroff(COLOR_PAIR(1)); //https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/printw.html
	refresh();
    werase(game_win); //Game window is cleared of previous screen.
    mvwaddch(game_win, pacman_y, pacman_x, ' '); //Needed to clear trailing output, similar to pygame.
    //Timer is also like pygame, I couldn't use sleep since sleep blocks the CPU.
    if (start_timer_fruit) {
		if (time(NULL) >= fruit_end_time) {
			stage[17][29] = EATEN;
			start_timer_fruit = false;
		}
	}
    if (score > 70 && going_once) {
		stage[17][29] = fruit;
		start_timer_fruit = true;
		going_once = false;
		fruit_end_time = time(NULL) + 10; //Takes the current time and adds 10 seconds, like in the original game.
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
    if (direction == KEY_UP || direction == KEY_DOWN) {
        previous_direction_y = direction;
    } else if (direction == KEY_LEFT || direction == KEY_RIGHT) {
        previous_direction_x = direction;
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
            pacman_old_x = pacman_x;
            pacman_old_y = pacman_y;
            pacman_y = next_direction_y;
            pacman_x = next_direction_x;
		} else {
            //This is is how the amount of valid paths are calculated, which is important because it determines
            //whether pacman should keep moving left or right (such as in a tunnel), if pacman should calm to a halt (such as when he is at
            //an 3 way intersection when he was moving up or down, or there is no more valid spots to the left or right).
            //It uses a mix of old and new positioning to accomplish this.
           int open_paths = 0;
           if (stage[pacman_y - 1][pacman_x] != WALL && stage[pacman_y - 1][pacman_x] != WALL2) {
               open_paths++;
           }
           if (stage[pacman_y + 1][pacman_x] != WALL && stage[pacman_y + 1][pacman_x] != WALL2) {
               open_paths++;
           }
           if (stage[pacman_y][pacman_x - 1] != WALL && stage[pacman_y][pacman_x - 1] != WALL2) {
               open_paths++;
           }
           if (stage[pacman_y][pacman_x + 1] != WALL && stage[pacman_y][pacman_x + 1] != WALL2) {
               open_paths++;
           }
           //In a normal tunnel, there are only two possible directions, so just keep moving in previous x direction.
           if (open_paths <= 2) {
                //Handles the horizontal tunnels.
                if (direction == KEY_UP  || direction == KEY_DOWN) {
                    if (previous_direction_x == KEY_LEFT && (stage[pacman_y][pacman_x - 1] != WALL && stage[pacman_y][pacman_x - 1] != WALL2)) {
                        pacman_old_x = pacman_x;
                        pacman_old_y = pacman_y;
                        pacman_x -= 1;
                        direction = KEY_LEFT;
                    } else if (previous_direction_x == KEY_RIGHT && (stage[pacman_y][pacman_x + 1] != WALL && stage[pacman_y][pacman_x + 1] != WALL2)) {
                        pacman_old_x = pacman_x;
                        pacman_old_y = pacman_y;
                        pacman_x += 1;
                        direction = KEY_RIGHT;
                    }
                } else {
                    //Handles the vertical tunnels.
                    if (previous_direction_y == KEY_UP && (stage[pacman_y - 1][pacman_x] != WALL && stage[pacman_y - 1][pacman_x] != WALL2)) {
                        if ((stage[pacman_y][pacman_x - 1] == WALL && stage[pacman_y][pacman_x + 1] == WALL)) {
                            pacman_old_x = pacman_x;
                            pacman_old_y = pacman_y;
                            pacman_y -= 1;
                            direction = KEY_UP;
                        }
                    } else if (previous_direction_y == KEY_DOWN && (stage[pacman_y + 1][pacman_x] != WALL && stage[pacman_y + 1][pacman_x] != WALL2)) {
                         if ((stage[pacman_y][pacman_x - 1] == WALL && stage[pacman_y][pacman_x + 1] == WALL)) {
                            pacman_old_x = pacman_x;
                            pacman_old_y = pacman_y;
                            pacman_y += 1;
                            direction = KEY_DOWN;
                         }
                    }
                }
           } else if (open_paths == 3) { //Pacman should only move left or right if his previous direction was from the left or right, not up or down.
                //If pacman's previous x position and y + 1 is a wall, then he was coming from the left or right. Otherwise, he was coming up
                if (direction == KEY_UP  || direction == KEY_DOWN) {
                    if (previous_direction_x == KEY_LEFT && (stage[pacman_old_y + 1][pacman_old_x] == WALL || stage[pacman_old_y + 1][pacman_old_x]==WALL2)) {
                        pacman_old_x = pacman_x;
                        pacman_old_y = pacman_y;
                        pacman_x -= 1;
                        direction = KEY_LEFT;
                    } else if (previous_direction_x == KEY_RIGHT && (stage[pacman_old_y + 1][pacman_old_x] == WALL || stage[pacman_old_y + 1][pacman_old_x]==WALL2)) {
                        pacman_old_x = pacman_x;
                        pacman_old_y = pacman_y;
                        pacman_x += 1;
                        direction = KEY_RIGHT;
                    }
                }
           }
        }
        if (stage[pacman_y][pacman_x] == DOT || stage[pacman_y][pacman_x] == fruit || stage[pacman_y][pacman_x] == SPECIAL) {
                if (stage[pacman_y][pacman_x] == DOT) {
                    score += 10;
                    pellets_collected++;
                } else if (stage[pacman_y][pacman_x] == fruit) {
                    if (stage[pacman_y][pacman_x] == CHERRY) {
                        score += 100;
                    } else if (stage[pacman_y][pacman_x] == STRAWBERRY) {
                        score += 300;
                    } else if (stage[pacman_y][pacman_x] == ORANGE) {
                        score += 500;
                    } else if (stage[pacman_y][pacman_x] == APPLE) {
                        score += 700;
                    } else if (stage[pacman_y][pacman_x] == MELON) {
                        score += 900;
                    } else {
                        score +=1200;
                    }
                } else {
                    ghost_time();
                }
                stage[pacman_y][pacman_x] = EATEN;
        }
	} else {
        //This is how pacman can move through the tunnel and teleport.
        if (next_direction_y == 14) {
            if (next_direction_x < 0) { 
                pacman_x = WIDTH - 1; 
                direction = KEY_LEFT;
        } else if (next_direction_x >= WIDTH) { 
            pacman_x = 0;
            direction = KEY_RIGHT;
        }
      }
    }
	  //Ghost Section///////////////////////////

    //blinky:
    //Most aggrressive ghost
    //Gets progressively faster the less dots there are
    //In scatter mode, he targets the top right corner
    //In chase mode, he tracks the current position of pacman

        //Determines when to start chasing pacman
        if(time(NULL) - start_time > 10){
            blinky.mode = 0;
        }

        //Chase mode
        if(blinky.mode == 0){
            blinky.target_x = pacman_x;
            blinky.target_y = pacman_y;
        }
        //Scatter mode
        else if((time(NULL) - start_time <= 10)|| blinky.mode == 1){
            //(58, 1) for top-right corner
            blinky.target_x = 58;
            blinky.target_y = 1;
        }
        //Frightened mode
        else if(blinky.mode == 2){
            //logic still needed
        }
    int pathB[4] = {0, 0, 0, 0}; //Up Down Left Right (For Blinky)
    if(stage[blinky.y - 1][blinky.x] != WALL && stage[blinky.y - 1][blinky.x] != WALL2) pathB[0] = 1; //Up
    if(stage[blinky.y + 1][blinky.x] != WALL && stage[blinky.y + 1][blinky.x] != WALL2) pathB[1] = 1; //Down
    if(stage[blinky.y][blinky.x - 1] != WALL && stage[blinky.y][blinky.x - 1] != WALL2) pathB[2] = 1; //Left
    if(stage[blinky.y][blinky.x + 1] != WALL && stage[blinky.y][blinky.x + 1] != WALL2) pathB[3] = 1; //Right

    double best_dis = 100000.0;
    int dir;
    for(int i = 0; i<4; i++){ //Check each path and determine which one is best
        //Prevents U-turns
        if((i == 0 && blinky.dir == 1) || (i == 1 && blinky.dir == 0) || (i == 2 && blinky.dir == 3) || (i == 3 && blinky.dir == 2)){
            continue;
        }
        
        int test_x = blinky.x; //Temp variable for potential x positions
        int test_y = blinky.y; //Temp variable for potential y postions

        // Predict where ghost would be in this direction
        if(i == 0) test_y--;      // Up
        else if(i == 1) test_y++; // Down
        else if(i == 2) test_x--; // Left
        else if(i == 3) test_x++; // Right

        if(pathB[i] == 1){
            double dis = pow(blinky.target_x - test_x,2) + pow(blinky.target_y - test_y, 2);
            if(dis<best_dis){
                dir = i; //Store the best direction
                best_dis = dis;
            }
            else{ //If path not valid, move onto next
                continue;
            }
        }
    }
    blinky.dir = dir; //Set the direction
    
    //Usage of the teleporter
    if (blinky.y == 14) {
		if (blinky.x - 1 < 0) { 
			blinky.x = WIDTH - 1;
			blinky.dir = 2;
    } else if (blinky.x + 1 >= WIDTH) { 
        blinky.x = 0;
        blinky.dir = 3;
    }
}    
        switch(blinky.dir){ //Move along the direction
            case 0:
            blinky.y--;
            break;
            case 1:
            blinky.y++;
            break;
            case 2:
            blinky.x--;
            break;
            case 3:
            blinky.x++;
            break;
        }

    //pinky:
    //During the chase mode tracks pacman's position 4 spaces ahead of him
    //During scatter, runs to the top left corner of the map

    //Determines when to exit spawn
        if(pinky.trapped){ //Ensures pinky escapes before progressing through the game
        if(time(NULL) - start_time < 10){
            pinky.target_x = 30;
            pinky.target_y = 12;
        }
        if(time(NULL) - start_time >= 10){
            pinky.free = true;
        }
        if (pinky.x == 30 && pinky.y == 12) {
            pinky.trapped = false;
        }
    }
    else{
        //Disables the ability to move through ghost house
        if(!pinky.trapped){
            pinky.free = false;
        }
        //Time needed to pass to start chasing
        if(time(NULL) - start_time > 20){
            pinky.mode = 0;
        }
        //Determine the direction of pacman for chase mode
        int dx = 0, dy = 0;
        if (direction == KEY_UP)    dy = -1;
        if (direction == KEY_DOWN)  dy = 1;
        if (direction == KEY_LEFT)  dx = -1;
        if (direction == KEY_RIGHT) dx = 1;
        //Chase mode
        if(pinky.mode == 0){
            //Determine the direction pacman is moving and set target 4 spaces ahead
            //Calculate the distance from pacman and pick position closest to him
            pinky.target_x = pacman_x + (4*dx);
            pinky.target_y = pacman_y + (4*dy);
        }
        //Scatter mode
        else if(pinky.mode == 1){
            //(1, 1) for top-left corner
            pinky.target_x = 1;
            pinky.target_y = 1;
        }
        //Frightened mode
        else if(pinky.mode == 2){
            //logic still needed
        }
    }
        int pathP[4] = {0, 0, 0, 0}; //Up Down Left Right (For Pinky)
        if(stage[pinky.y - 1][pinky.x] != WALL && (stage[pinky.y - 1][pinky.x] != WALL2 || pinky.free)) pathP[0] = 1; //Up
        if(stage[pinky.y + 1][pinky.x] != WALL && stage[pinky.y + 1][pinky.x] != WALL2) pathP[1] = 1; //Down
        if(stage[pinky.y][pinky.x - 1] != WALL && stage[pinky.y][pinky.x - 1] != WALL2) pathP[2] = 1; //Left
        if(stage[pinky.y][pinky.x + 1] != WALL && stage[pinky.y][pinky.x + 1] != WALL2) pathP[3] = 1; //Right

        double best_disP = 100000.0; //Best distance for pinky
        int dirP; //Used for Pinky's direction
    
        for(int i = 0; i<4; i++){ //Check each path and determine which one is best
            //Prevents U-turns
            if((i == 0 && pinky.dir == 1) || (i == 1 && pinky.dir == 0) || (i == 2 && pinky.dir == 3) || (i == 3 && pinky.dir == 2)){
                continue;
            }
        
            int test_x = pinky.x; //Temp variable for potential x positions
            int test_y = pinky.y; //Temp variable for potential y postions

            // Predict where ghost would be in this direction
            if(i == 0) test_y--;      // Up
            else if(i == 1) test_y++; // Down
            else if(i == 2) test_x--; // Left
            else if(i == 3) test_x++; // Right

            if(pathP[i] == 1){
                double dis = pow(pinky.target_x - test_x,2) + pow(pinky.target_y - test_y, 2);
                if(dis<best_disP){
                    dirP = i; //Store the best direction
                    best_disP = dis;
                }
                else{ //If path not valid, move onto next
                    continue;
                }
            }
        }
    pinky.dir = dirP; //Set the direction
    
    switch(pinky.dir){ //Move along the direction
            case 0:
            pinky.y--;
            break;
            case 1:
            pinky.y++;
            break;
            case 2:
            pinky.x--;
            break;
            case 3:
            pinky.x++;
            break;
        }
    //Usage of the teleporter
    if (pinky.y == 14) {
		if (pinky.x - 1 < 0) { 
			pinky.x = WIDTH - 1;
			pinky.dir = 2;
    } else if (pinky.x + 1 >= WIDTH) { 
        pinky.x = 0;
        pinky.dir = 3;
    }
}
//inky:
//30 pellets (300 points) must be eaten for him to be released
//Targets the bottom right corner during scatter mode
//Targets 2 spaces in front of pacman and draws a vector from where blinky is located to that space
//Proceeds to double the length of the vector, with the ending position being his target.

if(inky.trapped){
    if( score < 300){
        inky.target_x = 30;
        inky.target_y = 12;
    }
    if(score >= 300){
        inky.free = true;
    }
    if (inky.x == 30 && inky.y == 12) {
            inky.trapped = false;
        }
    }
else{
        //Disables the ability to move through ghost house
        if(!inky.trapped){
            inky.free = false;
        }

        //Score of 700 or more needed to chasing
        if(score >= 700){
            inky.mode = 0;
        }

        //Determine the direction of pacman for chase mode
        int dx_i = 0, dy_i = 0;
        if (direction == KEY_UP)    dy_i = -1;
        if (direction == KEY_DOWN)  dy_i = 1;
        if (direction == KEY_LEFT)  dx_i = -1;
        if (direction == KEY_RIGHT) dx_i = 1;

        //Chase mode
        if(inky.mode == 0){
            //Determine the direction pacman is moving and set a temp variable to look to spaces ahead
             int temp_x = pacman_x + (2*dx_i);
             int temp_y = pacman_y + (2*dy_i);
            //Subtract the distance between Blinky and double the position of temp point
            inky.target_x = (2*temp_x) - blinky.x;
            inky.target_y = (2*temp_y) - blinky.y;
        }
        //Scatter mode
        else if(inky.mode == 1){
            //(58, 28) for bottom-right corner
            inky.target_x = 58;
            inky.target_y = 28;
        }
        //Frightened mode
        else if(inky.mode == 2){
            //logic still needed
        }
}
        int pathI[4] = {0, 0, 0, 0}; //Up Down Left Right (For Inky)
        if(stage[inky.y - 1][inky.x] != WALL && (stage[inky.y - 1][inky.x] != WALL2 || inky.free)) pathI[0] = 1; //Up
        if(stage[inky.y + 1][inky.x] != WALL && stage[inky.y + 1][inky.x] != WALL2) pathI[1] = 1; //Down
        if(stage[inky.y][inky.x - 1] != WALL && stage[inky.y][inky.x - 1] != WALL2) pathI[2] = 1; //Left
        if(stage[inky.y][inky.x + 1] != WALL && stage[inky.y][inky.x + 1] != WALL2) pathI[3] = 1; //Right

        double best_disI = 100000.0; //Best distance for inky
        int dirI; //Used for inky's direction

        for(int i = 0; i<4; i++){ //Check each path and determine which one is best
            //Prevents U-turns
            if((i == 0 && inky.dir == 1) || (i == 1 && inky.dir == 0) || (i == 2 && inky.dir == 3) || (i == 3 && inky.dir == 2)){
                continue;
            }
        
            int test_x = inky.x; //Temp variable for potential x positions
            int test_y = inky.y; //Temp variable for potential y postions

            // Predict where ghost would be in this direction
            if(i == 0) test_y--;      // Up
            else if(i == 1) test_y++; // Down
            else if(i == 2) test_x--; // Left
            else if(i == 3) test_x++; // Right

            if(pathI[i] == 1){
                double dis = pow(inky.target_x - test_x,2) + pow(inky.target_y - test_y, 2);
                if(dis<best_disI){
                    dirI = i; //Store the best direction
                    best_disI = dis;
                }
                else{ //If path not valid, move onto next
                    continue;
                }
            }
        }
    inky.dir = dirI; //Set the direction

    switch(inky.dir){ //Move along the direction
            case 0:
            inky.y--;
            break;
            case 1:
            inky.y++;
            break;
            case 2:
            inky.x--;
            break;
            case 3:
            inky.x++;
            break;
        }
    //Usage of the teleporter
    if (inky.y == 14) {
		if (inky.x - 1 < 0) { 
			inky.x = WIDTH - 1;
			inky.dir = 2;
        }else if (inky.x + 1 >= WIDTH) { 
            inky.x = 0;
            inky.dir = 3;
        }
    }
//clyde:
//When in scatter mode, targets the bottom left of the map
//When in chase mode, if 8 positions or further from pacman, acts like blinky
//If within 8 positions of pacman, retreats to his corner

if(clyde.trapped){
    if( score < 600){
        clyde.target_x = 30;
        clyde.target_y = 12;
    }
    if(score >= 600){
        clyde.free = true;
    }
    if (inky.x == 30 && inky.y == 12) {
            clyde.trapped = false;
        }
    }
else{
    //Disables the ability to move through ghost house
        if(!clyde.trapped){
            clyde.free = false;
        }

        //Score of 1000 or more needed to chasing
        if(score >= 1000){
            clyde.mode = 0;
        }
        
        //Determine the direction of pacman for chase mode
        int dx_c = 0, dy_c = 0;
        if (direction == KEY_UP)    dy_c = -1;
        if (direction == KEY_DOWN)  dy_c = 1;
        if (direction == KEY_LEFT)  dx_c = -1;
        if (direction == KEY_RIGHT) dx_c = 1;

        //Chase mode
        if(clyde.mode == 0){
            clyde.target_x = pacman_x + (8*dx_c);
            clyde.target_y = pacman_y + (8*dy_c);
        }
        //Scatter mode
        else if(clyde.mode == 1){
            //(1, 28) for bottom-left corner
            inky.target_x = 1;
            inky.target_y = 28;
        }
        //Frightened mode
        else if(inky.mode == 2){
            //logic still needed
        }
}
        int pathC[4] = {0, 0, 0, 0}; //Up Down Left Right (For Inky)
        if(stage[clyde.y - 1][clyde.x] != WALL && (stage[clyde.y - 1][clyde.x] != WALL2 || clyde.free)) pathC[0] = 1; //Up
        if(stage[clyde.y + 1][clyde.x] != WALL && stage[clyde.y + 1][clyde.x] != WALL2) pathC[1] = 1; //Down
        if(stage[clyde.y][clyde.x - 1] != WALL && stage[clyde.y][clyde.x - 1] != WALL2) pathC[2] = 1; //Left
        if(stage[clyde.y][clyde.x + 1] != WALL && stage[clyde.y][clyde.x + 1] != WALL2) pathC[3] = 1; //Right

        double best_disC = 10000.0;
        int dirC;
        for(int i = 0; i<4; i++){ //Check each path and determine which one is best
            //Prevents U-turns
            if((i == 0 && clyde.dir == 1) || (i == 1 && clyde.dir == 0) || (i == 2 && clyde.dir == 3) || (i == 3 && clyde.dir == 2)){
                continue;
            }
        
            int test_x = clyde.x; //Temp variable for potential x positions
            int test_y = clyde.y; //Temp variable for potential y postions

            // Predict where ghost would be in this direction
            if(i == 0) test_y--;      // Up
            else if(i == 1) test_y++; // Down
            else if(i == 2) test_x--; // Left
            else if(i == 3) test_x++; // Right

            if(pathI[i] == 1){
                double dis = pow(clyde.target_x - test_x,2) + pow(clyde.target_y - test_y, 2);
                if(dis<best_disI){
                    dirC = i; //Store the best direction
                    best_disC = dis;
                }
                else{ //If path not valid, move onto next
                    continue;
                }
            }
        }
        clyde.dir = dirC; //Set the direction
        

    

//clyde:    
    mvwaddch(game_win, blinky.y, blinky.x, 'G' | COLOR_PAIR(4));
    mvwaddch(game_win, pinky.y, pinky.x, 'G' | COLOR_PAIR(6));
    mvwaddch(game_win, inky.y, inky.x, 'G' | COLOR_PAIR(3));
    mvwaddch(game_win, clyde.y, clyde.x, 'G' | COLOR_PAIR(1));
//End of ghost section//
    mvwaddch(game_win, pacman_y, pacman_x, 'P' | COLOR_PAIR(1)); //https://docs.oracle.com/cd/E86824_01/html/E54767/mvwaddch-3curses.html
    wrefresh(game_win); //Game window refreshed.
    napms(100); 
   }

   endwin();
   return 0;
}

//Note, to compile gcc -o pacman pacman.c -lncurses -lm
