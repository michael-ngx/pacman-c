/* This files provides address values that exist in the system */
#include <math.h>

#define SDRAM_BASE 0xC0000000
#define FPGA_ONCHIP_BASE 0xC8000000
#define FPGA_CHAR_BASE 0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define TIMER_BASE 0xFF202000
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define CHAR_BUF_CTRL_BASE 0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x211b
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00
#define BLACK 0

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Grid size. */
#define ROW 19
#define COL 22

/* Constants for animation */
#define BOX_LEN 2
#define NUM_BOXES 8
#define SPEED 1
#define PLAYER_RADIUS 5
#define MOUTH_SPEED 5 // CHANGE THIS BASED ON CLOCK SPEED
#define SPEED_G 1                                         
#define GHOST_RADIUS 5          

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define UP 3
#define DOWN 1
#define RIGHT 0
#define LEFT 2
int dir = RIGHT;

/*********************************************************************************************************************
* GLOBAL VARIABLES
**********************************************************************************************************************/
volatile int pixel_buffer_start; 
int graph[ROW][COL] =  { // y, x
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    {0, 3, 2, 2, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 3, 0}, 
    {0, 2, 0, 2, 2, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 2, 0}, 
    {0, 2, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 2, 0}, 
    {0, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0}, 
    {0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0}, 
    {0, 2, 0, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 0, 0, 2, 0}, 
    {0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 2, 0, 2, 0, 2, 0, 0, 2, 0}, 
    {0, 2, 2, 2, 0, 2, 2, 2, 0, 2, 0, 3, 0, 2, 2, 2, 0, 2, 2, 2, 2, 0}, 
    {0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 2, 2, 2, 0, 0, 0, 2, 0, 0, 0, 0}, 
    {0, 2, 2, 2, 0, 2, 2, 2, 0, 2, 0, 2, 0, 2, 2, 2, 0, 2, 2, 2, 2, 0}, 
    {0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 0, 0, 2, 0, 2, 0, 2, 0, 0, 2, 0}, 
    {0, 2, 0, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 0, 0, 2, 0}, 
    {0, 2, 0, 0, 0, 2, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0}, 
    {0, 2, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0}, 
    {0, 2, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 2, 0}, 
    {0, 2, 0, 2, 2, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 2, 0}, 
    {0, 2, 2, 2, 0, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 3, 0}, 
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  
};

int currRot = 0;
int open [9][9] = {
		{0, 0, 1, 1, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 0},
		{1, 1, 1, 0, 1, 1, 1, 0, 0},
		{1, 1, 1, 1, 1, 0, 0, 0, 0},
		{1, 1, 1, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 0},
		{0, 0, 1, 1, 1, 1, 1, 0, 0},
};
int closed [9][9] = {
        {0, 0, 1, 1, 1, 1, 1, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 1, 1, 1, 1, 1, 0, 0},
};

int ghost [9][9] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 3, 3, 3, 3, 3, 0, 0},
        {0, 3, 3, 5, 3, 3, 5, 3, 0},
        {0, 3, 5, 5, 1, 5, 5, 1, 0},
        {0, 3, 4, 5, 2, 4, 5, 2, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 3, 3, 3, 3, 0},
        {0, 3, 3, 3, 0, 3, 3, 3, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
};

int cherry [9][9] = {
        {0, 0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 1, 1, 1},
        {0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 2, 2, 0, 0, 0, 1, 0, 0},
        {2, 2, 2, 2, 0, 0, 1, 0, 0},
        {2, 2, 2, 2, 0, 2, 1, 0, 0},
        {0, 2, 2, 0, 2, 2, 2, 2, 0},
        {0, 0, 0, 0, 2, 3, 2, 2, 0},
        {0, 0, 0, 0, 0, 2, 2, 0, 0},
};

typedef struct Point
{
    int x, y;
} Point;

typedef struct Grid
{
    int row, col;
} Grid;

int originX = 39;
int originY = 24;
int worldMapRatio = 11;

int scoreValue = 0;
bool power_mode = false;

/*********************************************************************************************************************
* FUNCTION DECLARATIONS
**********************************************************************************************************************/
// Game Logic
Grid getGrid(int x, int y);
int setDir(int x, int y);
bool canMove(Point head);
bool canTurn(int x, int y, int dir);

// BFS algorithm
bool grid_equal(const Grid p1, const Grid p2);
bool isValid(int row, int col);
Grid* BFS (Grid start, Grid end);
Point* getPixelPath (Grid* path_grid, int ghost_number);
int count_pixel_path[] = {0, 0, 0};

// Drawing
void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();
void draw_box(int x, int y, short int col);
void drawMap();
void drawPac(int x, int y, int clear, int c);
void drawGhost (int x, int y, bool clear, int ghost_number);
void drawCollectibles();
void draw_powerups(int x, int y, bool clear);
void rotatePac();
void video_text(int x, int y, char * text_ptr);
void changeScore(int score);

/*********************************************************************************************************************
* MAIN PROGRAM
**********************************************************************************************************************/
int main(void)
{
    volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;

    /*************************************
    * PLAYER DATA
    **************************************/
    // Score
	char clear[10] = "       \0";
    char scoreLabel[8] = "SCORE: \0";
	char score[10] = "0\0";
    // Current player location, in pixels
    int x = 149;
    int y = 183;
    // Previous player location, in pixels
    int pX = x;
    int pY = y;
    // Pixel for head of next movement, changes based on direction
    Point head;
    // For player close/open mouth
    int c = 0;
    /**************************************
    * GHOST DATA
    ***************************************/
    bool not_first_ghost[] = {false, false, false};
    bool exist[] = {true, true, true};
    int x_ghost[] = {99, 55, 264};
    int y_ghost[] = {50, 182, 39};
    
    Grid* path_grid[3];
    Point* path_pixel[3];

    /**************************************
    * GAME DATA
    ***************************************/
    // Boolean to check for first drawing
    bool not_first = false; 
    // Timer for power mode (Changed based on clock speed)
    int power_timer = 0;

    /* Before iteration */
    clear_screen();
    drawMap();
	video_text(36, 10, clear);
	video_text(43, 10, clear);
    video_text(36, 10, scoreLabel);
	video_text(43, 10, score);
    drawCollectibles();
    wait_for_vsync();

    /*******************************************************************************
    * GAME LOOP
    ********************************************************************************/
    while (1)
    {
        /**************************************
        * PLAYER
        ***************************************/
		int newDir = setDir(x, y);
        if (((*(int *)KEY_BASE) != 0) && (newDir != -1)) dir = newDir;

        Grid grid = getGrid(x, y);
        if (graph[grid.row][grid.col] == 2) {
            graph[grid.row][grid.col] = 1;
            changeScore(10);
        } else if (graph[grid.row][grid.col] == 3)
        {
            graph[grid.row][grid.col] = 1;
            // Clear powerups from the map
            int x = (grid.col * worldMapRatio) + originX + 6;
            int y = (grid.row * worldMapRatio) + originY + 6;
            draw_powerups(x, y, true);
            // Change mode
            changeScore(10);
            power_mode = true;
            power_timer = 200;
            // Change behavior of ghost
            for (int i = 0; i < 3; i++)
            {
                count_pixel_path[i] = 0;
                drawGhost(x_ghost[i], y_ghost[i], true, i);
            }
        }
        // Countdown
        if (power_mode)
        {
            power_timer--;
            if (power_timer <= 0)
            {
                power_timer = 0;
                power_mode = false;
            }
        }
        
        // Number of pixels to be incremented if moved in chosen direction
        int headPos = PLAYER_RADIUS + SPEED;
        switch (dir)
        {
        case RIGHT:
            head.x = x + (headPos);
            head.y = y;
            if (canMove(head))
            {
                x += SPEED;
                c++;
            } else {c=0;}
            break;
        case LEFT:
            head.x = x - (headPos);
            head.y = y;
            if (canMove(head))
            {
                x -= SPEED;
                c++;
            } else {c=0;}
            break;
        case UP:
            head.x = x;
            head.y = y - (headPos);
            if (canMove(head))
            {
                y -= SPEED;
                c++;
            } else {c=0;}
            break;
        case DOWN:
            head.x = x;
            head.y = y + (headPos);
            if (canMove(head))
            {
                y += SPEED;
                c++;
            } else {c=0;}
            break;
        //////////////////////////////////////////////////////////////////////
        default:
            if (canMove(head))
            {
                x += SPEED;
                head.x = x + (headPos);
                head.y = y;
            }
        }

        if (not_first)
        {
            drawPac(pX, pY, TRUE, c);
        }
        drawPac(x, y, FALSE, c);
        pX = x;
        pY = y;
        not_first = true;

        /**************************************
        * GHOSTS
        ***************************************/
        bool reset_path[] = {false, false, false};
        for (int i = 0; i < 3; i++)
        {
            // Skips if ghost has been eaten
            if (!exist[i])
            {
                continue;
            }
            // Init first path for ghost/Ghost finished tracking previous path
            if (count_pixel_path[i] <= 0)
            {
                // Block not executed first time (no path has been created yet)
                if (not_first_ghost[i])
                {
                    free(path_grid[i]);
                    free(path_pixel[i]);
                }
                not_first_ghost[i] = true;
                reset_path[i] = true;
                Grid start = getGrid(x_ghost[i], y_ghost[i]);
                Grid dest;
                if (!power_mode)
                {
                    dest = getGrid(pX, pY);
                } else {
                    if (i == 0) dest = (Grid) {2, 5};
                    else if (i == 1) dest = (Grid) {9, 11};
                    else dest = (Grid) {14, 3};
                }
                path_grid[i] = BFS(start, dest);
                path_pixel[i] = getPixelPath(path_grid[i], i);
            }

            // Only clear previous location when not reseting
            if (!reset_path[i])
            {
                if (!power_mode)
                {
                    drawGhost(path_pixel[i][count_pixel_path[i] + 1].x, path_pixel[i][count_pixel_path[i] + 1].y, true, i);
                } else
                {
                    if (power_timer % 2 == 0)
                    {
                        drawGhost(path_pixel[i][count_pixel_path[i]].x, path_pixel[i][count_pixel_path[i]].y, true, i);
                    } else
                    {
                        drawGhost(path_pixel[i][count_pixel_path[i] + 1].x, path_pixel[i][count_pixel_path[i] + 1].y, true, i);
                    }
                }
            }
            // Let ghost travel on path_pixel
            drawGhost(path_pixel[i][count_pixel_path[i]].x, path_pixel[i][count_pixel_path[i]].y, false, i);
            // In power mode, ghost runs away with half the speed
            if (power_timer % 2 == 0)
            {
                count_pixel_path[i]--;
            }
            // When ghost reaches the destination
            if (count_pixel_path[i] < 0)
            {
                count_pixel_path[i] = 0;
            }
            x_ghost[i] = path_pixel[i][count_pixel_path[i]].x;
            y_ghost[i] = path_pixel[i][count_pixel_path[i]].y;
            // Pacman contact with ghost
            if (abs(x - x_ghost[i]) <= PLAYER_RADIUS && abs(y - y_ghost[i]) <= PLAYER_RADIUS)
            {
                if (!power_mode)
                {
                    printf("YOU LOSE");
                    return 0;
                } else
                {
                    exist[i] = false;
                    drawGhost(x_ghost[i], y_ghost[i], true, i);
                    changeScore(100);
                }
            }
        }
        
        /**************************************
        * ALL OBJECTS
        ***************************************/
        drawCollectibles();
        wait_for_vsync();
        
        /**************************************
        * WINNING/LOSING CONDITION
        ***************************************/
        // Ate everything
        if (scoreValue == (187*10 + 300))
        {
            printf("YOU WIN");
            return 0;
        }
        // Ate all power-ups but cannot eat all ghosts
        if (graph[1][1] == 1 && graph[1][20] == 1 && graph[8][11] == 1 && graph[17][20] == 1
            && power_mode == false && (exist[0] || exist[1] || exist[2]))
        {
            printf("NO MORE POWERUPS\nYOU LOSE");
            return 0;
        }
    }
}

/*********************************************************************************************************************
* HELPER FUNCTIONS
**********************************************************************************************************************/

void swap(int *x, int *y){
    int temp = *x;
    *x = *y;
    *y= temp;
}

void rotatePac() {
   for(int i=0; i<9; i++){
     for(int j=i+1; j<9; j++)
        swap(&open[i][j], &open[j][i]);
    }

   for(int i=0; i<9; i++){
     for(int j=0; j<9/2; j++){
        swap(&open[i][j], &open[i][9-j-1]);
     }
   }

   for(int i=0; i<9; i++){
     for(int j=i+1; j<9; j++)
        swap(&closed[i][j], &closed[j][i]);
    }

   for(int i=0; i<9; i++){
     for(int j=0; j<9/2; j++){
        swap(&closed[i][j], &closed[i][9-j-1]);
     }
   }
   currRot++;
   if (currRot == 4) {currRot = 0;}
}

/**************************************
* GAME LOGIC
***************************************/
int setDir(int x, int y)
{
    switch (*(int *)KEY_BASE)
        {
        case 1:
            if (canTurn(x, y, RIGHT)) return RIGHT;
			return -1;
            break;
        case 2:
            if (canTurn(x, y, LEFT)) return LEFT;
			return -1;
            break;
        case 4:
            if (canTurn(x, y, DOWN)) return DOWN;
			return -1;
            break;
        case 8:
            if (canTurn(x, y, UP)) return UP;
			return -1;
            break;
        default:
			return -1;;
        }
}

void changeScore (int score) {
    char clear[10] = "0\0";
    video_text(43, 10, clear);
    scoreValue += score;
    char s[10];
    sprintf(s, "%d", scoreValue);
    video_text(43, 10, s);
}

Grid getGrid(int x, int y)
{
    Grid g;
    g.col = floor((x - originX) / worldMapRatio);
    g.row = floor((y - originY) / worldMapRatio);
    return g;
}

bool canMove(Point head)
{
    return (*(short int *)(pixel_buffer_start + (head.y << 10) + (head.x << 1)) != BLUE);
}

bool canTurn(int x, int y, int dir)
{
    Grid grid = getGrid(x, y);
    int xRail = 39 + 5 + (grid.col * 11); // determining the proper turning coordinates
    int yRail = 24 + 5 + (grid.row * 11);

    switch (dir)
    {
    case RIGHT:
        return (graph[grid.row][grid.col + 1] > 0) && (y == yRail);
        break;
    case LEFT:
        return (graph[grid.row][grid.col - 1] > 0) && (y == yRail);
        break;
    case DOWN:
        return (graph[grid.row + 1][grid.col] > 0) && (x == xRail);
        break;
    case UP:
        return (graph[grid.row - 1][grid.col] > 0) && (x == xRail);
        break;
    default:
        return false;
    }
}

/**************************************
* BFS ALGORITHM
***************************************/
bool grid_equal(const Grid p1, const Grid p2)
{
    return (p1.row == p2.row) && (p1.col == p2.col);
}

// function to check if given row and column are within the bounds of the grid
bool isValid(int row, int col)
{
    return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL) && (graph[row][col] > 0);
}

// Return a vector of grids to be traversed from start to end
Grid* BFS (Grid start, Grid dest)
{
    // Path to be returned
    Grid* path = malloc(ROW * COL * sizeof(Grid));
    // Queue for BFS (FIFO)
    Grid queue [ROW * COL];
    int front = 0;
    int rear = 0;
    queue[rear++] = start;
    // Mark visited grids
    bool visited [ROW][COL];
    // Parent grid array of each grid
    Grid parent [ROW][COL];

    // Initialize visited, parent, and path (result) arrays
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            visited[i][j] = false;
            parent[i][j] = (Grid) {-1, -1};
        }
    }
    for (int k = 0; k < ROW * COL; k++)
    {
        path[k] = (Grid) {-1, -1};
    }

    // Mark visited first grid
    visited[start.row][start.col] = true;
    parent[start.row][start.col] = start;

    // BFS Loop
    while (front < rear)
    {
        Grid curr = queue[front++];
        int row = curr.row;
        int col = curr.col;

        // Found path
        if (grid_equal(curr, dest))
        {
            // Trace path
            int top = -1;
            int row_track = dest.row;
            int col_track = dest.col;
            // Add the destination cell to the path
            path[++top] = dest;
            // Backtrack from the destination to the source using the parent array
            while (!(parent[row_track][col_track].row == row_track && parent[row_track][col_track].col == col_track)) {
                path[++top] = parent[row_track][col_track];
                int row_temp = row_track;
                row_track = parent[row_track][col_track].row;
                col_track = parent[row_temp][col_track].col;
            }
            // Add the source cell to the path
            path[++top] = start;
            return path;
        }

        // Visit all neighbor grids if neighbor is within grid and not visited
        // Up neighbor
        if (isValid(row - 1, col) && !visited[row - 1][col])
        {
            visited[row - 1][col] = true;
            parent[row - 1][col] = curr;
            queue[rear++] = (Grid) {row - 1, col};
        }

        // Down neighbor
        if (isValid(row + 1, col) && !visited[row + 1][col])
        {
            visited[row + 1][col] = true;
            parent[row + 1][col] = curr;
            queue[rear++] = (Grid) {row + 1, col};
        }

        // Left neighbor
        if (isValid(row, col - 1) && !visited[row][col - 1])
        {
            visited[row][col - 1] = true;
            parent[row][col - 1] = curr;
            queue[rear++] = (Grid) {row, col - 1};
        }

        // Right neighbor
        if (isValid(row, col + 1) && !visited[row][col + 1])
        {
            visited[row][col + 1] = true;
            parent[row][col + 1] = curr;
            queue[rear++] = (Grid) {row, col + 1};
        }
    }

    // ??? case - no path is found
    return path;
}

