/*
Authors: Aiden Ocasio and Jhan Gomez
Instructor: Professor Fernando
Course: Computer Science 355
Purpose: To demonstrate how a simple and fun pacman game in C can be done utilizing the ncurses library,
the math libary, simple and advanced C code, and just general knowledge of game design.*/
#include <stdio.h>
#include <ncurses.h> //Ncurses library ported in
#include <stdbool.h>
#include <time.h> //Time to handle start screen. https://stackoverflow.com/questions/17167949/how-can-i-use-a-timer-in-c 
#include <stdlib.h> //Needed for exit and random.
#include <math.h> //Needed for distance
//Splash screen might be added later.
#include <unistd.h> //For sleep.
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
/* 
    Width and height from: https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/keys.html
    Pacman info on fruits: https://pacman.fandom.com/wiki/fruits
*/
char fruit = CHERRY;
int pacman_x = 1;
int pacman_y = 1;
int pacman_old_x = 1;
int pacman_old_y = 1;
int next_direction_x=1;
int next_direction_y=1;
int score = 0;
int highest_score = 0;
int lives = 3;
int fruit_end_time = 0;
time_t start_time = 0;
int seconds = 10; //For cherry timer.
int pellets_collected = 0; //For the reset logic.
bool going_once = true;
bool start_timer_fruit = false;
int pacman_move;
int previous_direction_x = KEY_RIGHT;
int queued_direction = ERR;
int previous_direction_y = KEY_DOWN;
int direction = KEY_RIGHT; //https://pacmancode.com/start-positions
int levels_beaten = 0;
bool running = true;
int frightened_timer;
bool frightened;
WINDOW *game_win;
//Maze for the game, declared as a 2D Matrix - Jhan
char original_stage[HEIGHT][WIDTH + 1] =  { 
    "############################################################", 
    "#@   ........              #####              .......     @#", 
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
    "#.          #####           ####           #####          .#",
    "#.#########################.####.#########################.#",
    "#@.........      ...........    ...........       ........@#", 
    "############################################################"  
};	
char stage[HEIGHT][WIDTH + 1];

