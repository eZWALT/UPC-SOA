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
    RUN_AWAY
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
} Pacman;

typedef struct {
    Pacman pacman;
    Ghost ghosts[NUM_GHOSTS];
    uint score;
    uint lives;
    uint level;
    char map[MAP_HEIGHT][MAP_WIDTH];
    LeaderboardEntry leaderboard[MAX_LEADERBOARD_ENTRIES];
    char currentPlayer[MAX_NAME_LENGTH];
} GameState;


//Priority queue
typedef struct {
    Point position;
    int distance;
} PriorityNode;

PriorityNode dequeue(PriorityNode* queue, int* front, int rear){
    int minIndex = *front;
    for(int i = *front+1; i < rear; ++i){
        if(queue[i].distance < queue[minIndex].distance){
            minIndex = i;
        }
    }
    PriorityNode node = queue[minIndex];
    for(int i = minIndex; i < rear-1;i++){
        queue[i] = queue[i+1];
    }
    (*front)++;
    return node;
}

void enqueue(PriorityNode* queue, int* rear, Point position, int distance){
    queue[*rear].position = position;
    queue[*rear].distance = distance;
    (*rear)++;
}

//Ghosts must spawn on the middle on the 1st map and on each corner on the 2nd
char predefinedMaps[NUM_LEVELS][24][80] = {
    {
    "################################################################################",
    "#*...................................#####....................................*#",
    "#.###.###.###.###.###.###.###.###.##.#####.##.###.###.###.###.###.###.###.####.#",
    "#.###.###.###.###.###.###.###.###.##.#####.##.###.###.###.###.###.###.###.####.#",
    "#.###.###.###.###.###*###.###.###.##.#####.##.###.###.###*###.###.###.###.####.#",
    "#.............###.###.###.###.###.##.#####.##.###.###.###.###.###..............#",
    "#.###.###.###.###.###.###.###.###.##.#####.##.###.###.###.###.###.###.###.####.#",
    "#.###.###.###.###.###.###..............*..............###.###.###.###.###.####.#",
    "#.###.###.###.###.###.###.#######.###########.#######.###.###.###.###.###.####.#",
    "#*........................#######.#.........#.#######.........................*#",
    "#################.###.###.........#.........#.........###.###.##################",
    "#################*###.###.#########..#. .#..#########.###*###.##################",
    "#################.###.###............# * #............###.###.##################",
    "#################.###.###.#########..#. .#..#########.###.###.##################",
    "#*................................#..#####..#.................................*#",
    "#.###.###.###.###.###.###.#######.#.........#.#######.###.###.###.###.###.###..#",
    "#.###.###.###.###.###.###.#######.#.........#.#######.###.###.###.###.###.###..#",
    "#.###.###.###.###.###.###.#######.###########.#######.###.###.###.###.###.###..#",
    "#.............###.###.###..............*..............###.###.###..............#",
    "#.###.###.###.###.###*###.###.###.##.#####.##.###.###.###*###.###.###.###.###..#",
    "#.###.###.###.###.###.###.###.###.##.#####.##.###.###.###.###.###.###.###.###..#",
    "#.###.###.###.###.###.###.###.###.##.#####.##.###.###.###.###.###.###.###.###..#",
    "#*...................................#####....................................*#",
    "################################################################################"
    },
    {
    "################################################################################",
    "#.................*..........................................................*.#",
    "#..............................................................................#",
    "#...######...############...####...############...#######...####...#########...#",
    "#...######...############...####...############...#######...####...#########...#",
    "#..........*....................................*..............................#",
    "#...######...####...############...#######...#######...############.*.#######..#",
    "#...######...####...############.*.#######...#######...############...#######..#",
    "#..............................................................................#",
    "#..*######...################...########...#######...########...###########....#",
    "#...######...################...########...#######...########...###########*...#",
    "#..............................................................................#",
    "#...####...*..###############....########...*..#################...*...####....#",
    "#...####......###############....########......#################.......####....#",
    "#..............................................................................#",
    "#...######...##########...#######.......###############...#######...########...#",
    "#...######...##########...#######.......###############...#######...########...#",
    "#..............................................................................#",
    "#...######...################...#######...#######...################...#####...#",
    "#...######.*.################...#######...#######.*.################.*.#####...#",
    "#...######...################...#######...#######...################...#####...#",
    "#..............................................................................#",
    "#*............................*................................................#",
    "################################################################################",
    }
};