Point* getPixelPath (Grid* path_grid, int ghost_number)
{
    // Path to be returned
    Point* path_pixel = malloc(ROW * COL * worldMapRatio * sizeof(Point));
    // Initialize return path
    for (int i = 0; i < ROW * COL * worldMapRatio; i++)
    {
        path_pixel[i] = (Point) {-1, -1};
    }
    count_pixel_path[ghost_number] = 0;
    for (int grid = 0; grid < ROW * COL - 1; grid++)
    {
        if (path_grid[grid + 1].row == -1 && path_grid[grid + 1].col == -1) 
        {
            int x = (path_grid[grid].col * worldMapRatio) + originX + 6;
            int y = (path_grid[grid].row * worldMapRatio) + originY + 6;
            path_pixel[count_pixel_path[ghost_number]] = (Point) {x, y};
            break;
        }
        // xy of grid mid point
        int curr_x = (path_grid[grid].col * worldMapRatio) + originX + 6;
        int curr_y = (path_grid[grid].row * worldMapRatio) + originY + 6;
        int next_x = (path_grid[grid + 1].col * worldMapRatio) + originX + 6;
        int next_y = (path_grid[grid + 1].row * worldMapRatio) + originY + 6;
        
        if (next_x > curr_x)
        {
            for (int i = 0; i < 11; i++)
            {
                int x = (path_grid[grid].col * worldMapRatio) + originX + 6 + i;
                int y = (path_grid[grid].row * worldMapRatio) + originY + 6;
                path_pixel[count_pixel_path[ghost_number]++] = (Point) {x, y};
            }
        } else if (next_x < curr_x)
        {
            for (int i = 0; i < 11; i++)
            {
                int x = (path_grid[grid].col * worldMapRatio) + originX + 6 - i;
                int y = (path_grid[grid].row * worldMapRatio) + originY + 6;
                path_pixel[count_pixel_path[ghost_number]++] = (Point) {x, y};
            }
        } else if (next_y < curr_y)
        {
            for (int i = 0; i < 11; i++)
            {
                int x = (path_grid[grid].col * worldMapRatio) + originX + 6;
                int y = (path_grid[grid].row * worldMapRatio) + originY + 6 - i;
                path_pixel[count_pixel_path[ghost_number]++] = (Point) {x, y};
            }
        } else if (next_y > curr_y)
        {
            for (int i = 0; i < 11; i++)
            {
                int x = (path_grid[grid].col * worldMapRatio) + originX + 6;
                int y = (path_grid[grid].row * worldMapRatio) + originY + 6 + i;
                path_pixel[count_pixel_path[ghost_number]++] = (Point) {x, y};
            }
        }
    }
    return path_pixel;
}

/**************************************
* DRAWING
***************************************/
void video_text(int x, int y, char * text_ptr)
{
    int offset;
    volatile char * character_buffer = (char *)FPGA_CHAR_BASE; // video character buffer
    offset = (y << 7) + x;
    while (*(text_ptr)) {
        *(character_buffer + offset) =
        *(text_ptr); // write to the character buffer
        ++text_ptr;
        ++offset;
    }
}

void drawCollectibles()
{
    for (int i = 0; i < COL; i++) {
        for (int j = 0; j < ROW; j++) {
            if (graph[j][i] == 2) {
                int x = (i * worldMapRatio) + originX + 6;
                int y = (j * worldMapRatio) + originY + 6;
                draw_box(x, y, ORANGE);
            } else if (graph[j][i] == 3)
            {
                int x = (i * worldMapRatio) + originX + 6;
                int y = (j * worldMapRatio) + originY + 6;
                draw_powerups(x, y, false);
            }
        }
    }
}

void draw_box(int x, int y, short int col)
{
    for (int i = x - 1; i < x + 1; i++)
    {
        for (int j = y - 1; j < y + 1; j++)
        {
            plot_pixel(i, j, col);
        }
    }
}

