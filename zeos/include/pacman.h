#ifndef PACMAN_HH
#define PACMAN_HH

#include <colors.h>
#include <libc.h>

//Basic game layout
#define MAX_NAME_LENGTH 10
#define MAX_LEADERBOARD_ENTRIES 2
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
#define BOOSTED_TIME 5000
#define RESPAWN_TIME 3000
#define FPS 5
#define FRAME_TIME 1.0/FPS

#define uint unsigned int

typedef struct {
    char name[MAX_NAME_LENGTH+1];
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
    char tile;
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
    uint pelletsRemaining;
    uint level;
    char map[MAP_HEIGHT][MAP_WIDTH+1];
    LeaderboardEntry leaderboard[MAX_LEADERBOARD_ENTRIES];
    int leaderboardSize;
    char currentPlayer[MAX_NAME_LENGTH+1];
} GameState;


//Priority queue
typedef struct {
    Point position;
    int priority;
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
void initializeRound(GameState *game, uint level, uint lives, uint score, char currentPlayer[MAX_NAME_LENGTH+1]);

/*******************Rendering and MAP functions******************/
void clear_line(uint row);

void renderEntities(GameState* game);

//WIP
void renderFooter(GameState* game, uint ticks);

//This function can be optimized to avoid print overhead
void renderMap(GameState* game);

void renderGame(GameState* game, int render_bg, uint ticks);

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

#endif