/******************* Initialization functions******************/
void initializeRound(GameState *game, uint level, uint lives, uint score){
    game->score = 0;
    game->lives = 3;
    game->level = 0;

    Point pacman_spawn = {1, 1};
    Point ghost_spawns[NUM_GHOSTS] = {Point()};

    // initialize
    initializeMap(game);
    initalizePacman(game, pacman_spawn);
    initializeGhosts(game, ghost_spawns); 
}

//Copy the map row by row, using the current level to select the map
void initializeMap(GameState* game){
    for (int i = 0; i < MAP_HEIGHT; ++i){
        strcpy(game->map[i], predefinedMaps[game->level][i]);
    }
}

//Initial position of ghosts, surrounding (X,Y), default colors
void initializeGhosts(GameState* game, Point spawn[NUM_GHOSTS]){
    for (int i = 0; i < NUM_GHOSTS; ++i){
        game->ghosts[i].spawnPoint = spawn[i];
    }
}

void initializePacman(GameState *game, Point spawn){
    game->pacman.spawnPoint = spawn;
    game->pacman.direction = RIGHT;
    game->pacman.isBoosted = 0;
    game->pacman.skin = 'C';
}

/*******************Leaderboard functions******************/
void initializeLeaderboard(GameState *game){
    for(int i = 0; i < MAX_LEADERBOARD_ENTRIES; ++i){
        strcpy(game->leaderboard[i].name, "");
        game->leaderboard[i].score = 0;
    }
}

void updateLeaderboard(GameState* game, const char* name, int score){

}

void displayLeaderboard(GameState* game){
    
}

/*******************Rendering and MAP functions******************/
//Probably missing a clear function (Rendering)

void removeItem(GameState* game, Point position){
    game->map[position.y][position.x] = ' ';
}
void renderMap(GameState* game){
    gotoxy(0, 1);

    for(int i = 0; i < MAP_HEIGHT; ++i){
        for(int j = 0; j < MAP_WIDTH; ++j){
            char cell = game->map[i][j];
            if(cell == "#"){
                set_color(0x01, 0x00);
            }
            else if(cell == "*"){
                set_color(0x04, 0x00);
            }
            else if(cell == " "){
                set_color(0x02,0x00);
            }
            print(cell);
        }
    }
}

void renderEntities(GameState* game) {
    // Render Pacman
    set_color(game->pacman.fg_color, game->pacman.bg_color);
    gotoxy(game->pacman.position.x, game->pacman.position.y);
    print(game->pacman.skin);

    // Render ghosts
    for (int i = 0; i < NUM_GHOSTS; ++i) {
        if (game->ghosts[i].isAlive) {
            set_color(game->ghosts[i].fg_color, game->pacman.bg_color);
            gotoxy(game->ghosts[i].position.x, game->ghosts[i].position.y);
            print(game->ghosts[i].skin);
        }
    }
    
}

void clear_line(uint row){
    char clear_str[MAP_WIDTH+1];
    clear_str[MAP_WIDTH] = '\0';
    set_color(BLACK, BLACK);
    gotoxy(0, row);
    print(clear_str);
}

void renderFooter(GameState* game){
    gotoxy(0, MAP_HEIGHT+1);
    //Clear footer
    clear_line(MAP_HEIGHT+1);
    set_color(WHITE, BLACK);

    const char * lives = "Lives: ";
    const char * score = "Score: ";
    const char * player = "Player: ";

    print(lives);
    //print(game->lives);
}
/******************* Collision and Movement ******************/
uint isPellet(GameState* game, Point position){
    return game->map[position.y][position.x] == '.';
}

uint isWall(GameState* game, Point position){
    return game->map[position.y][position.x] == '#';
}

uint isBooster(GameState* game, Point position){
    return game->map[position.y][position.x] == '*';
}

uint isGhost(GameState* game, Point position){
    for(int i = 0; i < NUM_GHOSTS; ++i){
        if(equalPoints(game->ghosts[i].position, position)) 
            return 1;
    }
    return 0;
}

int equalPoints(Point a, Point b){
    return a.x == b.x && a.y == b.y;
}

uint isValidMove(GameState* game, Point position){
    return position.x >= 0 && position.x < MAP_WIDTH && 
           position.y >= 0 && position.y < MAP_HEIGHT && 
           !isWall(game, position);
}

Direction determineDirection(Point current, Point next){
    if(next.y < current.y) return UP;
    if(next.y > current.y) return DOWN;
    if(next.x < current.x) return LEFT;
    if(next.x > current.x) return RIGHT;
    return NONE;
}

Direction random(){
    switch (rand() % 4) {
        case 0: return UP;  
        case 1: return DOWN;  
        case 2: return LEFT;  
        case 3: return RIGHT;
    }
}