void draw_powerups(int x, int y, bool clear)
{
    short int c1;
    for (int i=0; i<9; i++) {
        for (int j=0; j<9; j++) {
            if (!clear)
            {
                if (cherry[i][j] != 0){
                    switch (cherry[i][j]) {
                        case 1: c1 = ORANGE; break;
                        case 2: c1 = RED; break;
                        case 3: c1 = WHITE; break;
                        default: c1 = BLACK;
                    }
                    plot_pixel(j+x-4, i+y-4, c1);
                }
            } else {
                plot_pixel(j+x-4, i+y-4, BLACK);
            }
        }
    }
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void clear_screen()
{
    for (int x = 0; x < RESOLUTION_X; x++)
    {
        for (int y = 0; y < RESOLUTION_Y; y++)
        {
            plot_pixel(x, y, 0);
        }
    }
}

void wait_for_vsync()
{
    volatile int *pixel_ctrl_ptr = (int *)0xFF203020;
    register int status;

    *pixel_ctrl_ptr = 1;

    status = *(pixel_ctrl_ptr + 3);
    while ((status & 0x01) != 0)
    {
        status = *(pixel_ctrl_ptr + 3);
    }
}

void drawPacOpen(int x, int y, short int c1) {
    for (int i=0; i<9; i++) {
        for (int j=0; j<9; j++) {
            if (open[i][j] != 0) {
                plot_pixel(j+x-4, i+y-4, c1);
            } 
        }
    }
}

void drawPacClosed(int x, int y, short int c1)
{
	for (int i=0; i<9; i++) {
        for (int j=0; j<9; j++) {
            if (closed[i][j] != 0) {
                plot_pixel(j+x-4, i+y-4, c1);
            } 
        }
    }
}

void drawPac(int x, int y, int clear, int c)
{
    while (currRot != dir) {
        rotatePac();
    }
    short int c1;
    if (clear == 1) {c1 = BLACK; }
    else {c1 = 0xff20; }
    bool face = ((c % MOUTH_SPEED) < 2);

    if (face || clear == 1) {drawPacClosed(x, y, c1); }
    else {drawPacOpen(x, y, c1); }
}

void drawGhost (int x, int y, bool clear, int ghost_number)
{
    short int c1;
    for (int i=0; i<9; i++) {
        for (int j=0; j<9; j++) {
            if (!clear)
            {
                if (ghost[i][j] != 0){
                    switch (ghost[i][j]) {
                        case 1: c1 = BLACK; break;
                        case 2: c1 = MAGENTA; break;
                        case 3: if (power_mode) c1 = WHITE;
                                else {
                                    if (ghost_number == 0) c1 = RED; 
                                    else if (ghost_number == 1) c1 = CYAN; 
                                    else c1 = YELLOW; 
                                }
                                break;
                        case 4: c1 = PINK; break;
                        case 5: c1 = WHITE; break;
                        default: c1 = BLACK;
                    }
                    plot_pixel(j+x-4, i+y-4, c1);
                }
            } else
            {
                plot_pixel(j+x-4, i+y-4, BLACK);
            }
        }
    }
}

void drawMap()
{
    int walls[10891] = {25678, 26702, 27726, 28750, 29774, 30798, 31822, 32846, 33870, 34894, 35918, 36942, 37966, 38990, 40014, 41038, 42062, 43086, 44110, 45134, 46158, 47182, 48206, 49230, 50254, 51278, 52302, 53326, 54350, 55374, 56398, 57422, 58446, 59470, 60494, 61518, 62542, 63566, 64590, 65614, 66638, 67662, 68686, 69710, 70734, 71758, 72782, 73806, 74830, 75854, 76878, 77902, 78926, 79950, 80974, 81998, 83022, 84046, 85070, 86094, 87118, 88142, 89166, 90190, 91214, 92238, 93262, 94286, 95310, 96334, 97358, 98382, 99406, 100430, 101454, 102478, 103502, 104526, 105550, 106574, 107598, 108622, 109646, 110670, 111694, 112718, 113742, 114766, 115790, 116814, 117838, 118862, 119886, 120910, 121934, 122958, 123982, 125006, 126030, 127054, 128078, 129102, 130126, 131150, 132174, 133198, 134222, 135246, 136270, 137294, 138318, 139342, 140366, 141390, 142414, 143438, 144462, 145486, 146510, 147534, 148558, 149582, 150606, 151630, 152654, 153678, 154702, 155726, 156750, 157774, 158798, 159822, 160846, 161870, 162894, 163918, 164942, 165966, 166990, 168014, 169038, 170062, 171086, 172110, 173134, 174158, 175182, 176206, 177230, 178254, 179278, 180302, 181326, 182350, 183374, 184398, 185422, 186446, 187470, 188494, 189518, 190542, 191566, 192590, 193614, 194638, 195662, 196686, 197710, 198734, 199758, 200782, 201806, 202830, 203854, 204878, 205902, 206926, 207950, 208974, 209998, 211022, 212046, 213070, 214094, 215118, 216142, 217166, 218190, 219214, 220238, 221262, 222286, 223310, 224334, 225358, 226382, 227406, 228430, 229454, 230478, 231502, 232526, 233550, 234574, 235598, 236622, 24656, 25680, 26704, 27728, 28752, 29776, 30800, 31824, 32848, 33872, 34896, 35920, 36944, 37968, 38992, 40016, 41040, 42064, 43088, 44112, 45136, 46160, 47184, 48208, 49232, 50256, 51280, 52304, 53328, 54352, 55376, 56400, 57424, 58448, 59472, 60496, 61520, 62544, 63568, 64592, 65616, 66640, 67664, 68688, 69712, 70736, 71760, 72784, 73808, 74832, 75856, 76880, 77904, 78928, 79952, 80976, 82000, 83024, 84048, 85072, 86096, 87120, 88144, 89168, 90192, 91216, 92240, 93264, 94288, 95312, 96336, 97360, 98384, 99408, 100432, 101456, 102480, 103504, 104528, 105552, 106576, 107600, 108624, 109648, 110672, 111696, 112720, 113744, 114768, 115792, 116816, 117840, 118864, 119888, 120912, 121936, 122960, 123984, 125008, 126032, 127056, 128080, 129104, 130128, 131152, 132176, 133200, 134224, 135248, 136272, 137296, 138320, 139344, 140368, 141392, 142416, 143440, 144464, 145488, 146512, 147536, 148560, 149584, 150608, 151632, 152656, 153680, 154704, 155728, 156752, 157776, 158800, 159824, 160848, 161872, 162896, 163920, 164944, 165968, 166992, 168016, 169040, 170064, 171088, 172112, 173136, 174160, 175184, 176208, 177232, 178256, 179280, 180304, 181328, 182352, 183376, 184400, 185424, 186448, 187472, 188496, 189520, 190544, 191568, 192592, 193616, 194640, 195664, 196688, 197712, 198736, 199760, 200784, 201808, 202832, 203856, 204880, 205904, 206928, 207952, 208976, 210000, 211024, 212048, 213072, 214096, 215120, 216144, 217168, 218192, 219216, 220240, 221264, 222288, 223312, 224336, 225360, 226384, 227408, 228432, 229456, 230480, 231504, 232528, 233552, 234576, 235600, 236624, 237648, 24658, 25682, 26706, 235602, 236626, 237650, 24660, 25684, 236628, 237652, 24662, 25686, 236630, 237654, 24664, 25688, 29784, 30808, 31832, 32856, 33880, 34904, 35928, 36952, 37976, 39000, 40024, 41048, 42072, 43096, 44120, 45144, 46168, 47192, 48216, 49240, 50264, 51288, 52312, 53336, 54360, 55384, 56408, 57432, 58456, 59480, 60504, 61528, 62552, 63576, 64600, 65624, 66648, 67672, 68696, 69720, 70744, 71768, 72792, 73816, 74840, 75864, 76888, 77912, 78936, 79960, 80984, 82008, 83032, 84056, 85080, 86104, 87128, 88152, 89176, 90200, 91224, 92248, 93272, 94296, 95320, 96344, 97368, 98392, 99416, 100440, 101464, 102488, 103512, 104536, 105560, 106584, 107608, 108632, 109656, 110680, 111704, 112728, 113752, 114776, 115800, 116824, 117848, 118872, 119896, 120920, 121944, 122968, 123992, 125016, 126040, 127064, 128088, 129112, 130136, 131160, 132184, 133208, 134232, 135256, 136280, 137304, 138328, 139352, 140376, 141400, 142424, 143448, 144472, 145496, 146520, 147544, 148568, 149592, 150616, 151640, 152664, 153688, 154712, 155736, 156760, 157784, 158808, 159832, 160856, 161880, 162904, 163928, 164952, 165976, 167000, 168024, 169048, 170072, 171096, 172120, 173144, 174168, 175192, 176216, 177240, 178264, 179288, 180312, 181336, 182360, 183384, 184408, 185432, 186456, 187480, 188504, 189528, 190552, 191576, 192600, 193624, 194648, 195672, 196696, 197720, 198744, 199768, 200792, 201816, 202840, 203864, 204888, 205912, 206936, 207960, 208984, 210008, 211032, 212056, 213080, 214104, 215128, 216152, 217176, 218200, 219224, 220248, 221272, 222296, 223320, 224344, 225368, 226392, 227416, 228440, 229464, 230488, 231512, 232536, 236632, 237656, 24666, 25690, 29786, 232538, 236634, 237658, 24668, 25692, 29788, 232540, 236636, 237660, 24670, 25694, 29790, 232542, 236638, 237662, 24672, 25696, 29792, 34912, 35936, 36960, 37984, 39008, 40032, 41056, 42080, 43104, 44128, 45152, 46176, 47200, 48224, 49248, 50272, 51296, 52320, 53344, 54368, 55392, 56416, 57440, 58464, 59488, 60512, 61536, 62560, 63584, 64608, 65632, 66656, 67680, 68704, 69728, 70752, 71776, 72800, 73824, 74848, 75872, 76896, 77920, 78944, 79968, 80992, 82016, 83040, 84064, 85088, 86112, 87136, 88160, 89184, 90208, 91232, 92256, 93280, 94304, 95328, 96352, 97376, 98400, 99424, 100448, 101472, 102496, 103520, 104544, 105568, 106592, 107616, 108640, 109664, 110688, 111712, 112736, 113760, 114784, 115808, 116832, 117856, 118880, 119904, 120928, 121952, 122976, 124000, 125024, 126048, 127072, 128096, 129120, 130144, 131168, 132192, 133216, 134240, 135264, 136288, 137312, 138336, 139360, 140384, 141408, 142432, 143456, 144480, 145504, 146528, 147552, 148576, 149600, 150624, 151648, 152672, 153696, 154720, 155744, 156768, 157792, 158816, 159840, 160864, 161888, 162912, 163936, 164960, 165984, 167008, 168032, 169056, 170080, 171104, 172128, 173152, 174176, 175200, 176224, 177248, 178272, 179296, 180320, 181344, 182368, 183392, 184416, 185440, 186464, 187488, 188512, 189536, 190560, 191584, 192608, 193632, 194656, 195680, 196704, 197728, 198752, 199776, 200800, 201824, 202848, 203872, 204896, 205920, 206944, 207968, 208992, 210016, 211040, 212064, 213088, 214112, 215136, 216160, 217184, 218208, 219232, 220256, 221280, 222304, 223328, 224352, 225376, 226400, 227424, 232544, 236640, 237664, 24674, 25698, 29794, 33890, 34914, 35938, 36962, 37986, 39010, 40034, 41058, 42082, 43106, 44130, 45154, 46178, 47202, 48226, 49250, 50274, 51298, 52322, 53346, 54370, 55394, 56418, 57442, 58466, 59490, 60514, 61538, 62562, 63586, 64610, 65634, 66658, 67682, 68706, 69730, 70754, 71778, 72802, 73826, 74850, 75874, 76898, 77922, 78946, 79970, 80994, 82018, 83042, 84066, 85090, 86114, 87138, 88162, 89186, 90210, 91234, 92258, 93282, 94306, 95330, 96354, 97378, 98402, 99426, 100450, 101474, 102498, 103522, 104546, 105570, 106594, 107618, 108642, 109666, 110690, 111714, 112738, 113762, 114786, 115810, 116834, 117858, 118882, 119906, 120930, 121954, 122978, 124002, 125026, 126050, 127074, 128098, 129122, 130146, 131170, 132194, 133218, 134242, 135266, 136290, 137314, 138338, 139362, 140386, 141410, 142434, 143458, 144482, 145506, 146530, 147554, 148578, 149602, 150626, 151650, 152674, 153698, 154722, 155746, 156770, 157794, 158818, 159842, 160866, 161890, 162914, 163938, 164962, 165986, 167010, 168034, 169058, 170082, 171106, 172130, 173154, 174178, 175202, 176226, 177250, 178274, 179298, 180322, 181346, 182370, 183394, 184418, 185442, 186466, 187490, 188514, 189538, 190562, 191586, 192610, 193634, 194658, 195682, 196706, 197730, 198754, 199778, 200802, 201826, 202850, 203874, 204898, 205922, 206946, 207970, 208994, 210018, 211042, 212066, 213090, 214114, 215138, 216162, 217186, 218210, 219234, 220258, 221282, 222306, 223330, 224354, 225378, 226402, 227426, 228450, 232546, 236642, 237666, 24676, 25700, 29796, 33892, 34916, 35940, 226404, 227428, 228452, 232548, 236644, 237668, 24678, 25702, 29798, 33894, 34918, 227430, 228454, 232550, 236646, 237670, 24680, 25704, 29800, 33896, 34920, 227432, 228456, 232552, 236648, 237672, 24682, 25706, 29802, 33898, 34922, 227434, 228458, 232554, 236650, 237674, 24684, 25708, 29804, 33900, 34924, 227436, 228460, 232556, 236652, 237676, 24686, 25710, 29806, 33902, 34926, 227438, 228462, 232558, 236654, 237678, 24688, 25712, 29808, 33904, 34928, 227440, 228464, 232560, 236656, 237680, 24690, 25714, 29810, 33906, 34930, 227442, 228466, 232562, 236658, 237682, 24692, 25716, 29812, 33908, 34932, 227444, 228468, 232564, 236660, 237684, 24694, 25718, 29814, 33910, 34934, 227446, 228470, 232566, 236662, 237686, 24696, 25720, 29816, 33912, 34936, 227448, 228472, 232568, 236664, 237688, 24698, 25722, 29818, 33914, 34938, 50298, 51322, 52346, 53370, 54394, 55418, 56442, 57466, 58490, 59514, 60538, 61562, 62586, 63610, 64634, 65658, 66682, 67706, 68730, 69754, 70778, 71802, 72826, 73850, 74874, 75898, 76922, 77946, 78970, 79994, 81018, 82042, 83066, 84090, 85114, 86138, 87162, 88186, 89210, 90234, 91258, 92282, 93306, 94330, 95354, 96378, 97402, 98426, 99450, 100474, 101498, 102522, 103546, 104570, 105594, 106618, 107642, 108666, 109690, 110714, 129146, 130170, 131194, 132218, 133242, 151674, 152698, 153722, 154746, 155770, 156794, 157818, 158842, 159866, 160890, 161914, 162938, 163962, 164986, 166010, 167034, 168058, 169082, 170106, 171130, 172154, 173178, 174202, 175226, 176250, 177274, 178298, 179322, 180346, 181370, 182394, 183418, 184442, 185466, 186490, 187514, 188538, 189562, 190586, 191610, 192634, 193658, 194682, 195706, 196730, 197754, 198778, 199802, 200826, 201850, 202874, 203898, 204922, 205946, 206970, 207994, 209018, 210042, 211066, 212090, 227450, 228474, 232570, 236666, 237690, 24700, 25724, 29820, 33916, 34940, 48252, 49276, 50300, 51324, 52348, 53372, 54396, 55420, 56444, 57468, 58492, 59516, 60540, 61564, 62588, 63612, 64636, 65660, 66684, 67708, 68732, 69756, 70780, 71804, 72828, 73852, 74876, 75900, 76924, 77948, 78972, 79996, 81020, 82044, 83068, 84092, 85116, 86140, 87164, 88188, 89212, 90236, 91260, 92284, 93308, 94332, 95356, 96380, 97404, 98428, 99452, 100476, 101500, 102524, 103548, 104572, 105596, 106620, 107644, 108668, 109692, 110716, 111740, 112764, 127100, 128124, 129148, 130172, 131196, 132220, 133244, 134268, 135292, 149628, 150652, 151676, 152700, 153724, 154748, 155772, 156796, 157820, 158844, 159868, 160892, 161916, 162940, 163964, 164988, 166012, 167036, 168060, 169084, 170108, 171132, 172156, 173180, 174204, 175228, 176252, 177276, 178300, 179324, 180348, 181372, 182396, 183420, 184444, 185468, 186492, 187516, 188540, 189564, 190588, 191612, 192636, 193660, 194684, 195708, 196732, 197756, 198780, 199804, 200828, 201852, 202876, 203900, 204924, 205948, 206972, 207996, 209020, 210044, 211068, 212092, 213116, 214140, 227452, 228476, 232572, 236668, 237692, 24702, 25726, 29822, 33918, 34942, 48254, 49278, 50302, 110718, 111742, 112766, 127102, 128126, 134270, 135294, 149630, 150654, 151678, 212094, 213118, 214142, 227454, 228478, 232574, 236670, 237694, 24704, 25728, 29824, 33920, 34944, 47232, 48256, 112768, 113792, 126080, 127104, 128128, 134272, 135296, 136320, 148608, 149632, 214144, 215168, 227456, 228480, 232576, 236672, 237696, 24706, 25730, 29826, 33922, 34946, 47234, 48258, 112770, 113794, 126082, 127106, 135298, 136322, 148610, 149634, 214146, 215170, 227458, 228482, 232578, 236674, 237698, 24708, 25732, 29828, 33924, 34948, 47236, 48260, 52356, 53380, 54404, 55428, 56452, 57476, 58500, 59524, 60548, 61572, 62596, 63620, 64644, 65668, 66692, 67716, 68740, 69764, 70788, 71812, 72836, 73860, 74884, 75908, 76932, 77956, 78980, 80004, 81028, 82052, 83076, 84100, 85124, 86148, 87172, 88196, 89220, 90244, 91268, 92292, 93316, 94340, 95364, 96388, 97412, 98436, 99460, 100484, 101508, 102532, 103556, 104580, 105604, 106628, 107652, 108676, 112772, 113796, 126084, 127108, 131204, 135300, 136324, 148612, 149636, 153732, 154756, 155780, 156804, 157828, 158852, 159876, 160900, 161924, 162948, 163972, 164996, 166020, 167044, 168068, 169092, 170116, 171140, 172164, 173188, 174212, 175236, 176260, 177284, 178308, 179332, 180356, 181380, 182404, 183428, 184452, 185476, 186500, 187524, 188548, 189572, 190596, 191620, 192644, 193668, 194692, 195716, 196740, 197764, 198788, 199812, 200836, 201860, 202884, 203908, 204932, 205956, 206980, 208004, 209028, 210052, 214148, 215172, 227460, 228484, 232580, 236676, 237700, 24710, 25734, 29830, 33926, 34950, 47238, 48262, 86150, 112774, 113798, 126086, 127110, 131206, 135302, 136326, 148614, 149638, 176262, 214150, 215174, 227462, 228486, 232582, 236678, 237702, 24712, 25736, 29832, 33928, 34952, 47240, 48264, 86152, 112776, 113800, 126088, 127112, 131208, 135304, 136328, 148616, 149640, 176264, 214152, 215176, 227464, 228488, 232584, 236680, 237704, 24714, 25738, 29834, 33930, 34954, 48266, 49290, 50314, 86154, 110730, 111754, 112778, 126090, 127114, 131210, 135306, 136330, 149642, 150666, 151690, 176266, 212106, 213130, 214154, 227466, 228490, 232586, 236682, 237706, 24716, 25740, 29836, 33932, 34956, 48268, 49292, 50316, 51340, 52364, 53388, 54412, 55436, 56460, 57484, 58508, 59532, 60556, 61580, 62604, 63628, 64652, 65676, 66700, 67724, 68748, 69772, 70796, 71820, 72844, 73868, 74892, 75916, 76940, 77964, 78988, 80012, 81036, 86156, 91276, 92300, 93324, 94348, 95372, 96396, 97420, 98444, 99468, 100492, 101516, 102540, 103564, 104588, 105612, 106636, 107660, 108684, 109708, 110732, 111756, 112780, 126092, 127116, 131212, 135308, 136332, 149644, 150668, 151692, 152716, 153740, 154764, 155788, 156812, 157836, 158860, 159884, 160908, 161932, 162956, 163980, 165004, 166028, 167052, 168076, 169100, 170124, 171148, 176268, 181388, 182412, 183436, 184460, 185484, 186508, 187532, 188556, 189580, 190604, 191628, 192652, 193676, 194700, 195724, 196748, 197772, 198796, 199820, 200844, 201868, 202892, 203916, 204940, 205964, 206988, 208012, 209036, 210060, 211084, 212108, 213132, 214156, 227468, 228492, 232588, 236684, 237708, 24718, 25742, 29838, 33934, 34958, 50318, 51342, 52366, 53390, 54414, 55438, 56462, 57486, 58510, 59534, 60558, 61582, 62606, 63630, 64654, 65678, 66702, 67726, 68750, 69774, 70798, 71822, 72846, 73870, 74894, 75918, 76942, 77966, 78990, 80014, 81038, 82062, 86158, 90254, 91278, 92302, 93326, 94350, 95374, 96398, 97422, 98446, 99470, 100494, 101518, 102542, 103566, 104590, 105614, 106638, 107662, 108686, 109710, 110734, 126094, 127118, 131214, 135310, 136334, 151694, 152718, 153742, 154766, 155790, 156814, 157838, 158862, 159886, 160910, 161934, 162958, 163982, 165006, 166030, 167054, 168078, 169102, 170126, 171150, 172174, 176270, 180366, 181390, 182414, 183438, 184462, 185486, 186510, 187534, 188558, 189582, 190606, 191630, 192654, 193678, 194702, 195726, 196750, 197774, 198798, 199822, 200846, 201870, 202894, 203918, 204942, 205966, 206990, 208014, 209038, 210062, 211086, 212110, 227470, 228494, 232590, 236686, 237710, 24720, 25744, 29840, 33936, 34960, 80016, 81040, 82064, 86160, 90256, 91280, 92304, 126096, 127120, 131216, 135312, 136336, 170128, 171152, 172176, 176272, 180368, 181392, 182416, 227472, 228496, 232592, 236688, 237712, 24722, 25746, 29842, 33938, 34962, 81042, 82066, 86162, 90258, 91282, 126098, 127122, 131218, 135314, 136338, 171154, 172178, 176274, 180370, 181394, 227474, 228498, 232594, 236690, 237714, 24724, 25748, 29844, 33940, 34964, 81044, 82068, 86164, 90260, 91284, 126100, 127124, 131220, 135316, 136340, 171156, 172180, 176276, 180372, 181396, 227476, 228500, 232596, 236692, 237716, 24726, 25750, 29846, 33942, 34966, 81046, 82070, 86166, 90262, 91286, 126102, 127126, 131222, 135318, 136342, 171158, 172182, 176278, 180374, 181398, 227478, 228502, 232598, 236694, 237718, 24728, 25752, 29848, 33944, 34968, 81048, 82072, 86168, 90264, 91288, 126104, 127128, 131224, 135320, 136344, 171160, 172184, 176280, 180376, 181400, 227480, 228504, 232600, 236696, 237720, 24730, 25754, 29850, 33946, 34970, 81050, 82074, 86170, 90266, 91290, 126106, 127130, 131226, 135322, 136346, 171162, 172186, 176282, 180378, 181402, 227482, 228506, 232602, 236698, 237722, 24732, 25756, 29852, 33948, 34972, 81052, 82076, 86172, 90268, 91292, 126108, 127132, 131228, 135324, 136348, 171164, 172188, 176284, 180380, 181404, 227484, 228508, 232604, 236700, 237724, 24734, 25758, 29854, 33950, 34974, 81054, 82078, 86174, 90270, 91294, 126110, 127134, 131230, 135326, 136350, 171166, 172190, 176286, 180382, 181406, 227486, 228510, 232606, 236702, 237726, 24736, 25760, 29856, 33952, 34976, 81056, 82080, 86176, 90272, 91296, 126112, 127136, 131232, 135328, 136352, 171168, 172192, 176288, 180384, 181408, 227488, 228512, 232608, 236704, 237728, 24738, 25762, 29858, 33954, 34978, 81058, 82082, 86178, 90274, 91298, 126114, 127138, 131234, 135330, 136354, 171170, 172194, 176290, 180386, 181410, 227490, 228514, 232610, 236706, 237730, 24740, 25764, 29860, 33956, 34980, 36004, 81060, 82084, 86180, 90276, 91300, 125092, 126116, 127140, 131236, 135332, 136356, 137380, 171172, 172196, 176292, 180388, 181412, 226468, 227492, 228516, 232612, 236708, 237732, 24742, 25766, 29862, 33958, 34982, 36006, 37030, 38054, 39078, 40102, 41126, 42150, 43174, 61606, 62630, 63654, 64678, 65702, 81062, 82086, 86182, 90278, 91302, 106662, 107686, 108710, 109734, 110758, 111782, 112806, 113830, 114854, 115878, 116902, 117926, 118950, 119974, 120998, 122022, 123046, 124070, 125094, 126118, 127142, 131238, 135334, 136358, 137382, 138406, 139430, 140454, 141478, 142502, 143526, 144550, 145574, 146598, 147622, 148646, 149670, 150694, 151718, 152742, 153766, 154790, 155814, 171174, 172198, 176294, 180390, 181414, 196774, 197798, 198822, 199846, 200870, 219302, 220326, 221350, 222374, 223398, 224422, 225446, 226470, 227494, 228518, 232614, 236710, 237734, 24744, 25768, 29864, 34984, 36008, 37032, 38056, 39080, 40104, 41128, 42152, 43176, 44200, 45224, 59560, 60584, 61608, 62632, 63656, 64680, 65704, 66728, 67752, 81064, 82088, 86184, 90280, 91304, 104616, 105640, 106664, 107688, 108712, 109736, 110760, 111784, 112808, 113832, 114856, 115880, 116904, 117928, 118952, 119976, 121000, 122024, 123048, 124072, 125096, 126120, 131240, 136360, 137384, 138408, 139432, 140456, 141480, 142504, 143528, 144552, 145576, 146600, 147624, 148648, 149672, 150696, 151720, 152744, 153768, 154792, 155816, 156840, 157864, 171176, 172200, 176296, 180392, 181416, 194728, 195752, 196776, 197800, 198824, 199848, 200872, 201896, 202920, 217256, 218280, 219304, 220328, 221352, 222376, 223400, 224424, 225448, 226472, 227496, 232616, 236712, 237736, 24746, 25770, 29866, 43178, 44202, 45226, 59562, 60586, 66730, 67754, 81066, 82090, 86186, 90282, 91306, 104618, 105642, 106666, 131242, 155818, 156842, 157866, 171178, 172202, 176298, 180394, 181418, 194730, 195754, 201898, 202922, 217258, 218282, 219306, 232618, 236714, 237738, 24748, 25772, 29868, 45228, 46252, 58540, 59564, 60588, 66732, 67756, 68780, 81068, 82092, 86188, 90284, 91308, 103596, 104620, 131244, 157868, 158892, 171180, 172204, 176300, 180396, 181420, 193708, 194732, 195756, 201900, 202924, 203948, 216236, 217260, 232620, 236716, 237740, 24750, 25774, 29870, 45230, 46254, 58542, 59566, 67758, 68782, 81070, 82094, 86190, 90286, 91310, 103598, 104622, 131246, 157870, 158894, 171182, 172206, 176302, 180398, 181422, 193710, 194734, 202926, 203950, 216238, 217262, 232622, 236718, 237742, 24752, 25776, 29872, 30896, 31920, 32944, 33968, 34992, 36016, 37040, 38064, 39088, 40112, 41136, 45232, 46256, 58544, 59568, 63664, 67760, 68784, 81072, 82096, 86192, 90288, 91312, 103600, 104624, 108720, 109744, 110768, 111792, 112816, 113840, 114864, 115888, 116912, 117936, 118960, 119984, 121008, 122032, 123056, 124080, 125104, 126128, 127152, 128176, 129200, 130224, 131248, 132272, 133296, 134320, 135344, 136368, 137392, 138416, 139440, 140464, 141488, 142512, 143536, 144560, 145584, 146608, 147632, 148656, 149680, 150704, 151728, 152752, 153776, 157872, 158896, 171184, 172208, 176304, 180400, 181424, 193712, 194736, 198832, 202928, 203952, 216240, 217264, 221360, 222384, 223408, 224432, 225456, 226480, 227504, 228528, 229552, 230576, 231600, 232624, 236720, 237744, 24754, 25778, 29874, 45234, 46258, 58546, 59570, 63666, 67762, 68786, 81074, 82098, 90290, 91314, 103602, 104626, 157874, 158898, 171186, 172210, 180402, 181426, 193714, 194738, 198834, 202930, 203954, 216242, 217266, 232626, 236722, 237746, 24756, 25780, 29876, 45236, 46260, 58548, 59572, 63668, 67764, 68788, 81076, 82100, 83124, 89268, 90292, 91316, 103604, 104628, 157876, 158900, 171188, 172212, 173236, 179380, 180404, 181428, 193716, 194740, 198836, 202932, 203956, 216244, 217268, 232628, 236724, 237748, 24758, 25782, 29878, 43190, 44214, 45238, 58550, 59574, 63670, 67766, 68790, 82102, 83126, 89270, 90294, 104630, 105654, 106678, 155830, 156854, 157878, 172214, 173238, 179382, 180406, 193718, 194742, 198838, 202934, 203958, 217270, 218294, 219318, 232630, 236726, 237750, 24760, 25784, 29880, 35000, 36024, 37048, 38072, 39096, 40120, 41144, 42168, 43192, 44216, 45240, 58552, 59576, 63672, 67768, 68792, 82104, 83128, 84152, 85176, 86200, 87224, 88248, 89272, 90296, 104632, 105656, 106680, 107704, 108728, 109752, 110776, 111800, 112824, 113848, 114872, 115896, 116920, 117944, 118968, 119992, 121016, 122040, 123064, 124088, 125112, 126136, 127160, 128184, 129208, 130232, 131256, 132280, 133304, 134328, 135352, 136376, 137400, 138424, 139448, 140472, 141496, 142520, 143544, 144568, 145592, 146616, 147640, 148664, 149688, 150712, 151736, 152760, 153784, 154808, 155832, 156856, 157880, 172216, 173240, 174264, 175288, 176312, 177336, 178360, 179384, 180408, 193720, 194744, 198840, 202936, 203960, 217272, 218296, 219320, 220344, 221368, 222392, 223416, 224440, 225464, 226488, 227512, 232632, 236728, 237752, 24762, 25786, 29882, 33978, 35002, 36026, 37050, 38074, 39098, 40122, 41146, 42170, 43194, 58554, 59578, 63674, 67770, 68794, 84154, 85178, 86202, 87226, 88250, 106682, 107706, 108730, 109754, 110778, 111802, 112826, 113850, 114874, 115898, 116922, 117946, 118970, 119994, 121018, 122042, 123066, 124090, 125114, 126138, 127162, 128186, 129210, 130234, 131258, 132282, 133306, 134330, 135354, 136378, 137402, 138426, 139450, 140474, 141498, 142522, 143546, 144570, 145594, 146618, 147642, 148666, 149690, 150714, 151738, 152762, 153786, 154810, 155834, 174266, 175290, 176314, 177338, 178362, 193722, 194746, 198842, 202938, 203962, 219322, 220346, 221370, 222394, 223418, 224442, 225466, 226490, 227514, 228538, 232634, 236730, 237754, 24764, 25788, 29884, 33980, 35004, 36028, 58556, 59580, 63676, 67772, 68796, 193724, 194748, 198844, 202940, 203964, 226492, 227516, 228540, 232636, 236732, 237756, 24766, 25790, 29886, 33982, 35006, 58558, 59582, 63678, 67774, 68798, 193726, 194750, 198846, 202942, 203966, 227518, 228542, 232638, 236734, 237758, 24768, 25792, 29888, 33984, 35008, 58560, 59584, 63680, 67776, 68800, 193728, 194752, 198848, 202944, 203968, 227520, 228544, 232640, 236736, 237760, 24770, 25794, 29890, 33986, 35010, 58562, 59586, 63682, 67778, 68802, 193730, 194754, 198850, 202946, 203970, 227522, 228546, 232642, 236738, 237762, 24772, 25796, 29892, 33988, 35012, 58564, 59588, 63684, 67780, 68804, 193732, 194756, 198852, 202948, 203972, 227524, 228548, 232644, 236740, 237764, 24774, 25798, 29894, 33990, 35014, 58566, 59590, 63686, 67782, 68806, 193734, 194758, 198854, 202950, 203974, 227526, 228550, 232646, 236742, 237766, 24776, 25800, 29896, 33992, 35016, 58568, 59592, 63688, 67784, 68808, 193736, 194760, 198856, 202952, 203976, 227528, 228552, 232648, 236744, 237768, 24778, 25802, 29898, 33994, 35018, 58570, 59594, 63690, 67786, 68810, 193738, 194762, 198858, 202954, 203978, 227530, 228554, 232650, 236746, 237770, 24780, 25804, 29900, 33996, 35020, 58572, 59596, 63692, 67788, 68812, 193740, 194764, 198860, 202956, 203980, 227532, 228556, 232652, 236748, 237772, 24782, 25806, 29902, 33998, 35022, 58574, 59598, 63694, 67790, 68814, 193742, 194766, 198862, 202958, 203982, 227534, 228558, 232654, 236750, 237774, 24784, 25808, 29904, 34000, 35024, 57552, 58576, 59600, 63696, 67792, 68816, 193744, 194768, 198864, 202960, 203984, 205008, 227536, 228560, 232656, 236752, 237776, 24786, 25810, 29906, 34002, 35026, 50386, 51410, 52434, 53458, 54482, 55506, 56530, 57554, 58578, 59602, 63698, 67794, 68818, 84178, 85202, 86226, 87250, 88274, 89298, 90322, 91346, 92370, 93394, 94418, 95442, 96466, 97490, 98514, 99538, 100562, 101586, 102610, 103634, 104658, 105682, 106706, 107730, 108754, 109778, 110802, 129234, 130258, 131282, 132306, 133330, 151762, 152786, 153810, 154834, 155858, 156882, 157906, 158930, 159954, 160978, 162002, 163026, 164050, 165074, 166098, 167122, 168146, 169170, 170194, 171218, 172242, 173266, 174290, 175314, 176338, 177362, 178386, 193746, 194770, 198866, 202962, 203986, 205010, 206034, 207058, 208082, 209106, 210130, 211154, 212178, 227538, 228562, 232658, 236754, 237778, 24788, 25812, 29908, 34004, 35028, 48340, 49364, 50388, 51412, 52436, 53460, 54484, 55508, 56532, 57556, 58580, 63700, 67796, 68820, 82132, 83156, 84180, 85204, 86228, 87252, 88276, 89300, 90324, 91348, 92372, 93396, 94420, 95444, 96468, 97492, 98516, 99540, 100564, 101588, 102612, 103636, 104660, 105684, 106708, 107732, 108756, 109780, 110804, 111828, 112852, 127188, 128212, 129236, 130260, 131284, 132308, 133332, 134356, 135380, 149716, 150740, 151764, 152788, 153812, 154836, 155860, 156884, 157908, 158932, 159956, 160980, 162004, 163028, 164052, 165076, 166100, 167124, 168148, 169172, 170196, 171220, 172244, 173268, 174292, 175316, 176340, 177364, 178388, 179412, 180436, 193748, 194772, 198868, 203988, 205012, 206036, 207060, 208084, 209108, 210132, 211156, 212180, 213204, 214228, 227540, 228564, 232660, 236756, 237780, 24790, 25814, 29910, 34006, 35030, 48342, 49366, 50390, 63702, 67798, 68822, 82134, 83158, 84182, 110806, 111830, 112854, 127190, 128214, 134358, 135382, 149718, 150742, 151766, 178390, 179414, 180438, 193750, 194774, 198870, 212182, 213206, 214230, 227542, 228566, 232662, 236758, 237782, 24792, 25816, 29912, 34008, 35032, 47320, 48344, 63704, 67800, 68824, 81112, 82136, 112856, 113880, 126168, 127192, 128216, 134360, 135384, 136408, 148696, 149720, 180440, 181464, 193752, 194776, 198872, 214232, 215256, 227544, 228568, 232664, 236760, 237784, 24794, 25818, 29914, 34010, 35034, 47322, 48346, 63706, 67802, 68826, 81114, 82138, 112858, 113882, 126170, 127194, 135386, 136410, 148698, 149722, 180442, 181466, 193754, 194778, 198874, 214234, 215258, 227546, 228570, 232666, 236762, 237786, 24796, 25820, 29916, 34012, 35036, 47324, 48348, 52444, 53468, 54492, 55516, 56540, 57564, 58588, 59612, 60636, 61660, 62684, 63708, 67804, 68828, 81116, 82140, 86236, 87260, 88284, 89308, 90332, 91356, 92380, 93404, 94428, 95452, 96476, 97500, 98524, 99548, 100572, 101596, 102620, 103644, 104668, 105692, 106716, 107740, 108764, 112860, 113884, 126172, 127196, 131292, 135388, 136412, 148700, 149724, 153820, 154844, 155868, 156892, 157916, 158940, 159964, 160988, 162012, 163036, 164060, 165084, 166108, 167132, 168156, 169180, 170204, 171228, 172252, 173276, 174300, 175324, 176348, 180444, 181468, 193756, 194780, 198876, 199900, 200924, 201948, 202972, 203996, 205020, 206044, 207068, 208092, 209116, 210140, 214236, 215260, 227548, 228572, 232668, 236764, 237788, 24798, 25822, 29918, 34014, 35038, 47326, 48350, 67806, 68830, 81118, 82142, 112862, 113886, 126174, 127198, 131294, 135390, 136414, 148702, 149726, 180446, 181470, 193758, 194782, 214238, 215262, 227550, 228574, 232670, 236766, 237790, 24800, 25824, 29920, 34016, 35040, 47328, 48352, 67808, 68832, 81120, 82144, 112864, 113888, 126176, 127200, 131296, 135392, 136416, 148704, 149728, 180448, 181472, 193760, 194784, 214240, 215264, 227552, 228576, 232672, 236768, 237792, 24802, 25826, 29922, 34018, 35042, 48354, 49378, 50402, 66786, 67810, 68834, 82146, 83170, 84194, 110818, 111842, 112866, 126178, 127202, 131298, 135394, 136418, 149730, 150754, 151778, 178402, 179426, 180450, 193762, 194786, 195810, 212194, 213218, 214242, 227554, 228578, 232674, 236770, 237794, 24804, 25828, 29924, 34020, 35044, 48356, 49380, 50404, 51428, 52452, 53476, 54500, 55524, 56548, 57572, 58596, 59620, 60644, 61668, 62692, 63716, 64740, 65764, 66788, 67812, 68836, 82148, 83172, 84196, 85220, 86244, 87268, 88292, 89316, 90340, 91364, 92388, 93412, 94436, 95460, 96484, 97508, 98532, 99556, 100580, 101604, 102628, 103652, 104676, 105700, 106724, 107748, 108772, 109796, 110820, 111844, 112868, 126180, 127204, 131300, 135396, 136420, 149732, 150756, 151780, 152804, 153828, 154852, 155876, 156900, 157924, 158948, 159972, 160996, 162020, 163044, 164068, 165092, 166116, 167140, 168164, 169188, 170212, 171236, 172260, 173284, 174308, 175332, 176356, 177380, 178404, 179428, 180452, 193764, 194788, 195812, 196836, 197860, 198884, 199908, 200932, 201956, 202980, 204004, 205028, 206052, 207076, 208100, 209124, 210148, 211172, 212196, 213220, 214244, 227556, 228580, 232676, 236772, 237796, 24806, 25830, 29926, 34022, 35046, 50406, 51430, 52454, 53478, 54502, 55526, 56550, 57574, 58598, 59622, 60646, 61670, 62694, 63718, 64742, 65766, 66790, 67814, 84198, 85222, 86246, 87270, 88294, 89318, 90342, 91366, 92390, 93414, 94438, 95462, 96486, 97510, 98534, 99558, 100582, 101606, 102630, 103654, 104678, 105702, 106726, 107750, 108774, 109798, 110822, 126182, 127206, 131302, 135398, 136422, 151782, 152806, 153830, 154854, 155878, 156902, 157926, 158950, 159974, 160998, 162022, 163046, 164070, 165094, 166118, 167142, 168166, 169190, 170214, 171238, 172262, 173286, 174310, 175334, 176358, 177382, 178406, 194790, 195814, 196838, 197862, 198886, 199910, 200934, 201958, 202982, 204006, 205030, 206054, 207078, 208102, 209126, 210150, 211174, 212198, 227558, 228582, 232678, 236774, 237798, 24808, 25832, 29928, 34024, 35048, 126184, 127208, 131304, 135400, 136424, 227560, 228584, 232680, 236776, 237800, 24810, 25834, 29930, 34026, 35050, 126186, 127210, 131306, 135402, 136426, 227562, 228586, 232682, 236778, 237802, 24812, 25836, 29932, 34028, 35052, 126188, 127212, 131308, 135404, 136428, 227564, 228588, 232684, 236780, 237804, 24814, 25838, 29934, 34030, 35054, 126190, 127214, 131310, 135406, 136430, 227566, 228590, 232686, 236782, 237806, 24816, 25840, 29936, 34032, 35056, 126192, 127216, 131312, 135408, 136432, 227568, 228592, 232688, 236784, 237808, 24818, 25842, 29938, 34034, 35058, 126194, 127218, 131314, 135410, 136434, 227570, 228594, 232690, 236786, 237810, 24820, 25844, 29940, 34036, 35060, 126196, 127220, 131316, 135412, 136436, 227572, 228596, 232692, 236788, 237812, 24822, 25846, 29942, 34038, 35062, 126198, 127222, 131318, 135414, 136438, 227574, 228598, 232694, 236790, 237814, 24824, 25848, 29944, 34040, 35064, 126200, 127224, 131320, 135416, 136440, 227576, 228600, 232696, 236792, 237816, 24826, 25850, 29946, 34042, 35066, 126202, 127226, 131322, 135418, 136442, 227578, 228602, 232698, 236794, 237818, 24828, 25852, 29948, 34044, 35068, 36092, 125180, 126204, 127228, 131324, 135420, 136444, 137468, 226556, 227580, 228604, 232700, 236796, 237820, 24830, 25854, 29950, 34046, 35070, 36094, 37118, 38142, 39166, 40190, 41214, 42238, 43262, 44286, 45310, 46334, 47358, 48382, 49406, 50430, 51454, 52478, 53502, 54526, 55550, 56574, 57598, 58622, 59646, 60670, 61694, 62718, 63742, 64766, 65790, 66814, 67838, 84222, 85246, 86270, 87294, 88318, 106750, 107774, 108798, 109822, 110846, 111870, 112894, 113918, 114942, 115966, 116990, 118014, 119038, 120062, 121086, 122110, 123134, 124158, 125182, 126206, 127230, 131326, 135422, 136446, 137470, 138494, 139518, 140542, 141566, 142590, 143614, 144638, 145662, 146686, 147710, 148734, 149758, 150782, 151806, 152830, 153854, 154878, 155902, 174334, 175358, 176382, 177406, 178430, 194814, 195838, 196862, 197886, 198910, 199934, 200958, 201982, 203006, 204030, 205054, 206078, 207102, 208126, 209150, 210174, 211198, 212222, 213246, 214270, 215294, 216318, 217342, 218366, 219390, 220414, 221438, 222462, 223486, 224510, 225534, 226558, 227582, 228606, 232702, 236798, 237822, 24832, 25856, 29952, 35072, 36096, 37120, 38144, 39168, 40192, 41216, 42240, 43264, 44288, 45312, 46336, 47360, 48384, 49408, 50432, 51456, 52480, 53504, 54528, 55552, 56576, 57600, 58624, 59648, 60672, 61696, 62720, 63744, 64768, 65792, 66816, 67840, 68864, 82176, 83200, 84224, 85248, 86272, 87296, 88320, 89344, 90368, 104704, 105728, 106752, 107776, 108800, 109824, 110848, 111872, 112896, 113920, 114944, 115968, 116992, 118016, 119040, 120064, 121088, 122112, 123136, 124160, 125184, 126208, 131328, 136448, 137472, 138496, 139520, 140544, 141568, 142592, 143616, 144640, 145664, 146688, 147712, 148736, 149760, 150784, 151808, 152832, 153856, 154880, 155904, 156928, 157952, 172288, 173312, 174336, 175360, 176384, 177408, 178432, 179456, 180480, 193792, 194816, 195840, 196864, 197888, 198912, 199936, 200960, 201984, 203008, 204032, 205056, 206080, 207104, 208128, 209152, 210176, 211200, 212224, 213248, 214272, 215296, 216320, 217344, 218368, 219392, 220416, 221440, 222464, 223488, 224512, 225536, 226560, 227584, 232704, 236800, 237824, 24834, 25858, 29954, 66818, 67842, 68866, 82178, 83202, 89346, 90370, 104706, 105730, 106754, 131330, 155906, 156930, 157954, 172290, 173314, 179458, 180482, 193794, 194818, 195842, 232706, 236802, 237826, 24836, 25860, 29956, 67844, 68868, 81156, 82180, 83204, 89348, 90372, 91396, 103684, 104708, 131332, 157956, 158980, 171268, 172292, 173316, 179460, 180484, 181508, 193796, 194820, 232708, 236804, 237828, 24838, 25862, 29958, 67846, 68870, 81158, 82182, 90374, 91398, 103686, 104710, 131334, 157958, 158982, 171270, 172294, 180486, 181510, 193798, 194822, 232710, 236806, 237830, 24840, 25864, 29960, 30984, 32008, 33032, 34056, 35080, 36104, 37128, 38152, 39176, 40200, 41224, 42248, 43272, 44296, 45320, 46344, 47368, 48392, 49416, 50440, 51464, 52488, 53512, 54536, 55560, 56584, 57608, 58632, 59656, 60680, 61704, 62728, 63752, 67848, 68872, 81160, 82184, 86280, 90376, 91400, 103688, 104712, 108808, 109832, 110856, 111880, 112904, 113928, 114952, 115976, 117000, 118024, 119048, 120072, 121096, 122120, 123144, 124168, 125192, 126216, 127240, 128264, 129288, 130312, 131336, 132360, 133384, 134408, 135432, 136456, 137480, 138504, 139528, 140552, 141576, 142600, 143624, 144648, 145672, 146696, 147720, 148744, 149768, 150792, 151816, 152840, 153864, 157960, 158984, 171272, 172296, 176392, 180488, 181512, 193800, 194824, 198920, 199944, 200968, 201992, 203016, 204040, 205064, 206088, 207112, 208136, 209160, 210184, 211208, 212232, 213256, 214280, 215304, 216328, 217352, 218376, 219400, 220424, 221448, 222472, 223496, 224520, 225544, 226568, 227592, 228616, 229640, 230664, 231688, 232712, 236808, 237832, 24842, 25866, 63754, 67850, 68874, 81162, 82186, 86282, 90378, 91402, 103690, 104714, 157962, 158986, 171274, 172298, 176394, 180490, 181514, 193802, 194826, 198922, 236810, 237834, 24844, 25868, 63756, 67852, 68876, 81164, 82188, 86284, 90380, 91404, 103692, 104716, 157964, 158988, 171276, 172300, 176396, 180492, 181516, 193804, 194828, 198924, 236812, 237836, 24846, 25870, 26894, 63758, 67854, 68878, 81166, 82190, 86286, 90382, 91406, 104718, 105742, 106766, 155918, 156942, 157966, 171278, 172302, 176398, 180494, 181518, 193806, 194830, 198926, 235790, 236814, 237838, 24848, 25872, 26896, 27920, 28944, 29968, 30992, 32016, 33040, 34064, 35088, 36112, 37136, 38160, 39184, 40208, 41232, 42256, 43280, 44304, 45328, 46352, 47376, 48400, 49424, 50448, 51472, 52496, 53520, 54544, 55568, 56592, 57616, 58640, 63760, 67856, 68880, 81168, 82192, 86288, 90384, 91408, 104720, 105744, 106768, 107792, 108816, 109840, 110864, 111888, 112912, 113936, 114960, 115984, 117008, 118032, 119056, 120080, 121104, 122128, 123152, 124176, 125200, 126224, 127248, 128272, 129296, 130320, 131344, 132368, 133392, 134416, 135440, 136464, 137488, 138512, 139536, 140560, 141584, 142608, 143632, 144656, 145680, 146704, 147728, 148752, 149776, 150800, 151824, 152848, 153872, 154896, 155920, 156944, 157968, 171280, 172304, 176400, 180496, 181520, 193808, 194832, 198928, 204048, 205072, 206096, 207120, 208144, 209168, 210192, 211216, 212240, 213264, 214288, 215312, 216336, 217360, 218384, 219408, 220432, 221456, 222480, 223504, 224528, 225552, 226576, 227600, 228624, 229648, 230672, 231696, 232720, 233744, 234768, 235792, 236816, 237840, 25874, 26898, 27922, 28946, 29970, 30994, 32018, 33042, 34066, 35090, 36114, 37138, 38162, 39186, 40210, 41234, 42258, 43282, 44306, 45330, 46354, 47378, 48402, 49426, 50450, 51474, 52498, 53522, 54546, 55570, 56594, 57618, 58642, 59666, 63762, 67858, 68882, 81170, 82194, 86290, 90386, 91410, 106770, 107794, 108818, 109842, 110866, 111890, 112914, 113938, 114962, 115986, 117010, 118034, 119058, 120082, 121106, 122130, 123154, 124178, 125202, 126226, 127250, 128274, 129298, 130322, 131346, 132370, 133394, 134418, 135442, 136466, 137490, 138514, 139538, 140562, 141586, 142610, 143634, 144658, 145682, 146706, 147730, 148754, 149778, 150802, 151826, 152850, 153874, 154898, 155922, 171282, 172306, 176402, 180498, 181522, 193810, 194834, 198930, 203026, 204050, 205074, 206098, 207122, 208146, 209170, 210194, 211218, 212242, 213266, 214290, 215314, 216338, 217362, 218386, 219410, 220434, 221458, 222482, 223506, 224530, 225554, 226578, 227602, 228626, 229650, 230674, 231698, 232722, 233746, 234770, 235794, 236818, 57620, 58644, 59668, 63764, 67860, 68884, 81172, 82196, 86292, 90388, 91412, 171284, 172308, 176404, 180500, 181524, 193812, 194836, 198932, 203028, 204052, 205076, 58646, 59670, 63766, 67862, 68886, 81174, 82198, 86294, 90390, 91414, 171286, 172310, 176406, 180502, 181526, 193814, 194838, 198934, 203030, 204054, 58648, 59672, 63768, 67864, 68888, 81176, 82200, 86296, 90392, 91416, 171288, 172312, 176408, 180504, 181528, 193816, 194840, 198936, 203032, 204056, 58650, 59674, 63770, 67866, 68890, 81178, 82202, 86298, 90394, 91418, 171290, 172314, 176410, 180506, 181530, 193818, 194842, 198938, 203034, 204058, 58652, 59676, 63772, 67868, 68892, 81180, 82204, 86300, 90396, 91420, 171292, 172316, 176412, 180508, 181532, 193820, 194844, 198940, 203036, 204060, 58654, 59678, 63774, 67870, 68894, 81182, 82206, 86302, 90398, 91422, 171294, 172318, 176414, 180510, 181534, 193822, 194846, 198942, 203038, 204062, 58656, 59680, 63776, 67872, 68896, 81184, 82208, 86304, 90400, 91424, 171296, 172320, 176416, 180512, 181536, 193824, 194848, 198944, 203040, 204064, 58658, 59682, 63778, 67874, 68898, 81186, 82210, 86306, 90402, 91426, 171298, 172322, 176418, 180514, 181538, 193826, 194850, 198946, 203042, 204066, 58660, 59684, 63780, 67876, 68900, 81188, 82212, 86308, 90404, 91428, 171300, 172324, 176420, 180516, 181540, 193828, 194852, 198948, 203044, 204068, 58662, 59686, 63782, 67878, 68902, 81190, 82214, 86310, 90406, 91430, 171302, 172326, 176422, 180518, 181542, 193830, 194854, 198950, 203046, 204070, 58664, 59688, 63784, 67880, 68904, 81192, 82216, 86312, 90408, 91432, 171304, 172328, 176424, 180520, 181544, 193832, 194856, 198952, 203048, 204072, 58666, 59690, 63786, 67882, 68906, 81194, 82218, 86314, 90410, 91434, 104746, 105770, 106794, 107818, 108842, 109866, 110890, 111914, 112938, 113962, 114986, 116010, 117034, 118058, 119082, 120106, 121130, 122154, 123178, 124202, 125226, 126250, 127274, 128298, 129322, 130346, 131370, 132394, 133418, 134442, 135466, 136490, 137514, 138538, 139562, 140586, 141610, 142634, 143658, 144682, 145706, 146730, 147754, 148778, 149802, 150826, 151850, 152874, 153898, 154922, 155946, 156970, 157994, 171306, 172330, 176426, 180522, 181546, 193834, 194858, 198954, 203050, 204074, 58668, 59692, 63788, 67884, 68908, 81196, 82220, 86316, 90412, 91436, 103724, 104748, 105772, 106796, 107820, 108844, 109868, 110892, 111916, 112940, 113964, 114988, 116012, 117036, 118060, 119084, 120108, 121132, 122156, 123180, 124204, 125228, 126252, 127276, 128300, 129324, 130348, 131372, 132396, 133420, 134444, 135468, 136492, 137516, 138540, 139564, 140588, 141612, 142636, 143660, 144684, 145708, 146732, 147756, 148780, 149804, 150828, 151852, 152876, 153900, 154924, 155948, 156972, 157996, 159020, 171308, 172332, 176428, 180524, 181548, 193836, 194860, 198956, 203052, 204076, 58670, 59694, 63790, 67886, 68910, 81198, 82222, 86318, 90414, 91438, 103726, 104750, 105774, 156974, 157998, 159022, 171310, 172334, 176430, 180526, 181550, 193838, 194862, 198958, 203054, 204078, 58672, 59696, 63792, 67888, 68912, 81200, 82224, 86320, 90416, 91440, 103728, 104752, 158000, 159024, 171312, 172336, 176432, 180528, 181552, 193840, 194864, 198960, 203056, 204080, 58674, 59698, 63794, 67890, 68914, 81202, 82226, 86322, 90418, 91442, 103730, 104754, 158002, 159026, 171314, 172338, 176434, 180530, 181554, 193842, 194866, 198962, 203058, 204082, 58676, 59700, 63796, 67892, 68916, 81204, 82228, 86324, 90420, 91444, 103732, 104756, 108852, 109876, 110900, 111924, 112948, 113972, 114996, 116020, 117044, 118068, 119092, 120116, 121140, 122164, 123188, 124212, 125236, 126260, 127284, 128308, 129332, 130356, 131380, 132404, 133428, 134452, 135476, 136500, 137524, 138548, 139572, 140596, 141620, 142644, 143668, 144692, 145716, 146740, 147764, 148788, 149812, 150836, 151860, 152884, 153908, 158004, 159028, 171316, 172340, 176436, 180532, 181556, 193844, 194868, 198964, 203060, 204084, 58678, 59702, 63798, 67894, 68918, 81206, 82230, 90422, 91446, 103734, 104758, 108854, 153910, 158006, 159030, 171318, 172342, 180534, 181558, 193846, 194870, 198966, 203062, 204086, 58680, 59704, 63800, 67896, 68920, 81208, 82232, 83256, 89400, 90424, 91448, 103736, 104760, 108856, 153912, 158008, 159032, 171320, 172344, 173368, 179512, 180536, 181560, 193848, 194872, 198968, 203064, 204088, 58682, 59706, 63802, 67898, 68922, 82234, 83258, 89402, 90426, 103738, 104762, 108858, 153914, 158010, 159034, 172346, 173370, 179514, 180538, 193850, 194874, 198970, 203066, 204090, 58684, 59708, 63804, 67900, 68924, 82236, 83260, 84284, 85308, 86332, 87356, 88380, 89404, 90428, 103740, 104764, 108860, 113980, 115004, 116028, 117052, 118076, 119100, 120124, 121148, 122172, 123196, 124220, 125244, 126268, 127292, 128316, 129340, 130364, 131388, 132412, 133436, 134460, 135484, 136508, 137532, 138556, 139580, 140604, 141628, 142652, 143676, 144700, 145724, 146748, 147772, 148796, 153916, 158012, 159036, 172348, 173372, 174396, 175420, 176444, 177468, 178492, 179516, 180540, 193852, 194876, 198972, 203068, 204092, 58686, 59710, 63806, 67902, 68926, 84286, 85310, 86334, 87358, 88382, 103742, 104766, 108862, 112958, 113982, 115006, 116030, 117054, 118078, 119102, 120126, 121150, 122174, 123198, 124222, 125246, 126270, 127294, 128318, 129342, 130366, 131390, 132414, 133438, 134462, 135486, 136510, 137534, 138558, 139582, 140606, 141630, 142654, 143678, 144702, 145726, 146750, 147774, 148798, 149822, 153918, 158014, 159038, 174398, 175422, 176446, 177470, 178494, 193854, 194878, 198974, 203070, 204094, 58688, 59712, 63808, 67904, 68928, 103744, 104768, 108864, 112960, 113984, 115008, 147776, 148800, 149824, 153920, 158016, 159040, 193856, 194880, 198976, 203072, 204096, 58690, 59714, 63810, 67906, 68930, 103746, 104770, 108866, 112962, 113986, 148802, 149826, 153922, 158018, 159042, 193858, 194882, 198978, 203074, 204098, 58692, 59716, 63812, 67908, 68932, 103748, 104772, 108868, 112964, 113988, 148804, 149828, 153924, 158020, 159044, 193860, 194884, 198980, 203076, 204100, 58694, 59718, 63814, 67910, 68934, 103750, 104774, 108870, 112966, 113990, 148806, 149830, 153926, 158022, 159046, 193862, 194886, 198982, 203078, 204102, 58696, 59720, 63816, 67912, 68936, 103752, 104776, 108872, 112968, 113992, 148808, 149832, 153928, 158024, 159048, 193864, 194888, 198984, 203080, 204104, 58698, 59722, 63818, 67914, 68938, 103754, 104778, 108874, 112970, 113994, 148810, 149834, 153930, 158026, 159050, 193866, 194890, 198986, 203082, 204106, 58700, 59724, 63820, 67916, 68940, 103756, 104780, 108876, 112972, 113996, 148812, 149836, 153932, 158028, 159052, 193868, 194892, 198988, 203084, 204108, 58702, 59726, 63822, 67918, 68942, 103758, 104782, 108878, 112974, 113998, 148814, 149838, 153934, 158030, 159054, 193870, 194894, 198990, 203086, 204110, 58704, 59728, 63824, 67920, 68944, 103760, 104784, 108880, 112976, 114000, 148816, 149840, 153936, 158032, 159056, 193872, 194896, 198992, 203088, 204112, 58706, 59730, 63826, 67922, 68946, 103762, 104786, 108882, 112978, 114002, 148818, 149842, 153938, 158034, 159058, 193874, 194898, 198994, 203090, 204114, 58708, 59732, 63828, 67924, 68948, 103764, 104788, 108884, 112980, 114004, 115028, 147796, 148820, 149844, 153940, 158036, 159060, 193876, 194900, 198996, 203092, 204116, 58710, 59734, 63830, 67926, 68950, 84310, 85334, 86358, 87382, 88406, 103766, 104790, 108886, 112982, 114006, 115030, 116054, 117078, 118102, 119126, 120150, 121174, 122198, 140630, 141654, 142678, 143702, 144726, 145750, 146774, 147798, 148822, 149846, 153942, 158038, 159062, 174422, 175446, 176470, 177494, 178518, 193878, 194902, 198998, 203094, 204118, 58712, 59736, 63832, 67928, 68952, 82264, 83288, 84312, 85336, 86360, 87384, 88408, 89432, 90456, 103768, 104792, 108888, 114008, 115032, 116056, 117080, 118104, 119128, 120152, 121176, 122200, 123224, 124248, 138584, 139608, 140632, 141656, 142680, 143704, 144728, 145752, 146776, 147800, 148824, 153944, 158040, 159064, 172376, 173400, 174424, 175448, 176472, 177496, 178520, 179544, 180568, 193880, 194904, 199000, 203096, 204120, 58714, 59738, 63834, 67930, 68954, 82266, 83290, 89434, 90458, 103770, 104794, 108890, 122202, 123226, 124250, 138586, 139610, 140634, 153946, 158042, 159066, 172378, 173402, 179546, 180570, 193882, 194906, 199002, 203098, 204122, 58716, 59740, 63836, 67932, 68956, 81244, 82268, 83292, 89436, 90460, 91484, 103772, 104796, 108892, 124252, 125276, 137564, 138588, 153948, 158044, 159068, 171356, 172380, 173404, 179548, 180572, 181596, 193884, 194908, 199004, 203100, 204124, 58718, 59742, 63838, 67934, 68958, 81246, 82270, 90462, 91486, 103774, 104798, 108894, 124254, 125278, 137566, 138590, 153950, 158046, 159070, 171358, 172382, 180574, 181598, 193886, 194910, 199006, 203102, 204126, 58720, 59744, 63840, 67936, 68960, 81248, 82272, 86368, 90464, 91488, 103776, 104800, 108896, 109920, 110944, 111968, 112992, 114016, 115040, 116064, 117088, 118112, 119136, 120160, 124256, 125280, 137568, 138592, 142688, 143712, 144736, 145760, 146784, 147808, 148832, 149856, 150880, 151904, 152928, 153952, 158048, 159072, 171360, 172384, 176480, 180576, 181600, 193888, 194912, 199008, 203104, 204128, 58722, 59746, 63842, 67938, 68962, 81250, 82274, 86370, 90466, 91490, 103778, 104802, 124258, 125282, 137570, 138594, 158050, 159074, 171362, 172386, 176482, 180578, 181602, 193890, 194914, 199010, 203106, 204130, 58724, 59748, 63844, 67940, 68964, 81252, 82276, 86372, 90468, 91492, 103780, 104804, 124260, 125284, 137572, 138596, 158052, 159076, 171364, 172388, 176484, 180580, 181604, 193892, 194916, 199012, 203108, 204132, 58726, 59750, 63846, 67942, 68966, 81254, 82278, 86374, 90470, 91494, 103782, 104806, 105830, 122214, 123238, 124262, 138598, 139622, 140646, 157030, 158054, 159078, 171366, 172390, 176486, 180582, 181606, 193894, 194918, 199014, 203110, 204134, 58728, 59752, 63848, 67944, 68968, 81256, 82280, 86376, 90472, 91496, 103784, 104808, 105832, 106856, 107880, 108904, 109928, 110952, 111976, 113000, 114024, 115048, 116072, 117096, 118120, 119144, 120168, 121192, 122216, 123240, 124264, 138600, 139624, 140648, 141672, 142696, 143720, 144744, 145768, 146792, 147816, 148840, 149864, 150888, 151912, 152936, 153960, 154984, 156008, 157032, 158056, 159080, 171368, 172392, 176488, 180584, 181608, 193896, 194920, 199016, 203112, 204136, 58730, 59754, 63850, 67946, 68970, 81258, 82282, 86378, 90474, 91498, 104810, 105834, 106858, 107882, 108906, 109930, 110954, 111978, 113002, 114026, 115050, 116074, 117098, 118122, 119146, 120170, 121194, 122218, 140650, 141674, 142698, 143722, 144746, 145770, 146794, 147818, 148842, 149866, 150890, 151914, 152938, 153962, 154986, 156010, 157034, 158058, 171370, 172394, 176490, 180586, 181610, 193898, 194922, 199018, 203114, 204138, 58732, 59756, 63852, 67948, 68972, 81260, 82284, 86380, 90476, 91500, 171372, 172396, 176492, 180588, 181612, 193900, 194924, 199020, 203116, 204140, 58734, 59758, 63854, 67950, 68974, 81262, 82286, 86382, 90478, 91502, 171374, 172398, 176494, 180590, 181614, 193902, 194926, 199022, 203118, 204142, 58736, 59760, 63856, 67952, 68976, 81264, 82288, 86384, 90480, 91504, 171376, 172400, 176496, 180592, 181616, 193904, 194928, 199024, 203120, 204144, 58738, 59762, 63858, 67954, 68978, 81266, 82290, 86386, 90482, 91506, 171378, 172402, 176498, 180594, 181618, 193906, 194930, 199026, 203122, 204146, 58740, 59764, 63860, 67956, 68980, 81268, 82292, 86388, 90484, 91508, 171380, 172404, 176500, 180596, 181620, 193908, 194932, 199028, 203124, 204148, 58742, 59766, 63862, 67958, 68982, 81270, 82294, 86390, 90486, 91510, 171382, 172406, 176502, 180598, 181622, 193910, 194934, 199030, 203126, 204150, 58744, 59768, 63864, 67960, 68984, 81272, 82296, 86392, 90488, 91512, 171384, 172408, 176504, 180600, 181624, 193912, 194936, 199032, 203128, 204152, 58746, 59770, 63866, 67962, 68986, 81274, 82298, 86394, 90490, 91514, 171386, 172410, 176506, 180602, 181626, 193914, 194938, 199034, 203130, 204154, 58748, 59772, 63868, 67964, 68988, 81276, 82300, 86396, 90492, 91516, 171388, 172412, 176508, 180604, 181628, 193916, 194940, 199036, 203132, 204156, 58750, 59774, 63870, 67966, 68990, 81278, 82302, 86398, 90494, 91518, 171390, 172414, 176510, 180606, 181630, 193918, 194942, 199038, 203134, 204158, 57728, 58752, 59776, 63872, 67968, 68992, 81280, 82304, 86400, 90496, 91520, 92544, 170368, 171392, 172416, 176512, 180608, 181632, 193920, 194944, 199040, 203136, 204160, 205184, 25986, 27010, 28034, 29058, 30082, 31106, 32130, 33154, 34178, 35202, 36226, 37250, 38274, 39298, 40322, 41346, 42370, 43394, 44418, 45442, 46466, 47490, 48514, 49538, 50562, 51586, 52610, 53634, 54658, 55682, 56706, 57730, 58754, 59778, 63874, 67970, 68994, 81282, 82306, 86402, 90498, 91522, 92546, 93570, 94594, 95618, 96642, 97666, 98690, 99714, 100738, 101762, 102786, 103810, 104834, 105858, 106882, 107906, 108930, 109954, 110978, 129410, 130434, 131458, 132482, 133506, 151938, 152962, 153986, 155010, 156034, 157058, 158082, 159106, 160130, 161154, 162178, 163202, 164226, 165250, 166274, 167298, 168322, 169346, 170370, 171394, 172418, 176514, 180610, 181634, 193922, 194946, 199042, 203138, 204162, 205186, 206210, 207234, 208258, 209282, 210306, 211330, 212354, 213378, 214402, 215426, 216450, 217474, 218498, 219522, 220546, 221570, 222594, 223618, 224642, 225666, 226690, 227714, 228738, 229762, 230786, 231810, 232834, 233858, 234882, 235906, 236930, 24964, 25988, 27012, 28036, 29060, 30084, 31108, 32132, 33156, 34180, 35204, 36228, 37252, 38276, 39300, 40324, 41348, 42372, 43396, 44420, 45444, 46468, 47492, 48516, 49540, 50564, 51588, 52612, 53636, 54660, 55684, 56708, 57732, 58756, 63876, 67972, 68996, 81284, 82308, 86404, 91524, 92548, 93572, 94596, 95620, 96644, 97668, 98692, 99716, 100740, 101764, 102788, 103812, 104836, 105860, 106884, 107908, 108932, 109956, 110980, 112004, 113028, 127364, 128388, 129412, 130436, 131460, 132484, 133508, 134532, 135556, 149892, 150916, 151940, 152964, 153988, 155012, 156036, 157060, 158084, 159108, 160132, 161156, 162180, 163204, 164228, 165252, 166276, 167300, 168324, 169348, 170372, 171396, 176516, 180612, 181636, 193924, 194948, 199044, 204164, 205188, 206212, 207236, 208260, 209284, 210308, 211332, 212356, 213380, 214404, 215428, 216452, 217476, 218500, 219524, 220548, 221572, 222596, 223620, 224644, 225668, 226692, 227716, 228740, 229764, 230788, 231812, 232836, 233860, 234884, 235908, 236932, 237956, 24966, 25990, 27014, 63878, 67974, 68998, 81286, 82310, 86406, 110982, 112006, 113030, 127366, 128390, 134534, 135558, 149894, 150918, 151942, 176518, 180614, 181638, 193926, 194950, 199046, 235910, 236934, 237958, 24968, 25992, 63880, 67976, 69000, 81288, 82312, 86408, 113032, 114056, 126344, 127368, 128392, 134536, 135560, 136584, 148872, 149896, 176520, 180616, 181640, 193928, 194952, 199048, 236936, 237960, 24970, 25994, 63882, 67978, 69002, 81290, 82314, 86410, 113034, 114058, 126346, 127370, 135562, 136586, 148874, 149898, 176522, 180618, 181642, 193930, 194954, 199050, 236938, 237962, 24972, 25996, 30092, 31116, 32140, 33164, 34188, 35212, 36236, 37260, 38284, 39308, 40332, 41356, 42380, 43404, 44428, 45452, 46476, 47500, 48524, 49548, 50572, 51596, 52620, 53644, 54668, 55692, 56716, 57740, 58764, 59788, 60812, 61836, 62860, 63884, 67980, 69004, 81292, 82316, 86412, 87436, 88460, 89484, 90508, 91532, 92556, 93580, 94604, 95628, 96652, 97676, 98700, 99724, 100748, 101772, 102796, 103820, 104844, 105868, 106892, 107916, 108940, 113036, 114060, 126348, 127372, 131468, 135564, 136588, 148876, 149900, 153996, 155020, 156044, 157068, 158092, 159116, 160140, 161164, 162188, 163212, 164236, 165260, 166284, 167308, 168332, 169356, 170380, 171404, 172428, 173452, 174476, 175500, 176524, 180620, 181644, 193932, 194956, 199052, 200076, 201100, 202124, 203148, 204172, 205196, 206220, 207244, 208268, 209292, 210316, 211340, 212364, 213388, 214412, 215436, 216460, 217484, 218508, 219532, 220556, 221580, 222604, 223628, 224652, 225676, 226700, 227724, 228748, 229772, 230796, 231820, 232844, 236940, 237964, 24974, 25998, 30094, 67982, 69006, 81294, 82318, 86414, 113038, 114062, 126350, 127374, 131470, 135566, 136590, 148878, 149902, 176526, 180622, 181646, 193934, 194958, 232846, 236942, 237966, 24976, 26000, 30096, 67984, 69008, 81296, 82320, 86416, 113040, 114064, 126352, 127376, 131472, 135568, 136592, 148880, 149904, 176528, 180624, 181648, 193936, 194960, 232848, 236944, 237968, 24978, 26002, 30098, 66962, 67986, 69010, 81298, 82322, 86418, 110994, 112018, 113042, 126354, 127378, 131474, 135570, 136594, 149906, 150930, 151954, 176530, 180626, 181650, 193938, 194962, 195986, 232850, 236946, 237970, 24980, 26004, 30100, 35220, 36244, 37268, 38292, 39316, 40340, 41364, 42388, 43412, 44436, 45460, 46484, 47508, 48532, 49556, 50580, 51604, 52628, 53652, 54676, 55700, 56724, 57748, 58772, 59796, 60820, 61844, 62868, 63892, 64916, 65940, 66964, 67988, 69012, 81300, 82324, 86420, 91540, 92564, 93588, 94612, 95636, 96660, 97684, 98708, 99732, 100756, 101780, 102804, 103828, 104852, 105876, 106900, 107924, 108948, 109972, 110996, 112020, 113044, 126356, 127380, 131476, 135572, 136596, 149908, 150932, 151956, 152980, 154004, 155028, 156052, 157076, 158100, 159124, 160148, 161172, 162196, 163220, 164244, 165268, 166292, 167316, 168340, 169364, 170388, 171412, 176532, 180628, 181652, 193940, 194964, 195988, 197012, 198036, 199060, 200084, 201108, 202132, 203156, 204180, 205204, 206228, 207252, 208276, 209300, 210324, 211348, 212372, 213396, 214420, 215444, 216468, 217492, 218516, 219540, 220564, 221588, 222612, 223636, 224660, 225684, 226708, 227732, 232852, 236948, 237972, 24982, 26006, 30102, 34198, 35222, 36246, 37270, 38294, 39318, 40342, 41366, 42390, 43414, 44438, 45462, 46486, 47510, 48534, 49558, 50582, 51606, 52630, 53654, 54678, 55702, 56726, 57750, 58774, 59798, 60822, 61846, 62870, 63894, 64918, 65942, 66966, 67990, 81302, 82326, 86422, 90518, 91542, 92566, 93590, 94614, 95638, 96662, 97686, 98710, 99734, 100758, 101782, 102806, 103830, 104854, 105878, 106902, 107926, 108950, 109974, 110998, 126358, 127382, 131478, 135574, 136598, 151958, 152982, 154006, 155030, 156054, 157078, 158102, 159126, 160150, 161174, 162198, 163222, 164246, 165270, 166294, 167318, 168342, 169366, 170390, 171414, 172438, 176534, 180630, 181654, 194966, 195990, 197014, 198038, 199062, 200086, 201110, 202134, 203158, 204182, 205206, 206230, 207254, 208278, 209302, 210326, 211350, 212374, 213398, 214422, 215446, 216470, 217494, 218518, 219542, 220566, 221590, 222614, 223638, 224662, 225686, 226710, 227734, 228758, 232854, 236950, 237974, 24984, 26008, 30104, 34200, 35224, 36248, 81304, 82328, 86424, 90520, 91544, 92568, 126360, 127384, 131480, 135576, 136600, 170392, 171416, 172440, 176536, 180632, 181656, 226712, 227736, 228760, 232856, 236952, 237976, 24986, 26010, 30106, 34202, 35226, 81306, 82330, 86426, 90522, 91546, 126362, 127386, 131482, 135578, 136602, 171418, 172442, 176538, 180634, 181658, 227738, 228762, 232858, 236954, 237978, 24988, 26012, 30108, 34204, 35228, 81308, 82332, 86428, 90524, 91548, 126364, 127388, 131484, 135580, 136604, 171420, 172444, 176540, 180636, 181660, 227740, 228764, 232860, 236956, 237980, 24990, 26014, 30110, 34206, 35230, 81310, 82334, 86430, 90526, 91550, 126366, 127390, 131486, 135582, 136606, 171422, 172446, 176542, 180638, 181662, 227742, 228766, 232862, 236958, 237982, 24992, 26016, 30112, 34208, 35232, 81312, 82336, 86432, 90528, 91552, 126368, 127392, 131488, 135584, 136608, 171424, 172448, 176544, 180640, 181664, 227744, 228768, 232864, 236960, 237984, 24994, 26018, 30114, 34210, 35234, 81314, 82338, 86434, 90530, 91554, 126370, 127394, 131490, 135586, 136610, 171426, 172450, 176546, 180642, 181666, 227746, 228770, 232866, 236962, 237986, 24996, 26020, 30116, 34212, 35236, 81316, 82340, 86436, 90532, 91556, 126372, 127396, 131492, 135588, 136612, 171428, 172452, 176548, 180644, 181668, 227748, 228772, 232868, 236964, 237988, 24998, 26022, 30118, 34214, 35238, 81318, 82342, 86438, 90534, 91558, 126374, 127398, 131494, 135590, 136614, 171430, 172454, 176550, 180646, 181670, 227750, 228774, 232870, 236966, 237990, 25000, 26024, 30120, 34216, 35240, 81320, 82344, 86440, 90536, 91560, 126376, 127400, 131496, 135592, 136616, 171432, 172456, 176552, 180648, 181672, 227752, 228776, 232872, 236968, 237992, 25002, 26026, 30122, 34218, 35242, 81322, 82346, 86442, 90538, 91562, 126378, 127402, 131498, 135594, 136618, 171434, 172458, 176554, 180650, 181674, 227754, 228778, 232874, 236970, 237994, 25004, 26028, 30124, 34220, 35244, 81324, 82348, 86444, 90540, 91564, 125356, 126380, 127404, 131500, 135596, 136620, 137644, 171436, 172460, 176556, 180652, 181676, 227756, 228780, 232876, 236972, 237996, 25006, 26030, 30126, 34222, 35246, 50606, 51630, 52654, 53678, 54702, 55726, 56750, 57774, 58798, 59822, 60846, 61870, 62894, 63918, 64942, 65966, 81326, 82350, 86446, 90542, 91566, 106926, 107950, 108974, 109998, 111022, 112046, 113070, 114094, 115118, 116142, 117166, 118190, 119214, 120238, 121262, 122286, 123310, 124334, 125358, 126382, 127406, 131502, 135598, 136622, 137646, 138670, 139694, 140718, 141742, 142766, 143790, 144814, 145838, 146862, 147886, 148910, 149934, 150958, 151982, 153006, 154030, 155054, 156078, 171438, 172462, 176558, 180654, 181678, 197038, 198062, 199086, 200110, 201134, 202158, 203182, 204206, 205230, 206254, 207278, 208302, 209326, 210350, 211374, 212398, 227758, 228782, 232878, 236974, 237998, 25008, 26032, 30128, 34224, 35248, 48560, 49584, 50608, 51632, 52656, 53680, 54704, 55728, 56752, 57776, 58800, 59824, 60848, 61872, 62896, 63920, 64944, 65968, 66992, 68016, 81328, 82352, 86448, 90544, 91568, 104880, 105904, 106928, 107952, 108976, 110000, 111024, 112048, 113072, 114096, 115120, 116144, 117168, 118192, 119216, 120240, 121264, 122288, 123312, 124336, 125360, 126384, 131504, 136624, 137648, 138672, 139696, 140720, 141744, 142768, 143792, 144816, 145840, 146864, 147888, 148912, 149936, 150960, 151984, 153008, 154032, 155056, 156080, 157104, 158128, 171440, 172464, 176560, 180656, 181680, 194992, 196016, 197040, 198064, 199088, 200112, 201136, 202160, 203184, 204208, 205232, 206256, 207280, 208304, 209328, 210352, 211376, 212400, 213424, 214448, 227760, 228784, 232880, 236976, 238000, 25010, 26034, 30130, 34226, 35250, 48562, 49586, 50610, 65970, 66994, 68018, 81330, 82354, 86450, 90546, 91570, 104882, 105906, 106930, 131506, 156082, 157106, 158130, 171442, 172466, 176562, 180658, 181682, 194994, 196018, 197042, 212402, 213426, 214450, 227762, 228786, 232882, 236978, 238002, 25012, 26036, 30132, 34228, 35252, 47540, 48564, 68020, 69044, 81332, 82356, 86452, 90548, 91572, 103860, 104884, 131508, 158132, 159156, 171444, 172468, 176564, 180660, 181684, 193972, 194996, 214452, 215476, 227764, 228788, 232884, 236980, 238004, 25014, 26038, 30134, 34230, 35254, 47542, 48566, 68022, 69046, 81334, 82358, 86454, 90550, 91574, 103862, 104886, 131510, 158134, 159158, 171446, 172470, 176566, 180662, 181686, 193974, 194998, 214454, 215478, 227766, 228790, 232886, 236982, 238006, 25016, 26040, 30136, 34232, 35256, 47544, 48568, 52664, 53688, 54712, 55736, 56760, 57784, 58808, 59832, 60856, 61880, 62904, 63928, 68024, 69048, 81336, 82360, 86456, 90552, 91576, 103864, 104888, 108984, 110008, 111032, 112056, 113080, 114104, 115128, 116152, 117176, 118200, 119224, 120248, 121272, 122296, 123320, 124344, 125368, 126392, 127416, 128440, 129464, 130488, 131512, 132536, 133560, 134584, 135608, 136632, 137656, 138680, 139704, 140728, 141752, 142776, 143800, 144824, 145848, 146872, 147896, 148920, 149944, 150968, 151992, 153016, 154040, 158136, 159160, 171448, 172472, 176568, 180664, 181688, 193976, 195000, 199096, 200120, 201144, 202168, 203192, 204216, 205240, 206264, 207288, 208312, 209336, 210360, 214456, 215480, 227768, 228792, 232888, 236984, 238008, 25018, 26042, 30138, 34234, 35258, 47546, 48570, 68026, 69050, 81338, 82362, 90554, 91578, 103866, 104890, 158138, 159162, 171450, 172474, 180666, 181690, 193978, 195002, 214458, 215482, 227770, 228794, 232890, 236986, 238010, 25020, 26044, 30140, 34236, 35260, 47548, 48572, 68028, 69052, 81340, 82364, 83388, 89532, 90556, 91580, 103868, 104892, 158140, 159164, 171452, 172476, 173500, 179644, 180668, 181692, 193980, 195004, 214460, 215484, 227772, 228796, 232892, 236988, 238012, 25022, 26046, 30142, 34238, 35262, 48574, 49598, 50622, 65982, 67006, 68030, 82366, 83390, 89534, 90558, 104894, 105918, 106942, 156094, 157118, 158142, 172478, 173502, 179646, 180670, 195006, 196030, 197054, 212414, 213438, 214462, 227774, 228798, 232894, 236990, 238014, 25024, 26048, 30144, 34240, 35264, 48576, 49600, 50624, 51648, 52672, 53696, 54720, 55744, 56768, 57792, 58816, 59840, 60864, 61888, 62912, 63936, 64960, 65984, 67008, 68032, 82368, 83392, 84416, 85440, 86464, 87488, 88512, 89536, 90560, 104896, 105920, 106944, 107968, 108992, 110016, 111040, 112064, 113088, 114112, 115136, 116160, 117184, 118208, 119232, 120256, 121280, 122304, 123328, 124352, 125376, 126400, 127424, 128448, 129472, 130496, 131520, 132544, 133568, 134592, 135616, 136640, 137664, 138688, 139712, 140736, 141760, 142784, 143808, 144832, 145856, 146880, 147904, 148928, 149952, 150976, 152000, 153024, 154048, 155072, 156096, 157120, 158144, 172480, 173504, 174528, 175552, 176576, 177600, 178624, 179648, 180672, 195008, 196032, 197056, 198080, 199104, 200128, 201152, 202176, 203200, 204224, 205248, 206272, 207296, 208320, 209344, 210368, 211392, 212416, 213440, 214464, 227776, 228800, 232896, 236992, 238016, 25026, 26050, 30146, 34242, 35266, 50626, 51650, 52674, 53698, 54722, 55746, 56770, 57794, 58818, 59842, 60866, 61890, 62914, 63938, 64962, 65986, 84418, 85442, 86466, 87490, 88514, 106946, 107970, 108994, 110018, 111042, 112066, 113090, 114114, 115138, 116162, 117186, 118210, 119234, 120258, 121282, 122306, 123330, 124354, 125378, 126402, 127426, 128450, 129474, 130498, 131522, 132546, 133570, 134594, 135618, 136642, 137666, 138690, 139714, 140738, 141762, 142786, 143810, 144834, 145858, 146882, 147906, 148930, 149954, 150978, 152002, 153026, 154050, 155074, 156098, 174530, 175554, 176578, 177602, 178626, 197058, 198082, 199106, 200130, 201154, 202178, 203202, 204226, 205250, 206274, 207298, 208322, 209346, 210370, 211394, 212418, 227778, 228802, 232898, 236994, 238018, 25028, 26052, 30148, 34244, 35268, 227780, 228804, 232900, 236996, 238020, 25030, 26054, 30150, 34246, 35270, 227782, 228806, 232902, 236998, 238022, 25032, 26056, 30152, 34248, 35272, 227784, 228808, 232904, 237000, 238024, 25034, 26058, 30154, 34250, 35274, 227786, 228810, 232906, 237002, 238026, 25036, 26060, 30156, 34252, 35276, 227788, 228812, 232908, 237004, 238028, 25038, 26062, 30158, 34254, 35278, 227790, 228814, 232910, 237006, 238030, 25040, 26064, 30160, 34256, 35280, 227792, 228816, 232912, 237008, 238032, 25042, 26066, 30162, 34258, 35282, 227794, 228818, 232914, 237010, 238034, 25044, 26068, 30164, 34260, 35284, 227796, 228820, 232916, 237012, 238036, 25046, 26070, 30166, 34262, 35286, 227798, 228822, 232918, 237014, 238038, 25048, 26072, 30168, 34264, 35288, 227800, 228824, 232920, 237016, 238040, 25050, 26074, 30170, 34266, 35290, 50650, 51674, 52698, 53722, 54746, 55770, 56794, 57818, 58842, 59866, 60890, 61914, 62938, 63962, 64986, 66010, 84442, 85466, 86490, 87514, 88538, 89562, 90586, 91610, 92634, 93658, 94682, 95706, 96730, 97754, 98778, 99802, 100826, 101850, 102874, 103898, 104922, 105946, 106970, 107994, 109018, 110042, 111066, 129498, 130522, 131546, 132570, 133594, 152026, 153050, 154074, 155098, 156122, 157146, 158170, 159194, 160218, 161242, 162266, 163290, 164314, 165338, 166362, 167386, 168410, 169434, 170458, 171482, 172506, 173530, 174554, 175578, 176602, 177626, 178650, 197082, 198106, 199130, 200154, 201178, 202202, 203226, 204250, 205274, 206298, 207322, 208346, 209370, 210394, 211418, 212442, 227802, 228826, 232922, 237018, 238042, 25052, 26076, 30172, 34268, 35292, 48604, 49628, 50652, 51676, 52700, 53724, 54748, 55772, 56796, 57820, 58844, 59868, 60892, 61916, 62940, 63964, 64988, 66012, 67036, 68060, 82396, 83420, 84444, 85468, 86492, 87516, 88540, 89564, 90588, 91612, 92636, 93660, 94684, 95708, 96732, 97756, 98780, 99804, 100828, 101852, 102876, 103900, 104924, 105948, 106972, 107996, 109020, 110044, 111068, 112092, 113116, 127452, 128476, 129500, 130524, 131548, 132572, 133596, 134620, 135644, 149980, 151004, 152028, 153052, 154076, 155100, 156124, 157148, 158172, 159196, 160220, 161244, 162268, 163292, 164316, 165340, 166364, 167388, 168412, 169436, 170460, 171484, 172508, 173532, 174556, 175580, 176604, 177628, 178652, 179676, 180700, 195036, 196060, 197084, 198108, 199132, 200156, 201180, 202204, 203228, 204252, 205276, 206300, 207324, 208348, 209372, 210396, 211420, 212444, 213468, 214492, 227804, 228828, 232924, 237020, 238044, 25054, 26078, 30174, 34270, 35294, 48606, 49630, 50654, 66014, 67038, 68062, 82398, 83422, 84446, 111070, 112094, 113118, 127454, 128478, 134622, 135646, 149982, 151006, 152030, 178654, 179678, 180702, 195038, 196062, 197086, 212446, 213470, 214494, 227806, 228830, 232926, 237022, 238046, 25056, 26080, 30176, 34272, 35296, 47584, 48608, 68064, 69088, 81376, 82400, 113120, 114144, 126432, 127456, 128480, 134624, 135648, 136672, 148960, 149984, 180704, 181728, 194016, 195040, 214496, 215520, 227808, 228832, 232928, 237024, 238048, 25058, 26082, 30178, 34274, 35298, 47586, 48610, 68066, 69090, 81378, 82402, 113122, 114146, 126434, 127458, 135650, 136674, 148962, 149986, 180706, 181730, 194018, 195042, 214498, 215522, 227810, 228834, 232930, 237026, 238050, 25060, 26084, 30180, 34276, 35300, 47588, 48612, 52708, 53732, 54756, 55780, 56804, 57828, 58852, 59876, 60900, 61924, 62948, 63972, 68068, 69092, 81380, 82404, 86500, 87524, 88548, 89572, 90596, 91620, 92644, 93668, 94692, 95716, 96740, 97764, 98788, 99812, 100836, 101860, 102884, 103908, 104932, 105956, 106980, 108004, 109028, 113124, 114148, 126436, 127460, 131556, 135652, 136676, 148964, 149988, 154084, 155108, 156132, 157156, 158180, 159204, 160228, 161252, 162276, 163300, 164324, 165348, 166372, 167396, 168420, 169444, 170468, 171492, 172516, 173540, 174564, 175588, 176612, 180708, 181732, 194020, 195044, 199140, 200164, 201188, 202212, 203236, 204260, 205284, 206308, 207332, 208356, 209380, 210404, 214500, 215524, 227812, 228836, 232932, 237028, 238052, 25062, 26086, 30182, 34278, 35302, 47590, 48614, 52710, 63974, 68070, 69094, 81382, 82406, 86502, 109030, 113126, 114150, 126438, 127462, 131558, 135654, 136678, 148966, 149990, 154086, 176614, 180710, 181734, 194022, 195046, 199142, 210406, 214502, 215526, 227814, 228838, 232934, 237030, 238054, 25064, 26088, 30184, 34280, 35304, 47592, 48616, 52712, 63976, 68072, 69096, 81384, 82408, 86504, 109032, 113128, 114152, 126440, 127464, 131560, 135656, 136680, 148968, 149992, 154088, 176616, 180712, 181736, 194024, 195048, 199144, 210408, 214504, 215528, 227816, 228840, 232936, 237032, 238056, 25066, 26090, 30186, 34282, 35306, 47594, 48618, 52714, 63978, 68074, 69098, 81386, 82410, 86506, 109034, 113130, 114154, 126442, 127466, 131562, 135658, 136682, 148970, 149994, 154090, 176618, 180714, 181738, 194026, 195050, 199146, 210410, 214506, 215530, 227818, 228842, 232938, 237034, 238058, 25068, 26092, 30188, 34284, 35308, 47596, 48620, 52716, 63980, 68076, 69100, 81388, 82412, 86508, 109036, 113132, 114156, 126444, 127468, 131564, 135660, 136684, 148972, 149996, 154092, 176620, 180716, 181740, 194028, 195052, 199148, 210412, 214508, 215532, 227820, 228844, 232940, 237036, 238060, 25070, 26094, 30190, 34286, 35310, 47598, 48622, 52718, 63982, 68078, 69102, 81390, 82414, 86510, 109038, 113134, 114158, 126446, 127470, 131566, 135662, 136686, 148974, 149998, 154094, 176622, 180718, 181742, 194030, 195054, 199150, 210414, 214510, 215534, 227822, 228846, 232942, 237038, 238062, 25072, 26096, 30192, 34288, 35312, 47600, 48624, 52720, 63984, 68080, 69104, 81392, 82416, 86512, 109040, 113136, 114160, 126448, 127472, 131568, 135664, 136688, 148976, 150000, 154096, 176624, 180720, 181744, 194032, 195056, 199152, 210416, 214512, 215536, 227824, 228848, 232944, 237040, 238064, 25074, 26098, 30194, 34290, 35314, 47602, 48626, 52722, 63986, 68082, 69106, 81394, 82418, 86514, 109042, 113138, 114162, 126450, 127474, 131570, 135666, 136690, 148978, 150002, 154098, 176626, 180722, 181746, 194034, 195058, 199154, 210418, 214514, 215538, 227826, 228850, 232946, 237042, 238066, 25076, 26100, 30196, 34292, 35316, 47604, 48628, 52724, 63988, 68084, 69108, 81396, 82420, 86516, 109044, 113140, 114164, 126452, 127476, 131572, 135668, 136692, 148980, 150004, 154100, 176628, 180724, 181748, 194036, 195060, 199156, 210420, 214516, 215540, 227828, 228852, 232948, 237044, 238068, 25078, 26102, 30198, 34294, 35318, 47606, 48630, 52726, 63990, 68086, 69110, 81398, 82422, 86518, 109046, 113142, 114166, 126454, 127478, 131574, 135670, 136694, 148982, 150006, 154102, 176630, 180726, 181750, 194038, 195062, 199158, 210422, 214518, 215542, 227830, 228854, 232950, 237046, 238070, 25080, 26104, 30200, 34296, 35320, 47608, 48632, 52728, 63992, 68088, 69112, 81400, 82424, 86520, 109048, 113144, 114168, 126456, 127480, 131576, 135672, 136696, 148984, 150008, 154104, 176632, 180728, 181752, 194040, 195064, 199160, 210424, 214520, 215544, 227832, 228856, 232952, 237048, 238072, 25082, 26106, 30202, 34298, 35322, 47610, 48634, 52730, 53754, 54778, 55802, 56826, 57850, 58874, 59898, 60922, 61946, 62970, 63994, 68090, 69114, 81402, 82426, 86522, 87546, 88570, 89594, 90618, 91642, 92666, 93690, 94714, 95738, 96762, 97786, 98810, 99834, 100858, 101882, 102906, 103930, 104954, 105978, 107002, 108026, 109050, 113146, 114170, 126458, 127482, 131578, 135674, 136698, 148986, 150010, 154106, 155130, 156154, 157178, 158202, 159226, 160250, 161274, 162298, 163322, 164346, 165370, 166394, 167418, 168442, 169466, 170490, 171514, 172538, 173562, 174586, 175610, 176634, 180730, 181754, 194042, 195066, 199162, 200186, 201210, 202234, 203258, 204282, 205306, 206330, 207354, 208378, 209402, 210426, 214522, 215546, 227834, 228858, 232954, 237050, 238074, 25084, 26108, 30204, 34300, 35324, 47612, 48636, 68092, 69116, 81404, 82428, 113148, 114172, 126460, 127484, 131580, 135676, 136700, 148988, 150012, 180732, 181756, 194044, 195068, 214524, 215548, 227836, 228860, 232956, 237052, 238076, 25086, 26110, 30206, 34302, 35326, 47614, 48638, 68094, 69118, 81406, 82430, 113150, 114174, 126462, 127486, 131582, 135678, 136702, 148990, 150014, 180734, 181758, 194046, 195070, 214526, 215550, 227838, 228862, 232958, 237054, 238078, 25088, 26112, 30208, 34304, 35328, 48640, 49664, 50688, 66048, 67072, 68096, 82432, 83456, 84480, 111104, 112128, 113152, 126464, 127488, 131584, 135680, 136704, 150016, 151040, 152064, 178688, 179712, 180736, 195072, 196096, 197120, 212480, 213504, 214528, 227840, 228864, 232960, 237056, 238080, 25090, 26114, 30210, 34306, 35330, 48642, 49666, 50690, 51714, 52738, 53762, 54786, 55810, 56834, 57858, 58882, 59906, 60930, 61954, 62978, 64002, 65026, 66050, 67074, 68098, 82434, 83458, 84482, 85506, 86530, 87554, 88578, 89602, 90626, 91650, 92674, 93698, 94722, 95746, 96770, 97794, 98818, 99842, 100866, 101890, 102914, 103938, 104962, 105986, 107010, 108034, 109058, 110082, 111106, 112130, 113154, 126466, 127490, 131586, 135682, 136706, 150018, 151042, 152066, 153090, 154114, 155138, 156162, 157186, 158210, 159234, 160258, 161282, 162306, 163330, 164354, 165378, 166402, 167426, 168450, 169474, 170498, 171522, 172546, 173570, 174594, 175618, 176642, 177666, 178690, 179714, 180738, 195074, 196098, 197122, 198146, 199170, 200194, 201218, 202242, 203266, 204290, 205314, 206338, 207362, 208386, 209410, 210434, 211458, 212482, 213506, 214530, 227842, 228866, 232962, 237058, 238082, 25092, 26116, 30212, 34308, 35332, 50692, 51716, 52740, 53764, 54788, 55812, 56836, 57860, 58884, 59908, 60932, 61956, 62980, 64004, 65028, 66052, 84484, 85508, 86532, 87556, 88580, 89604, 90628, 91652, 92676, 93700, 94724, 95748, 96772, 97796, 98820, 99844, 100868, 101892, 102916, 103940, 104964, 105988, 107012, 108036, 109060, 110084, 111108, 126468, 127492, 131588, 135684, 136708, 152068, 153092, 154116, 155140, 156164, 157188, 158212, 159236, 160260, 161284, 162308, 163332, 164356, 165380, 166404, 167428, 168452, 169476, 170500, 171524, 172548, 173572, 174596, 175620, 176644, 177668, 178692, 197124, 198148, 199172, 200196, 201220, 202244, 203268, 204292, 205316, 206340, 207364, 208388, 209412, 210436, 211460, 212484, 227844, 228868, 232964, 237060, 238084, 25094, 26118, 30214, 34310, 35334, 126470, 127494, 131590, 135686, 136710, 227846, 228870, 232966, 237062, 238086, 25096, 26120, 30216, 34312, 35336, 126472, 127496, 131592, 135688, 136712, 227848, 228872, 232968, 237064, 238088, 25098, 26122, 30218, 34314, 35338, 126474, 127498, 131594, 135690, 136714, 227850, 228874, 232970, 237066, 238090, 25100, 26124, 30220, 34316, 35340, 126476, 127500, 131596, 135692, 136716, 227852, 228876, 232972, 237068, 238092, 25102, 26126, 30222, 34318, 35342, 126478, 127502, 131598, 135694, 136718, 227854, 228878, 232974, 237070, 238094, 25104, 26128, 30224, 34320, 35344, 126480, 127504, 131600, 135696, 136720, 227856, 228880, 232976, 237072, 238096, 25106, 26130, 30226, 34322, 35346, 126482, 127506, 131602, 135698, 136722, 227858, 228882, 232978, 237074, 238098, 25108, 26132, 30228, 34324, 35348, 126484, 127508, 131604, 135700, 136724, 227860, 228884, 232980, 237076, 238100, 25110, 26134, 30230, 34326, 35350, 126486, 127510, 131606, 135702, 136726, 227862, 228886, 232982, 237078, 238102, 25112, 26136, 30232, 34328, 35352, 126488, 127512, 131608, 135704, 136728, 227864, 228888, 232984, 237080, 238104, 25114, 26138, 30234, 34330, 35354, 36378, 125466, 126490, 127514, 131610, 135706, 136730, 137754, 226842, 227866, 228890, 232986, 237082, 238106, 25116, 26140, 30236, 34332, 35356, 36380, 37404, 38428, 39452, 40476, 41500, 42524, 43548, 44572, 45596, 46620, 47644, 48668, 49692, 50716, 51740, 52764, 53788, 54812, 55836, 56860, 57884, 58908, 59932, 60956, 61980, 63004, 64028, 65052, 66076, 67100, 68124, 69148, 70172, 71196, 72220, 73244, 74268, 75292, 76316, 77340, 78364, 79388, 80412, 81436, 82460, 83484, 84508, 85532, 86556, 87580, 88604, 89628, 90652, 91676, 92700, 93724, 94748, 95772, 96796, 97820, 98844, 99868, 100892, 101916, 102940, 103964, 104988, 106012, 107036, 108060, 109084, 110108, 111132, 112156, 113180, 114204, 115228, 116252, 117276, 118300, 119324, 120348, 121372, 122396, 123420, 124444, 125468, 126492, 127516, 131612, 135708, 136732, 137756, 138780, 139804, 140828, 141852, 142876, 143900, 144924, 145948, 146972, 147996, 149020, 150044, 151068, 152092, 153116, 154140, 155164, 156188, 157212, 158236, 159260, 160284, 161308, 162332, 163356, 164380, 165404, 166428, 167452, 168476, 169500, 170524, 171548, 172572, 173596, 174620, 175644, 176668, 177692, 178716, 179740, 180764, 181788, 182812, 183836, 184860, 185884, 186908, 187932, 188956, 189980, 191004, 192028, 193052, 194076, 195100, 196124, 197148, 198172, 199196, 200220, 201244, 202268, 203292, 204316, 205340, 206364, 207388, 208412, 209436, 210460, 211484, 212508, 213532, 214556, 215580, 216604, 217628, 218652, 219676, 220700, 221724, 222748, 223772, 224796, 225820, 226844, 227868, 228892, 232988, 237084, 238108, 25118, 26142, 30238, 35358, 36382, 37406, 38430, 39454, 40478, 41502, 42526, 43550, 44574, 45598, 46622, 47646, 48670, 49694, 50718, 51742, 52766, 53790, 54814, 55838, 56862, 57886, 58910, 59934, 60958, 61982, 63006, 64030, 65054, 66078, 67102, 68126, 69150, 70174, 71198, 72222, 73246, 74270, 75294, 76318, 77342, 78366, 79390, 80414, 81438, 82462, 83486, 84510, 85534, 86558, 87582, 88606, 89630, 90654, 91678, 92702, 93726, 94750, 95774, 96798, 97822, 98846, 99870, 100894, 101918, 102942, 103966, 104990, 106014, 107038, 108062, 109086, 110110, 111134, 112158, 113182, 114206, 115230, 116254, 117278, 118302, 119326, 120350, 121374, 122398, 123422, 124446, 125470, 126494, 131614, 136734, 137758, 138782, 139806, 140830, 141854, 142878, 143902, 144926, 145950, 146974, 147998, 149022, 150046, 151070, 152094, 153118, 154142, 155166, 156190, 157214, 158238, 159262, 160286, 161310, 162334, 163358, 164382, 165406, 166430, 167454, 168478, 169502, 170526, 171550, 172574, 173598, 174622, 175646, 176670, 177694, 178718, 179742, 180766, 181790, 182814, 183838, 184862, 185886, 186910, 187934, 188958, 189982, 191006, 192030, 193054, 194078, 195102, 196126, 197150, 198174, 199198, 200222, 201246, 202270, 203294, 204318, 205342, 206366, 207390, 208414, 209438, 210462, 211486, 212510, 213534, 214558, 215582, 216606, 217630, 218654, 219678, 220702, 221726, 222750, 223774, 224798, 225822, 226846, 227870, 232990, 237086, 238110, 25120, 26144, 30240, 131616, 232992, 237088, 238112, 25122, 26146, 30242, 131618, 232994, 237090, 238114, 25124, 26148, 30244, 131620, 232996, 237092, 238116, 25126, 26150, 30246, 31270, 32294, 33318, 34342, 35366, 36390, 37414, 38438, 39462, 40486, 41510, 42534, 43558, 44582, 45606, 46630, 47654, 48678, 49702, 50726, 51750, 52774, 53798, 54822, 55846, 56870, 57894, 58918, 59942, 60966, 61990, 63014, 64038, 65062, 66086, 67110, 68134, 69158, 70182, 71206, 72230, 73254, 74278, 75302, 76326, 77350, 78374, 79398, 80422, 81446, 82470, 83494, 84518, 85542, 86566, 87590, 88614, 89638, 90662, 91686, 92710, 93734, 94758, 95782, 96806, 97830, 98854, 99878, 100902, 101926, 102950, 103974, 104998, 106022, 107046, 108070, 109094, 110118, 111142, 112166, 113190, 114214, 115238, 116262, 117286, 118310, 119334, 120358, 121382, 122406, 123430, 124454, 125478, 126502, 127526, 128550, 129574, 130598, 131622, 132646, 133670, 134694, 135718, 136742, 137766, 138790, 139814, 140838, 141862, 142886, 143910, 144934, 145958, 146982, 148006, 149030, 150054, 151078, 152102, 153126, 154150, 155174, 156198, 157222, 158246, 159270, 160294, 161318, 162342, 163366, 164390, 165414, 166438, 167462, 168486, 169510, 170534, 171558, 172582, 173606, 174630, 175654, 176678, 177702, 178726, 179750, 180774, 181798, 182822, 183846, 184870, 185894, 186918, 187942, 188966, 189990, 191014, 192038, 193062, 194086, 195110, 196134, 197158, 198182, 199206, 200230, 201254, 202278, 203302, 204326, 205350, 206374, 207398, 208422, 209446, 210470, 211494, 212518, 213542, 214566, 215590, 216614, 217638, 218662, 219686, 220710, 221734, 222758, 223782, 224806, 225830, 226854, 227878, 228902, 229926, 230950, 231974, 232998, 237094, 238118, 25128, 26152, 237096, 238120, 25130, 26154, 237098, 238122, 25132, 26156, 27180, 236076, 237100, 238124, 25134, 26158, 27182, 28206, 29230, 30254, 31278, 32302, 33326, 34350, 35374, 36398, 37422, 38446, 39470, 40494, 41518, 42542, 43566, 44590, 45614, 46638, 47662, 48686, 49710, 50734, 51758, 52782, 53806, 54830, 55854, 56878, 57902, 58926, 59950, 60974, 61998, 63022, 64046, 65070, 66094, 67118, 68142, 69166, 70190, 71214, 72238, 73262, 74286, 75310, 76334, 77358, 78382, 79406, 80430, 81454, 82478, 83502, 84526, 85550, 86574, 87598, 88622, 89646, 90670, 91694, 92718, 93742, 94766, 95790, 96814, 97838, 98862, 99886, 100910, 101934, 102958, 103982, 105006, 106030, 107054, 108078, 109102, 110126, 111150, 112174, 113198, 114222, 115246, 116270, 117294, 118318, 119342, 120366, 121390, 122414, 123438, 124462, 125486, 126510, 127534, 128558, 129582, 130606, 131630, 132654, 133678, 134702, 135726, 136750, 137774, 138798, 139822, 140846, 141870, 142894, 143918, 144942, 145966, 146990, 148014, 149038, 150062, 151086, 152110, 153134, 154158, 155182, 156206, 157230, 158254, 159278, 160302, 161326, 162350, 163374, 164398, 165422, 166446, 167470, 168494, 169518, 170542, 171566, 172590, 173614, 174638, 175662, 176686, 177710, 178734, 179758, 180782, 181806, 182830, 183854, 184878, 185902, 186926, 187950, 188974, 189998, 191022, 192046, 193070, 194094, 195118, 196142, 197166, 198190, 199214, 200238, 201262, 202286, 203310, 204334, 205358, 206382, 207406, 208430, 209454, 210478, 211502, 212526, 213550, 214574, 215598, 216622, 217646, 218670, 219694, 220718, 221742, 222766, 223790, 224814, 225838, 226862, 227886, 228910, 229934, 230958, 231982, 233006, 234030, 235054, 236078, 237102, 238126, 26160, 27184, 28208, 29232, 30256, 31280, 32304, 33328, 34352, 35376, 36400, 37424, 38448, 39472, 40496, 41520, 42544, 43568, 44592, 45616, 46640, 47664, 48688, 49712, 50736, 51760, 52784, 53808, 54832, 55856, 56880, 57904, 58928, 59952, 60976, 62000, 63024, 64048, 65072, 66096, 67120, 68144, 69168, 70192, 71216, 72240, 73264, 74288, 75312, 76336, 77360, 78384, 79408, 80432, 81456, 82480, 83504, 84528, 85552, 86576, 87600, 88624, 89648, 90672, 91696, 92720, 93744, 94768, 95792, 96816, 97840, 98864, 99888, 100912, 101936, 102960, 103984, 105008, 106032, 107056, 108080, 109104, 110128, 111152, 112176, 113200, 114224, 115248, 116272, 117296, 118320, 119344, 120368, 121392, 122416, 123440, 124464, 125488, 126512, 127536, 128560, 129584, 130608, 131632, 132656, 133680, 134704, 135728, 136752, 137776, 138800, 139824, 140848, 141872, 142896, 143920, 144944, 145968, 146992, 148016, 149040, 150064, 151088, 152112, 153136, 154160, 155184, 156208, 157232, 158256, 159280, 160304, 161328, 162352, 163376, 164400, 165424, 166448, 167472, 168496, 169520, 170544, 171568, 172592, 173616, 174640, 175664, 176688, 177712, 178736, 179760, 180784, 181808, 182832, 183856, 184880, 185904, 186928, 187952, 188976, 190000, 191024, 192048, 193072, 194096, 195120, 196144, 197168, 198192, 199216, 200240, 201264, 202288, 203312, 204336, 205360, 206384, 207408, 208432, 209456, 210480, 211504, 212528, 213552, 214576, 215600, 216624, 217648, 218672, 219696, 220720, 221744, 222768, 223792, 224816, 225840, 226864, 227888, 228912, 229936, 230960, 231984, 233008, 234032, 235056, 236080, 237104};

    for (int n = 0; n < 10891; n++)
    {
        *(short int *)(pixel_buffer_start + walls[n]) = BLUE;
    }
}