//Defines how the ghosts behave -Aiden
typedef struct ghosts{
    int x, y; // Where the ghost is located
    int prev_x, prev_y; //Previous location of the ghost
    int target_x, target_y; //Goal position
    int speed;
    int dir; //Up(0), Down(1), Left(2), Right(3)
    int mode; //Determines if they are chasing, scattering, frightened, or eaten
    time_t time;
    bool trapped; //Determine if ghost is in spawn
    bool free; //Allows ghost to bypass WALL2
} Ghost;
Ghost blinky, pinky, inky, clyde; //Moved here to allow reset logic to occur unlike before.
//Handles when ghosts are in special stage - Aiden
void ghost_time() {
	//Will need timer, aswell as cool effect and other things.
    frightened_timer = 60;
    frightened = true;
}
//Starting and restart from game over screen - Jhan
void coin_inserted(int countdown) {
     for (int i = countdown; i >= 1; i--) {
        clear();
        mvprintw(20, 70, "Coin inserted! Game starting in: %d seconds ", i);
        /* THIS ASCII ART IS PROVIDED FREE COURTESY OF https://www.asciiart.eu/video-games/pacman */
        mvprintw(21, 60, "             _ __   __ _  ___ _ __ ___   __ _ _ __   ");
        mvprintw(22, 60, "            | '_ \\ / _` |/ __| '_ ` _ \\ / _` | '_ \\  ");
        mvprintw(23, 60, "            | |_) | (_| | (__| | | | | | (_| | | | | ");
        mvprintw(24, 60, "            | .__/ \\__,_|\\___|_| |_| |_|\\__,_|_| |_| ");
        mvprintw(25, 60, "            |_|                                      ");
        mvprintw(26, 60, "            Please wait. Your game will start shortly...");
        mvprintw(27, 45, "Demonstration by Aiden Ocasio and Jhan Gomez. This game is not endorsed by Bandai Namco Entertainment.");
        refresh(); 
        sleep(1);
    }
    clear();
}
//General reset and restart logic, including reset of score, points, and positions. - Jhan
void reset(int gameMode) {
    clear();
    nodelay(stdscr, FALSE);
    if (gameMode == 2) {
        for (int i = 0; i<HEIGHT; i++) {
            for (int j=0; j<WIDTH + 1; j++) {
                stage[i][j] = original_stage[i][j];
            }
        }  
        score = 0;
    }
    if (gameMode == 1) {
        for (int i = 0; i<HEIGHT; i++) {
            for (int j=0; j<WIDTH + 1; j++) {
                stage[i][j] = original_stage[i][j];
            }
        }  
        int get = ERR;  
        mvprintw(20, 90, "Game Over.");
        mvprintw(21, 75, "Your score for this round was: %d points.", score);
        mvprintw(22, 75, "Your highest score was: %d points.", highest_score);
        mvprintw(23, 75, "You were able to complete: %d stages.", levels_beaten);
        mvprintw(24, 75, "Press enter to insert a coin...");
        refresh();
        score = 0;
        highest_score = 0;
        lives = 3;
        levels_beaten = 0;
        get = getch();
        if (get == '\n' || get == '\r') {
            clear();
            coin_inserted(5);
        }
    } else {
        score = score;
    }
    clear();
    //Resetting logic. -Jhan
    pacman_x = 1;
    pacman_y = 1;
    pacman_old_x = 1;
    pacman_old_y = 1;
    next_direction_x=1;
    next_direction_y=1;
    fruit_end_time = 0;
    start_time = 0;
    seconds = 10; //For cherry timer.
    going_once = true;
    start_timer_fruit = false;
    pacman_move = ERR;
    previous_direction_x = KEY_RIGHT;
    previous_direction_y = KEY_DOWN;
    direction = KEY_RIGHT; //https://pacmancode.com/start-positions
    queued_direction = ERR;
      

   //Initializing ghost locations and modes. -Aiden
    blinky.x = 30; 
    blinky.y = 12;
    blinky.prev_x = 30;
    blinky.prev_y = 12;
    blinky.mode = 1; //Set to scatter
    blinky.time = time(NULL);

    pinky.x = 30;
    pinky.y = 15;
    pinky.prev_x = 30;
    pinky.prev_y = 15;
    pinky.mode = 1;
    pinky.trapped = true;
    pinky.free = false; 
    pinky.time = time(NULL);

    inky.x = 28;
    inky.y = 15;
    inky.prev_x = 28;
    inky.prev_y = 15;
    inky.mode = 1;
    inky.trapped = true;
    inky.free = false;
    inky.time = time(NULL); 

    clyde.x = 32;
    clyde.y = 15;
    clyde.prev_x = 32;
    clyde.prev_y = 15;
    clyde.mode = 1;
    clyde.trapped = true;
    clyde.free = false;
    clyde.time = time(NULL);
    pellets_collected = 0; //For the reset logic.
    //Switch determines what fruit to use. -Jhan
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
   //Blocking movement is disabled for main game. -Jhan
    nodelay(stdscr, TRUE);
    start_time = time(NULL);
    running = true;
}
int main(int argc, char * argv[]) {
    srand(time(NULL)); //Needed so ghosts move randomly every frame
    //Sets the stage using a nested for loop. -Jhan
    for (int i = 0; i<HEIGHT; i++) {
        for (int j=0; j<WIDTH + 1; j++) {
            stage[i][j] = original_stage[i][j];
        }
    }
   //WIDTH = 60, HEIGHT = 30, Because Terminal displays characters bigger in terms of height vs width. -Jhan
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
   init_pair(7, COLOR_WHITE, -1); //Key 7, White Character, Black Background.
   //Only 8 colors, so fruit is all red.

   //NOTE: While in a normal pacman game, the dots would be yellow. I dislike how much the yellow blends
   //with pacman because you cannot change the opacity of a color in ncurses. Or if is, it adds unnecessary bloat. -Jhan
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
   
   
    //Initializing ghost locations and modes. -Aiden
    blinky.x = 30; 
    blinky.y = 12;
    blinky.prev_x = 30;
    blinky.prev_y = 12;
    blinky.mode = 1; //Set to scatter
    blinky.time = time(NULL);

    pinky.x = 30;
    pinky.y = 15;
    pinky.prev_x = 30;
    pinky.prev_y = 15;
    pinky.mode = 1;
    pinky.trapped = true;
    pinky.free = false; 
    pinky.time = time(NULL);

    inky.x = 28;
    inky.y = 15;
    inky.prev_x = 28;
    inky.prev_y = 15;
    inky.mode = 1;
    inky.trapped = true;
    inky.free = false;
    inky.time = time(NULL); 

    clyde.x = 32;
    clyde.y = 15;
    clyde.prev_x = 32;
    clyde.prev_y = 15;
    clyde.mode = 1;
    clyde.trapped = true;
    clyde.free = false;
    clyde.time = time(NULL);
   //-----------------------------------------------//
   coin_inserted(10); //To give the user some time before the game begins. -Jhan
   start_time = time(NULL);
   while (running) {
    if (pellets_collected == 150) { //Mission complete logic -Jhan
        levels_beaten++;
        reset(2);
        flushinp(); //https://pubs.opengroup.org/onlinepubs/7908799/xcurses/flushinp.html, Removes all input
    }
    //Score is white
    mvprintw(8, 45, "SCORE: %d ", score);
    mvprintw(7, 68, "HIGH SCORE: %d ", highest_score);
    attron(COLOR_PAIR(1));
    //Lives are yellow.
    mvprintw(8, 97, "LIVES: %d ", lives);
    attroff(COLOR_PAIR(1)); //https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/printw.html
	refresh();
    werase(game_win); //Game window is cleared of previous screen.
    mvwaddch(game_win, pacman_y, pacman_x, ' '); //Needed to clear trailing output, similar to pygame.
    //Timer is also like pygame, I couldn't use sleep since sleep blocks the CPU -Jhan.
    if (start_timer_fruit) {
		if (time(NULL) >= fruit_end_time) {
			stage[17][29] = EATEN;
			start_timer_fruit = false;
		}
	}
    //Will show the fruit only once for every 70 points. -Jhan
    if (score > 70 && going_once) {
		stage[17][29] = fruit;
		start_timer_fruit = true;
		going_once = false;
		fruit_end_time = time(NULL) + 10; //Takes the current time and adds 10 seconds, like in the original game.
	}
    for (int y = 0; y < HEIGHT; y++) { //This loop draws the stage as it evolves. Starting with the y axis, or the columns. -Jhan
		for (int x = 0; x < WIDTH; x++) { //Same as above, but for x axis
			if (stage[y][x] == WALL) {
			    mvwaddch(game_win, y, x, stage[y][x] | COLOR_PAIR(2)); //Stage drawn.
			} else if (stage[y][x] ==  DOT) {
				mvwaddch(game_win, y, x, stage[y][x] | COLOR_PAIR(3)); //Stage drawn.	
			} else if (stage[y][x] == fruit) {
				mvwaddch(game_win, y, x, stage[y][x] | COLOR_PAIR(4)); //Stage drawn.	
			} else if (stage[y][x] == SPECIAL) {
				mvwaddch(game_win, y, x, stage[y][x] | COLOR_PAIR(5)); //Stage drawn.	
			} else {
				mvwaddch(game_win, y, x, stage[y][x]); //Stage drawn.
			}
		}
	}
    //Needed for intersection logic. -Jhan
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
    //Movement along the y axis is queued to give that authentic pacman feel. -Jhan
    switch(pacman_move) {
        case KEY_UP: 
          if (stage[pacman_y - 1][pacman_x]!=WALL && stage[pacman_y - 1][pacman_x]!=WALL2) {
                direction = pacman_move;
                queued_direction = ERR;
            } else {
                queued_direction = pacman_move;
            }
            break;
        case KEY_DOWN:
            if (stage[pacman_y + 1][pacman_x]!=WALL && stage[pacman_y + 1][pacman_x]!=WALL2) {
                direction = pacman_move;
                queued_direction = ERR;
            } else {
                queued_direction = pacman_move;
            }
            break;
        case KEY_LEFT:
        case KEY_RIGHT:
             direction = pacman_move;
             break;
        case 27: 
             running = false;
             break;
    }

    switch(direction) { //Switched because of collision conflicts, the program needs to predict what happens next rather
		//than just moving and then calculating conflict. -Jhan
        case KEY_UP:    next_direction_y--; break;
        case KEY_DOWN:  next_direction_y++; break;
        case KEY_LEFT:  next_direction_x--; break;
        case KEY_RIGHT: next_direction_x++; break;
    }
    //Pacman movement logic, determines when to execute queued direction. -Jhan
    if (queued_direction == KEY_UP || queued_direction == KEY_DOWN) { 
        int test_x = next_direction_x; 
        int test_y = next_direction_y; 
        if (queued_direction == KEY_UP) { 
            test_y--; 
        } else if (queued_direction == KEY_DOWN) { 
            test_y++; 
        } if (stage[test_y][test_x] != WALL && stage[test_y][test_x] != WALL2) { 
            direction = queued_direction; 
            queued_direction = ERR; 
        } 
    }
    //Prevents an out of bound error for the program. -Jhan
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
            //It uses a mix of old and new positioning to accomplish this. -Jhan
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
                //If pacman's previous x position and y + 1 is a wall, then he was coming from the left or right. Otherwise, he was coming up. -Jhan
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
	} else {
        //This is how pacman can move through the tunnel and teleport. -Jhan
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
    //Handles points for fruits. -Jhan
    if (stage[pacman_y][pacman_x] == DOT || stage[pacman_y][pacman_x] == fruit || stage[pacman_y][pacman_x] == SPECIAL) {
                if (stage[pacman_y][pacman_x] == DOT) {
                    score += 10;
                    highest_score += 10;
                    pellets_collected++;
                } else if (stage[pacman_y][pacman_x] == fruit) {
                    if (stage[pacman_y][pacman_x] == CHERRY) {
                        score += 100;
                        highest_score += 100;
                    } else if (stage[pacman_y][pacman_x] == STRAWBERRY) {
                        score += 300;
                        highest_score += 300;
                    } else if (stage[pacman_y][pacman_x] == ORANGE) {
                        score += 500;
                        highest_score += 500;
                    } else if (stage[pacman_y][pacman_x] == APPLE) {
                        score += 700;
                        highest_score += 700;
                    } else if (stage[pacman_y][pacman_x] == MELON) {
                        score += 900;
                        highest_score += 900;
                    } else {
                        score +=1200;
                        highest_score += 1200;
                    }
                } else {
                    ghost_time();
                }
                stage[pacman_y][pacman_x] = EATEN;
    }
	 //Ghost Section///////////////////////////

    //blinky:
    //Most aggrressive ghost
    //Gets progressively faster the less dots there are
    //In scatter mode, he targets the top right corner
    //In chase mode, he tracks the current position of pacman -Aiden
        
    //Eaten (Return to house)
    if(blinky.mode == 3){
        blinky.free = true;
        blinky.target_x = 30;
        blinky.target_y = 15;
        }
    //Frightened mode
    else if(blinky.mode == 2){
        if(abs(blinky.x - blinky.target_x) < 1 && abs(blinky.y - blinky.target_y) < 1){
            blinky.target_x = rand() % WIDTH;
            blinky.target_y = rand() % HEIGHT;
        }
    }
    else{
        //Determines when to start chasing pacman
        if (blinky.mode < 2) {
            if (time(NULL) - start_time > 10) {
                blinky.mode = 0; // Chase
            } else {
                blinky.mode = 1; // Scatter
            }
        }
        //Chase mode
        if(blinky.mode == 0){
            blinky.target_x = pacman_x;
            blinky.target_y = pacman_y;
        }
        //Scatter mode
        if((time(NULL) - start_time <= 10)|| blinky.mode == 1){
            //(58, 1) for top-right corner
            blinky.target_x = 58;
            blinky.target_y = 1;
        }
    }
    
        //Check if in the house
        if (blinky.mode == 3 && blinky.x == 30 && blinky.y == 15) {
            blinky.trapped = true;
        }
        //How to escape
        if(blinky.trapped){
            blinky.target_x = 30;
            blinky.target_y = 12;

            if ( blinky.y <= 12) {
                blinky.trapped = false;
                blinky.free = false;
                blinky.mode = 0;
            }
        }
        

    blinky.prev_x = blinky.x;
    blinky.prev_y = blinky.y;
    int pathB[4] = {0, 0, 0, 0}; //Up Down Left Right (For Blinky)
    
    if(stage[blinky.y - 1][blinky.x] != WALL && (stage[blinky.y - 1][blinky.x] != WALL2 || blinky.free)) pathB[0] = 1; //Up
    if(stage[blinky.y + 1][blinky.x] != WALL && (stage[blinky.y + 1][blinky.x] != WALL2 || blinky.free)) pathB[1] = 1; //Down
    if(stage[blinky.y][blinky.x - 1] != WALL && stage[blinky.y][blinky.x - 1] != WALL2) pathB[2] = 1; //Left
    if(stage[blinky.y][blinky.x + 1] != WALL && stage[blinky.y][blinky.x + 1] != WALL2) pathB[3] = 1; //Right

    double best_dis = 100000.0;
    int dir = blinky.dir;
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
    
        //Eaten (Return to house)
        if(pinky.mode == 3){
            pinky.free = true;
            pinky.target_x = 30;
            pinky.target_y = 15;
            
            //Entering house (only when eaten)
            if (abs(pinky.x - 30) <= 1 && abs(pinky.y - 15) <= 1) {
                pinky.trapped = true;
                pinky.time = time(NULL);
            }
        }
        //Frightened mode
        else if(pinky.mode == 2){
             if(abs(pinky.x - pinky.target_x) < 1 && abs(pinky.y - pinky.target_y) < 1){
                pinky.target_x = rand() % WIDTH;
                pinky.target_y = rand() % HEIGHT;
            }
        }
        else{
            //Scatter/chase timer
            if (pinky.mode < 2){
                if (time(NULL) - start_time > 10){
                    pinky.mode = 0; // chase
                }
            else{
                pinky.mode = 1; // scatter
            }
        }
        //Chase mode
        if (pinky.mode == 0) {
            int dx = 0, dy = 0;
            if (direction == KEY_UP)    dy = -1;
            if (direction == KEY_DOWN)  dy = 1;
            if (direction == KEY_LEFT)  dx = -1;
            if (direction == KEY_RIGHT) dx = 1;

            pinky.target_x = pacman_x + 4 * dx;
            pinky.target_y = pacman_y + 4 * dy;
        }

    //Scatter mode
        else if (pinky.mode == 1) {
            pinky.target_x = 1;
            pinky.target_y = 1;
        }
    }

        //Escape logic
        if(pinky.trapped){
            
            pinky.free = true;
            pinky.target_x = 30;
            pinky.target_y = 12;
            
            if(pinky.y <= 12) {
                pinky.trapped = false;
                pinky.free = false;
                pinky.mode = 1; // scatter after eaten
            }
        }
        
        pinky.prev_x = pinky.x;
        pinky.prev_y = pinky.y;

        int pathP[4] = {0, 0, 0, 0}; //Up Down Left Right (For Pinky)
        if(stage[pinky.y - 1][pinky.x] != WALL && (stage[pinky.y - 1][pinky.x] != WALL2 || pinky.free)) pathP[0] = 1; //Up
        if(stage[pinky.y + 1][pinky.x] != WALL && (stage[pinky.y + 1][pinky.x] != WALL2 || pinky.free)) pathP[1] = 1; //Down
        if(stage[pinky.y][pinky.x - 1] != WALL && stage[pinky.y][pinky.x - 1] != WALL2) pathP[2] = 1; //Left
        if(stage[pinky.y][pinky.x + 1] != WALL && stage[pinky.y][pinky.x + 1] != WALL2) pathP[3] = 1; //Right

        double best_disP = 100000.0; //Best distance for pinky
        int dirP = pinky.dir; //Used for Pinky's direction
    
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
    if( pellets_collected < 30){
        inky.target_x = 30;
        inky.target_y = 12;
    }
    if(pellets_collected >= 30){
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

        //70 dots eaten or more needed to start chasing
        if(pellets_collected >= 70){
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
            if(abs(inky.x - inky.target_x) < 1 && abs(inky.y - pinky.target_y) < 1){
                inky.target_x = rand() % WIDTH;
                inky.target_y = rand() % HEIGHT;
            }
        }
        else if(inky.mode == 3){
            inky.free = true;
            inky.target_x = 30;
            inky.target_y = 15;
        }
}
        inky.prev_x = inky.x;
        inky.prev_y = inky.y;
        int pathI[4] = {0, 0, 0, 0}; //Up Down Left Right (For Inky)
        if(stage[inky.y - 1][inky.x] != WALL && (stage[inky.y - 1][inky.x] != WALL2 || inky.free)) pathI[0] = 1; //Up
        if(stage[inky.y + 1][inky.x] != WALL && stage[inky.y + 1][inky.x] != WALL2) pathI[1] = 1; //Down
        if(stage[inky.y][inky.x - 1] != WALL && stage[inky.y][inky.x - 1] != WALL2) pathI[2] = 1; //Left
        if(stage[inky.y][inky.x + 1] != WALL && stage[inky.y][inky.x + 1] != WALL2) pathI[3] = 1; //Right

        double best_disI = 100000.0; //Best distance for inky
        int dirI = inky.dir; //Used for inky's direction

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
    if( pellets_collected < 60){
        clyde.target_x = 30;
        clyde.target_y = 12;
    }
    if(pellets_collected >= 60){
        clyde.free = true;
    }
    if (clyde.x == 30 && clyde.y == 12) {
            clyde.trapped = false;
        }
    }
else{
    //Disables the ability to move through ghost house
        if(!clyde.trapped){
            clyde.free = false;
        }

        //100 dots eaten or more needed to start chasing
        if(pellets_collected >= 100){
            clyde.mode = 0;
        }

        //Chase mode
        if(clyde.mode == 0){
            clyde.target_x = pacman_x;
            clyde.target_y = pacman_y;
        }
        //Scatter mode
        else if(clyde.mode == 1){
            //(1, 28) for bottom-left corner
            clyde.target_x = 1;
            clyde.target_y = 28;
        }
        //Frightened mode
        else if(clyde.mode == 2){
            if(abs(clyde.x - clyde.target_x) < 1 && abs(clyde.y - clyde.target_y) < 1){
                clyde.target_x = rand() % WIDTH;
                clyde.target_y = rand() % HEIGHT;
            }
        }
        else if(clyde.mode == 3){
            clyde.target_x = 30;
            clyde.target_y = 15;
        }
}
        clyde.prev_x = clyde.x;
        clyde.prev_y = clyde.y;
        int pathC[4] = {0, 0, 0, 0}; //Up Down Left Right (For Inky)
        if(stage[clyde.y - 1][clyde.x] != WALL && (stage[clyde.y - 1][clyde.x] != WALL2 || clyde.free)) pathC[0] = 1; //Up
        if(stage[clyde.y + 1][clyde.x] != WALL && stage[clyde.y + 1][clyde.x] != WALL2) pathC[1] = 1; //Down
        if(stage[clyde.y][clyde.x - 1] != WALL && stage[clyde.y][clyde.x - 1] != WALL2) pathC[2] = 1; //Left
        if(stage[clyde.y][clyde.x + 1] != WALL && stage[clyde.y][clyde.x + 1] != WALL2) pathC[3] = 1; //Right

        double best_disC = 10000.0;
        int dirC = clyde.dir;
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

            if(pathC[i] == 1){
                double dis = pow(clyde.target_x - test_x,2) + pow(clyde.target_y - test_y, 2);
                if(dis<best_disC){
                    dirC = i; //Store the best direction
                    best_disC = dis;
                }
                else{ //If path not valid, move onto next
                    continue;
                }
            }
        }
        clyde.dir = dirC; //Set the direction
        //If 8 or more positions away from pacman, chase, else scatter
        if(best_disC >= 64.0){ //64 due to distance being squared
            clyde.mode = 0;
        }
        else{
            clyde.mode = 1;
        }

        switch(clyde.dir){ //Move along the direction
            case 0:
            clyde.y--;
            break;
            case 1:
            clyde.y++;
            break;
            case 2:
            clyde.x--;
            break;
            case 3:
            clyde.x++;
            break;
        }

        //Usage of the teleporter
    if(clyde.y == 14) {
		if(clyde.x - 1 < 0) { 
			clyde.x = WIDTH - 1;
			clyde.dir = 2;
        } else if (clyde.x + 1 >= WIDTH) { 
            clyde.x = 0;
            clyde.dir = 3;
        }
    }