Point getNextPoint(Point initial, Direction direction){
    switch(direction){
        case UP: initial.y++; break;
        case DOWN: initial.y--; break;
        case RIGHT: initial.x++; break;
        case LEFT: initial.x--; break;
    }
    return initial;
}

Direction dfs(GameState* game, Point start, Point target){
    char visited[MAP_HEIGHT][MAP_WIDTH] = {0};
    //Define a max size for the stack
    Point stack[MAP_HEIGHT * MAP_WIDTH];
    Point previous[MAP_HEIGHT][MAP_WIDTH];
    int top = 0;

    stack[top] = start;
    visited[start.y][start.x] = 1;

    //Do DFS iteratively to avoid destroying the tiny stack of the OS
    while(top >= 0){
        Point current = stack[top--];
        
        if(equalPoints(current,target)){
            Point pos = current;
            while (previous[pos.y][pos.x].x != start.x || previous[pos.y][pos.x].y != start.y) {
                pos = previous[pos.y][pos.x];
            }
            return determineDirection(pos, target);
        }

        Point moves[] = {{current.x-1,current.y}, {current.x+1, current.y}, {current.x, current.y-1}, {current.x, current.y+1}};
        for(int i = 0; i < 4; ++i){
            Point next = moves[i];
            if(isValidMove(game, next) && !visited[next.x][next.y]){
                stack[++top] = next;
                visited[next.y][next.x] = 1;
                previous[next.y][next.x] = current;
            }
        }
    }
    return NONE;
}

Direction bfs(GameState* game, Point start, Point target){
    char visited[MAP_HEIGHT][MAP_WIDTH] = {0};
    Point queue[MAP_HEIGHT * MAP_WIDTH];
    int front = 0, rear = 0;
    Point previous[MAP_HEIGHT][MAP_WIDTH];

    //Enqueue starting point 
    queue[rear++] = start;
    visited[start.y][start.x] = 1;

    while(front < rear){
        Point current = queue[front++];
        
        //If found, backtrack
        if(equalPoints(current,target)){
            Point pos = current;
            while (previous[pos.y][pos.x].x != start.x || previous[pos.y][pos.x].y != start.y) {
                pos = previous[pos.y][pos.x];
            }
            return determineDirection(pos, target);
        }

        Point moves[] = {{current.x - 1, current.y}, {current.x + 1, current.y}, {current.x, current.y - 1}, {current.x, current.y + 1}};
        for(int i = 0; i < 4; ++i){
            Point next = moves[i];
            if(isValidMove(game, next) && !visited[next.y][next.x]){
                queue[rear++] = next;
                visited[next.y][next.x] = 1;
                previous[next.y][next.x] = current;
            }
        }
    }
    return NONE;
}

Direction dijkstra(GameState* game, Point start, Point target){
    int distances[MAP_HEIGHT][MAP_WIDTH];
    Point previous[MAP_HEIGHT][MAP_WIDTH];
    PriorityNode queue[MAP_HEIGHT * MAP_WIDTH]; 
    int front = 0, rear = 0;

    //Initialize infinite distances
    for(int y = 0; y < MAP_HEIGHT; ++y){
        for(int x = 0; x < MAP_WIDTH; ++x){
            distances[y][x] = INFINITY;
        }
    }

    //Initialize start point
    distances[start.y][start.x] = 0;
    enqueue(queue, &rear, start, 0);

    //Actual dijkstra
    while(front < rear){
        PriorityNode node = dequeue(queue, &front, rear);
        Point pos = node.position;
        
        //target reached, backtrack until the first step
        if(equalPoints(pos, target)){
            while(previous[pos.y][pos.x].x != start.x || previous[pos.y][pos.x].y != start.y){
                pos = previous[pos.y][pos.x];
            }
            //This line should be checked
            return determineDirection(pos, target);
        }

        Point moves[] = {{pos.x-1, pos.y}, {pos.x+1, pos.y}, {pos.x, pos.y-1}, {pos.x,pos.y+1}};
        for(int i = 0; i < 4; ++i){
            Point next = moves[i];

            //Check if the movement is legal
            if(isValidMove(game, next) && distances[next.y][next.x] > distances[pos.y][pos.x] + 1){
                distances[next.y][next.x] = distances[pos.y][pos.x] + 1;
                previous[next.y][next.x] = pos;
                enqueue(queue, &rear, next, distances[next.y][next.x]);
            }
        }
    }
    return NONE;
}

/******************* Update state ******************/

