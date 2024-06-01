#ifndef PACMAN_HH
#define PACMAN_HH

#include <colors.h>
#include <libc.h>

//Basic game layout
#define MAX_NAME_LENGTH 10
#define MAX_LEADERBOARD_ENTRIES 5
#define MAP_HEIGHT 24 //One line must be reserved for info
#define MAP_WIDTH 80

//Game variables
#define NUM_LEVELS 2
#define NUM_GHOSTS 4
#define INFINITY 2147483647


//Point variables
#define PELLET_POINTS 10
#define BOOSTER_POINTS 50 
#define GHOST_POINTS 200

//Timer variables miliseconds
//We need a gettime and sleep SYSCALL!!!!!
#define BOOSTED_TIME 10000
#define RESPAWN_TIME 5000
#define FPS 60
#define FRAME_TIME 1.0/FPS

#define uint unsigned int

typedef struct {
    char name[MAX_NAME_LENGTH];
    uint score;
} LeaderboardEntry;

typedef struct {
    uint x,y;
} Point;

typedef enum {
    RANDOM, 
    BFS,
    DFS, 
    DIJKSTRA
} Algorithm;

typedef enum {
    UP,
    DOWN,
    RIGHT,
    LEFT,
    NONE
} Direction;

typedef enum {
    CHASE,
    RUN_AWAY,
    STILL,
} Behaviour;

typedef struct {
    Point spawnPoint;
    Point position;
    Direction direction;
    char skin;
    int fg_color;
    int bg_color;
    uint isAlive;
    uint deadTimer;
    Algorithm algorithm;
    Behaviour behaviour;
} Ghost;

//Both pacman and ghost should belong to an abstract class (entity)
typedef struct {
    Point position;
    Point spawnPoint;
    Direction direction;
    int fg_color;
    int bg_color;
    char skin;
    uint isBoosted;
    uint boostedTimer;
    uint isAlive;
    uint respawnTimer;
} Pacman;

typedef struct {
    Pacman pacman;
    Ghost ghosts[NUM_GHOSTS];
    uint score;
    uint lives;
    uint level;
    char map[MAP_HEIGHT][MAP_WIDTH+1];
    LeaderboardEntry leaderboard[MAX_LEADERBOARD_ENTRIES];
    char currentPlayer[MAX_NAME_LENGTH];
} GameState;


//Priority queue
typedef struct {
    Point position;
    int distance;
} PriorityNode;

/******************* Initialization functions******************/
//Copy the map row by row, using the current level to select the map
void initializeMap(GameState* game);

//Initial position of ghosts, surrounding (X,Y), default colors
void initializeGhosts(GameState* game, Point spawn[NUM_GHOSTS]);

//Initial position and attributes of pacman, the player
void initializePacman(GameState *game, Point spawn);

//initial leaderboard (Once per full game loop only)
void initializeLeaderboard(GameState *game);

//Initialize a round (Once per round)
void initializeRound(GameState *game, uint level, uint lives, uint score);

/*******************Rendering and MAP functions******************/
void clear_line(uint row);

void renderEntities(GameState* game);

//WIP
void renderFooter(GameState* game);

//This function can be optimized to avoid print overhead
void renderMap(GameState* game);

void renderGame(GameState* game);

//Removes the item on the map and it gets replaced by a space
void removeItem(GameState* game, Point position);

//WIP
void renderLeaderboard();

//WIP
void renderGameOver();

/******************* Collision and Movement ******************/
uint isPellet(GameState* game, Point position);

uint isWall(GameState* game, Point position);

uint isBooster(GameState* game, Point position);

uint isGhost(GameState* game, Point position);

int equalPoints(Point a, Point b);

uint isValidMove(GameState* game, Point position);

Direction determineDirection(Point current, Point next);

Point getNextPoint(Point initial, Direction direction);

Direction random();

Direction dfs(GameState* game, Point start, Point target);

Direction bfs(GameState* game, Point start, Point target);

Direction dijkstra(GameState* game, Point start, Point target);

/******************* Update state ******************/

//WIP
void updateLeaderboard(GameState* game, const char* name, int score);

void changeGhostsBehaviour(GameState* game, Behaviour bh);

void updatePacmanPosition(GameState* game); 

void updateTimerState(GameState* game, uint elapsedTime);

void updateGhostsPositions(GameState* game);

void updateGameState(GameState* game, uint elapsedTime);

/******************* Main game functions ******************/

//Main loop for a single player (until all the lives are consumed)
void playGame(GameState* game, const char *playerName);

/*
void main_loop(){
    GameState game;
    char playerName[MAX_NAME_LENGTH];

    initializeLeaderboard(&game);

    while (1) {
        printf("Enter your name: ");
        //Get user input command pattern
        playGame(&game, playerName);

        displayLeaderboard(&game);

        printf("Do you want to play again? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        if (choice != 'y' && choice != 'Y') {
            break;
        }
    }
}
*/

//MAIN MAIN LOOP (WITH TIME)
/*
int pacman_main() {
    srand(0);

    GameState game;
    // Initialize the game state, map, and other variables

    struct timespec lastTime, currentTime;

    // What is CLOCK_MONOTONIC, clock_gettime, timespec ?
    clock_gettime(CLOCK_MONOTONIC, &lastTime);

    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &currentTime);
        int elapsedTime = (currentTime.tv_sec - lastTime.tv_sec) * 1000 + (currentTime.tv_nsec - lastTime.tv_nsec) / 1000000;

        if (elapsedTime >= FRAME_TIME) {
            // Read user input for Pacman direction
            // For example:
            // game.pacman.direction = readInput();

            // Update game state
            updatePacmanPosition(&game);
            updateGhostsPosition(&game, elapsedTime);
            updateBoostedState(&game, elapsedTime);

            // Render the game state
            renderGame(&game);

            // Update lastTime
            lastTime = currentTime;

            // Add a delay or frame rate control here if necessary
        } else {
            // Sleep for the remaining time to maintain a consistent frame rate
            usleep((FRAME_TIME - elapsedTime) * 1000);
        }
    }

    return 0;
}
*/

#endif