//End of ghost section//
    //Detect if pacman and ghosts crossed paths or occupy the same tiles
    int eat[4] = {0, 0, 0, 0}; //Blinky, pinky, inky, clyde
    bool sameTile = false;
    bool crossPaths = false;
    //Blinky
    sameTile = (blinky.x == pacman_x && blinky.y == pacman_y);
    crossPaths = blinky.x == pacman_old_x && blinky.y == pacman_old_y && blinky.prev_x == pacman_x && blinky.prev_y == pacman_y;
    eat[0] = sameTile || crossPaths;

    //Pinky
    sameTile = (pinky.x == pacman_x && pinky.y == pacman_y);
    crossPaths = pinky.x == pacman_old_x && pinky.y == pacman_old_y && pinky.prev_x == pacman_x && pinky.prev_y == pacman_y;
    eat[1] = sameTile || crossPaths;

    //Inky
    sameTile = (inky.x == pacman_x && inky.y == pacman_y);
    crossPaths = inky.x == pacman_old_x && inky.y == pacman_old_y && inky.prev_x == pacman_x && inky.prev_y == pacman_y;
    eat[2] = sameTile || crossPaths;

    //Clyde
    sameTile = (clyde.x == pacman_x && clyde.y == pacman_y);
    crossPaths = clyde.x == pacman_old_x && clyde.y == pacman_old_y && clyde.prev_x == pacman_x && clyde.prev_y == pacman_y;
    eat[3] = sameTile || crossPaths;

        if(frightened){
            blinky.mode = 2;
            pinky.mode = 2;
            inky.mode = 2;
            clyde.mode = 2;
        }
        if(frightened_timer > 0){
            //If a ghost comes into contact with pacman, become "eye balls" and return to ghost house
            if(eat[0]){
                blinky.mode = 3;
                frightened = false;
            }
            if(eat[1]){
                pinky.mode = 3;
                frightened = false;
            }
            if(eat[2]){
                inky.mode = 3;
                frightened = false;
            }
            if(eat[3]){
                clyde.mode = 3;
                frightened = false;
            }
            frightened_timer--;
        }
         else if(eat[0] || eat[1] || eat[2] || eat[3]){
             lives--;
             if(lives <= 0){
                 running = false;
                 reset(1);
             } else{
                reset(0);
                flushinp();
             }
        }
    //Check if power pellet effects are still going on -Aiden.
    if(frightened_timer == 0) frightened = false;
       
    //Drawing character on screen logic. Jhan and Aiden.
        if(blinky.mode == 2){ // frightened
            if(frightened_timer % 2 == 0){
                mvwaddch(game_win, blinky.y, blinky.x, 'G' | COLOR_PAIR(7));
            }
            else{
                mvwaddch(game_win, blinky.y, blinky.x, 'G' | COLOR_PAIR(2));
            }
        }
        else if(blinky.mode == 3){ // eaten (eyes)
            mvwaddch(game_win, blinky.y, blinky.x, ':' | COLOR_PAIR(7));
        }
        else{ // normal
            mvwaddch(game_win, blinky.y, blinky.x, 'G' | COLOR_PAIR(4));
        }

        if(pinky.mode == 2){ // frightened
            if(frightened_timer % 2 == 0){
                mvwaddch(game_win, pinky.y, pinky.x, 'G' | COLOR_PAIR(7));
            }
            else{
                mvwaddch(game_win, pinky.y, pinky.x, 'G' | COLOR_PAIR(2));
            }
        }
        else if(pinky.mode == 3){ // eaten (eyes)
            mvwaddch(game_win, pinky.y, pinky.x, ':' | COLOR_PAIR(7));
        }
        else{ // normal
            mvwaddch(game_win, pinky.y, pinky.x, 'G' | COLOR_PAIR(6));
        }

        if(inky.mode == 2){ // frightened
            if(frightened_timer % 2 == 0){
                mvwaddch(game_win, inky.y, inky.x, 'G' | COLOR_PAIR(7));
            }
            else{
                mvwaddch(game_win, inky.y, inky.x, 'G' | COLOR_PAIR(2));
            }
        }
        else if(inky.mode == 3){ // eaten (eyes)
            mvwaddch(game_win, inky.y, inky.x, ':' | COLOR_PAIR(7));
        }
        else{ // normal
            mvwaddch(game_win, inky.y, inky.x, 'G' | COLOR_PAIR(3));
        }
        
        if(clyde.mode == 2){ // frightened
            if(frightened_timer % 2 == 0){
                mvwaddch(game_win, clyde.y, clyde.x, 'G' | COLOR_PAIR(7));
            }
            else{
                mvwaddch(game_win, clyde.y, clyde.x, 'G' | COLOR_PAIR(2));
            }
        }
        else if(clyde.mode == 3){ // eaten (eyes)
            mvwaddch(game_win, clyde.y, clyde.x, ':' | COLOR_PAIR(7));
        }
        else{ // normal
            mvwaddch(game_win, clyde.y, clyde.x, 'G' | COLOR_PAIR(1));
        }
        
    

    mvwaddch(game_win, pacman_y, pacman_x, 'P' | COLOR_PAIR(1)); //https://docs.oracle.com/cd/E86824_01/html/E54767/mvwaddch-3curses.html
    wrefresh(game_win); //Game window refreshed. -Jhan
    napms(100); 
   }

   endwin();
   return 0;
}

//Note, to compile gcc -o pacman pacman.c -lncurses -lm