void changeGhostsBehaviour(GameState* game, Behaviour bh){
    for(int i = 0; i < NUM_GHOSTS; ++i){
        game->ghosts[i].behaviour = bh;
        //Change the color of the ghost to blue and white
        if(bh == RUN_AWAY){
            game->ghosts[i].fg_color = 3;
        }
        else if(bh == CHASE){
            game->ghosts[i].fg_color = 2;
        }
    }
}

void updatePacmanPosition(GameState* game){
    Point nextPosition = game->pacman.position;

    switch(game->pacman.direction){
        case UP: nextPosition.y--; break;
        case DOWN: nextPosition.y++; break;
        case LEFT: nextPosition.x--; break;
        case RIGHT: nextPosition.x++; break;
    }

    //Check if any item is consumed
    if(!isWall(game, nextPosition)){
        game->pacman.position = nextPosition;

        if(isPellet(game, nextPosition)){
            game->score += PELLET_POINTS;
            removeItem(game, nextPosition);

        }
        else if(isBooster(game, nextPosition)){
            game->score += BOOSTER_POINTS;
            removeItem(game, nextPosition);
            game->pacman.isBoosted = 1;
            game->pacman.boostedTimer = BOOSTED_TIME;
            //All ghosts enter panic state
            changeGhostsBehaviour(game, RUN_AWAY);
        }
    }


    //Check if there is a collision with any ghost
    if(isGhost(game, nextPosition)){
        for(int i = 0; i < NUM_GHOSTS; ++i){
            //Ghosts will be eaten
            if(game->pacman.isBoosted){
                if(equalPoints(game->ghosts[i].position, nextPosition) && game->ghosts[i].isAlive){
                    game->ghosts[i].position = game->ghosts[i].spawnPoint;
                    game->ghosts[i].deadTimer = RESPAWN_TIME;
                    game->ghosts[i].isAlive = 0;
                    game->score += GHOST_POINTS;
                }
            }
            //Ghost eats pacman
            else{
                if(equalPoints(game->ghosts[i].position, nextPosition) && game->ghosts[i].isAlive){
                    game->lives--;
                    game->pacman.position = game->pacman.spawnPoint;
                    break; //No need to check for more collisions
                }
            }
        }
    }
}

void updateTimerState(GameState* game, uint elapsedTime){
    //Update boosted timer
    if(game->pacman.isBoosted){
        game->pacman.boostedTimer -= elapsedTime;
        if(game->pacman.boostedTimer <= 0){
            game->pacman.isBoosted = 0;
            changeGhostsBehaviour(game, CHASE);
        }
    } 

    //Update ghosts status
    for(int i = 0; i < NUM_GHOSTS; ++i){
        if(!game->ghosts[i].isAlive){
            game->ghosts[i].deadTimer -= elapsedTime;
            if(game->ghosts[i].deadTimer <= 0){
                game->ghosts[i].isAlive = 1;
            }
        }
    }
}

void updateGhostsPositions(GameState* game){
    for(int i = 0; i < NUM_GHOSTS; ++i){
        if(game->ghosts[i].isAlive){
            Point nextPosition, initialPosition = game->ghosts[i].position;
            Direction dir;
            //Tiny heuristic (AI!) :)
            Point targetPosition = (game->ghosts[i].behaviour == CHASE)
            ? game->pacman.position : game->ghosts[i].spawnPoint;

            switch (game->ghosts[i].algorithm) {
                case RANDOM:
                    dir = random();
                case DFS:
                    dir = dfs(game, initialPosition, targetPosition);
                case BFS:
                    dir = bfs(game, initialPosition, targetPosition);
                case DIJKSTRA:
                    dir = dijkstra(game, initialPosition, targetPosition);
            }

            nextPosition = getNextPoint(initialPosition, dir);

            //If a wall is hitten, then the movement will be randomized
            //This can be infinite if you are infinitely unlucky
            while(isWall(game, nextPosition)){
                nextPosition = getNextPoint(initialPosition, random());
            }
            if(isValidMove(game, nextPosition)){
                game->ghosts[i].position = nextPosition;
                game->ghosts[i].direction = determineDirection(game->ghosts[i].position, nextPosition);
            }
        }
    }
}



/******************* Main game functions ******************/
//THESE FUNCTIONS ARE STILL ON DEVELOPMENT !!!!!!! 
//YET TO BE IMPLEMENTED

//Main loop for a single player (until all the lives are consumed)
void playGame(GameState* game, const char *playerName){

    while(game->lives > 0){

    }

    updateLeaderboard(game, playerName, game->score);
}

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

//MAIN MAIN LOOP (WITH TIME)
int pacman_main() {
    srand(time(NULL));

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


#endif PACMAN_HH