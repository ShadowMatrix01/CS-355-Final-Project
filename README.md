# Systems Programming Project (CCSU)

## Project Overview

This project is designed to be completed over a period of **2--3 weeks**
by a team of **two students**.

-   Each student must have very clear roles and responsibilities.
-   At the beginning of each function/code block, include:
    -   Primary author
    -   Description of functionality
-   Each group must use **GitHub**:
    -   One owner
    -   One collaborator
-   A reasonable commit history is expected.
-   All games must support **keyboard controls**.

------------------------------------------------------------------------

## Technical Requirements

-   Use **curses or ncurses** for visuals.
-   Must run in a **terminal environment** (e.g., Codespaces).
-   You are encouraged to:
    -   Improve UI
    -   Add features
    -   Increase gameplay complexity

------------------------------------------------------------------------

## Game Options

You must select **one** of the following:

------------------------------------------------------------------------

## 🟡 Pac-Man

### Objective

Navigate a maze and eat all Pac-Dots.

### Game Design

-   Maze size: **30 × 30**
-   Start position: top-left corner
-   4 ghosts:
    -   Random spawn
    -   Random movement
    -   Can kill Pac-Man
-   4 power pellets:
    -   Random placement
    -   Allow ghost-eating for **10 seconds**
-   Total Pac-Dots: **150**

### Game Play

-   **Win:** All Pac-Dots eaten
-   **Lose:** Pac-Man is eaten

------------------------------------------------------------------------

## 🍄 Mario

### Objective

Reach the castle without hitting obstacles.

### Game Design

-   Constant movement speed
-   Jump ability
-   Star power-up:
    -   Random appearance
    -   Allows passing through obstacles
-   Castle appears after some time

### Game Play

-   **Win:** Reach castle
-   **Lose:** Hit obstacle

------------------------------------------------------------------------

## 🧱 Tetris

### Objective

Score points by completing horizontal lines.

### Game Design

-   Grid: **≥ 10 wide × 25 high**
-   Blocks:
    -   Line (4)
    -   Box (2×2)
    -   L-shape
    -   Z-shape
-   Blocks fall at constant speed
-   Player can:
    -   Rotate blocks
    -   Move horizontally
-   Completed lines:
    -   Disappear
    -   Give 1 point
    -   Cause blocks above to fall

### Game Play

-   **Win:** 20 points
-   **Lose:** Stack reaches height limit (no space)

------------------------------------------------------------------------

## 🐍 Snake

### Objective

Consume food and grow without crashing.

### Game Design

-   Grid: **≥ 20 × 20**
-   Visible border required
-   Initial length: **3**
-   Fixed speed
-   Food:
    -   Random spawn
    -   Increases length by 1

### Game Play

-   **Win:** Length reaches half border perimeter
-   **Lose:** Hit wall or self

------------------------------------------------------------------------

## Grading (15 Points)

  Category and Points
  -------------------------- --------
  Variable/Function Naming:   3
  Code Comments:              2
  GitHub Commit History:      4
  Working Demonstration:      2
  Overall Gameplay:           4

------------------------------------------------------------------------

## Bonus Points

-   🥇 First Place: +4 points
-   🥈 Second Place: +2 points

------------------------------------------------------------------------

## Notes

-   Creativity is encouraged
-   Focus on gameplay and user experience
-   Plan ahead to meet deadlines

------------------------------------------------------------------------

## Submission

Ensure: - GitHub repo is complete - Code runs correctly - Documentation
is included

------------------------------------------------------------------------

Good luck and have fun!
