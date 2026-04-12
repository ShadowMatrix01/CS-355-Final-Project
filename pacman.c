#include <stdio.h>
#include <ncurses.h> //Ncurses library ported in
#include <stdbool.h>
#define WIDTH 60 //Because it looked incorrect
#define HEIGHT 30 

// Width and height from: https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/keys.html
/*Pac-Man
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
lost if Pac-Man is eaten by a ghost*/

int pacman_x = 1;
int pacman_y = 1;
int main(int argc, char * argv[]) {
   WINDOW *game_win;
   int pacman_move;
   bool running = true;
   initscr();
   clear();
   noecho();
   cbreak();
   keypad(stdscr, TRUE);
   game_win = newwin(HEIGHT, WIDTH, 0, 0); //Window initialized with
   //size 60 x 30. See comment for reason.
   box(game_win, 0, 0); //Border around the screen.
  
   curs_set(0); //Cursor hidden from terminal, because that breaks game flow.
   refresh(); //Stdscr refreshed
   wrefresh(game_win); //Game window refreshed.
   while (running) {
    pacman_move = getch();
    move(30,0);
    clrtoeol(); //This is how to avoid the weird line artifacting appparently.
    switch(pacman_move) {
        case KEY_UP: 
             if (pacman_y > 1) { //Needed to avoid an out of bounds bug.
                pacman_y--; //Pacman moves up by decrementing y axis by one.
             }
             mvprintw(HEIGHT, 0, "Up has been pressed");
             break;
        case KEY_DOWN:
             if (pacman_y < HEIGHT - 2) { //Needed to avoid an out of bounds bug.
                pacman_y++; //Pacman moves down by incrementing y axis by one.
             }
             mvprintw(HEIGHT, 0, "Down has been pressed");
             break;
        case KEY_LEFT:
             if (pacman_x > 1) {
                pacman_x--;
             }
             mvprintw(HEIGHT,0, "Left pressed");
             break;
        case KEY_RIGHT:
             if (pacman_x < WIDTH - 2) {
                pacman_x++;
             }
             mvprintw(HEIGHT,0, "Right pressed");
             break;
        case 27: 
             running = false;
             break;
        default:
            break;
    }
    mvwaddch(game_win, pacman_y, pacman_x, 'P'); //https://docs.oracle.com/cd/E86824_01/html/E54767/mvwaddch-3curses.html
    refresh(); //Stdscr refreshed
    wrefresh(game_win); //Game window refreshed.
   }
   endwin();
   return 0;
}

//Note, to compile gcc -o pacman pacman.c -lncurses