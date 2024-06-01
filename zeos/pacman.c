#include <pacman.h>
#include <colors.h>
#include <libc.h>

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

//The additional space is used for null terminator (Since map is treated as an array of strings
//Rather than a matrix of characters)
char predefinedMaps[NUM_LEVELS][MAP_HEIGHT][MAP_WIDTH+1] = {
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
    "#.###.###.###.###.###.###.#######.#.........#.#######.###.###.###.###.###.####.#",
    "#.###.###.###.###.###.###.#######.#.........#.#######.###.###.###.###.###.####.#",
    "#.###.###.###.###.###.###.#######.###########.#######.###.###.###.###.###.####.#",
    "#.............###.###.###..............*..............###.###.###..............#",
    "#.###.###.###.###.###*###.###.###.##.#####.##.###.###.###*###.###.###.###.####.#",
    "#.###.###.###.###.###.###.###.###.##.#####.##.###.###.###.###.###.###.###.####.#",
    "#.###.###.###.###.###.###.###.###.##.#####.##.###.###.###.###.###.###.###.####.#",
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
    "#.*...........................*................................................#",
    "################################################################################",
    }
};

/******************* Input handling functions******************/
void processInput(GameState* game, char input){
    input = lower(input);
    switch(input){
        case 'w':
            game->pacman.direction = UP;
            break;
        case 's':
            game->pacman.direction = DOWN;
            break;
        case 'a': 
            game->pacman.direction = LEFT;
            break;
        case 'd':
            game->pacman.direction = RIGHT;
            break;
        default: 
            game->pacman.direction = NONE;
            break;
    }
}

/******************* Initialization functions******************/
//Copy the map row by row, using the current level to select the map
void initializeMap(GameState* game){
    for (int i = 0; i < MAP_HEIGHT; ++i){
        strcpy(game->map[i], predefinedMaps[game->level][i]);
    }
}

//Initial position of ghosts, surrounding (X,Y), default colors
void initializeGhosts(GameState* game, Point spawn[NUM_GHOSTS]){
    Algorithm ghost_alg[NUM_GHOSTS] = {RANDOM, BFS, DFS, DIJKSTRA};
    int ghost_color[NUM_GHOSTS] = {LIGHT_CYAN, LIGHT_MAGENTA, LIGHT_GREEN, RED};

    for (int i = 0; i < NUM_GHOSTS; ++i){
        game->ghosts[i].spawnPoint = spawn[i];
        game->ghosts[i].position = spawn[i];
        game->ghosts[i].direction = NONE;
        game->ghosts[i].skin = '0';
        game->ghosts[i].tile = '.';
        game->ghosts[i].fg_color = ghost_color[i];
        game->ghosts[i].bg_color = BLACK;
        game->ghosts[i].isAlive = 1;
        game->ghosts[i].deadTimer = 0;
        game->ghosts[i].algorithm = ghost_alg[i];
        game->ghosts[i].behaviour = CHASE;
    }
}

void initializePacman(GameState *game, Point spawn){
    game->pacman.spawnPoint = spawn;
    game->pacman.position = spawn;
    game->pacman.direction = NONE;
    game->pacman.fg_color = WHITE;
    game->pacman.bg_color = BLACK;
    game->pacman.skin = 'C';
    game->pacman.isBoosted = 0;
    game->pacman.boostedTimer = 0;
    game->pacman.respawnTimer = 0;
    game->pacman.isAlive = 1;
    game->map[spawn.y][spawn.x] = 'C';
}

void initializeLeaderboard(GameState *game){
    for(int i = 0; i < MAX_LEADERBOARD_ENTRIES; ++i){
        strcpy(game->leaderboard[i].name, "");
        game->leaderboard[i].score = 0;
    }
}

void initializeRound(GameState *game, uint level, uint lives, uint score){
    game->score = score;
    game->lives = lives;
    game->level = level;

    Point pacman_spawn;
    Point ghost_spawns[NUM_GHOSTS];

    // Set spawn points based on the level
    if (game->level == 0) {
        pacman_spawn = (Point){36, 1};
        ghost_spawns[0] = (Point){38, 11};
        ghost_spawns[1] = (Point){40, 11};
        ghost_spawns[2] = (Point){38, 13};
        ghost_spawns[3] = (Point){40, 13};
        game->pelletsRemaining = 300;
    } else {
        pacman_spawn = (Point){42, 10};
        ghost_spawns[0] = (Point){78, 1};
        ghost_spawns[1] = (Point){1, 22};
        ghost_spawns[2] = (Point){1, 1};
        ghost_spawns[3] = (Point){78, 22};
        game->pelletsRemaining = 600;

    }

    // initialize
    initializeMap(game);
    initializePacman(game, pacman_spawn);
    initializeGhosts(game, ghost_spawns); 
}

/*******************Rendering and MAP functions******************/

// Function to map characters to their corresponding foreground and background colors
void getMapColors(GameState * game, char character, int* fg_color, int* bg_color) {
    switch (character) {
        case '#':
            *fg_color = LIGHT_RED;
            *bg_color = BLACK;
            break;
        case '*':
            *fg_color = LIGHT_GREEN;
            *bg_color = BLACK;
            break;
        case '.':
            *fg_color = LIGHT_CYAN;
            *bg_color = BLACK;
            break;
        case ' ':
            *fg_color = BLACK;
            *bg_color = BLACK;
            break;
        case 'C':
            *fg_color = game->pacman.fg_color;
            *bg_color = game->pacman.bg_color;
            break;
        case '0':
            *fg_color = game->ghosts[0].fg_color;
            *bg_color = game->ghosts[0].bg_color;
            break;
        case '1':
            *fg_color = game->ghosts[1].fg_color;
            *bg_color = game->ghosts[1].bg_color;
            break;
        case '2':
            *fg_color = game->ghosts[2].fg_color;
            *bg_color = game->ghosts[2].bg_color;
            break;
        case '3':
            *fg_color = game->ghosts[3].fg_color;
            *bg_color = game->ghosts[3].bg_color;
            break;

        // Add cases for other characters as needed
        default:
            // Default color if character not found
            *fg_color = WHITE;
            *bg_color = BLACK;
            break;
    }
}
void clear_line(uint row){
    char clear_str[MAP_WIDTH+1];
    clear_str[MAP_WIDTH] = '\0';
    set_color(BLACK, BLACK);
    gotoxy(0, row);
    print(clear_str);
}

//Probably missing a clear function (Rendering)
void renderEntities(GameState* game) {
    // Render Pacman
    char entity[2] = {game->pacman.skin, '\0'};
    set_color(game->pacman.fg_color, game->pacman.bg_color);
    gotoxy(game->pacman.position.x, game->pacman.position.y);
    print(entity);

    // Render ghosts
    for (int i = 0; i < NUM_GHOSTS; ++i) {
        if (game->ghosts[i].isAlive) {
            set_color(game->ghosts[i].fg_color, game->pacman.bg_color);
            gotoxy(game->ghosts[i].position.x, game->ghosts[i].position.y);
            entity[0] = game->ghosts[i].skin;
            print(entity);
        }
    }
}

//WIP
void renderFooter(GameState* game){
    //Clear footer
    clear_line(MAP_HEIGHT+1);
    gotoxy(0, MAP_HEIGHT+1);
    set_color(WHITE, BLACK);

    const char * lives = "Lives: ";
    const char * score = "Score: ";
    const char * player = "Player: ";

    print(lives);
    print(score);
    print(player);
    //print(game->lives);
}

//This function can be optimized to avoid print overhead
//WIP: adding buffer to avoid blinking
void renderMap(GameState* game){
    gotoxy(0, 0);
    char buffer[2];
    buffer[1] = '\0';
    int fg_color,bg_color;

    for(int i = 0; i < MAP_HEIGHT; ++i){
        for(int j = 0; j < MAP_WIDTH; ++j){
            buffer[0] = game->map[i][j];
            if(buffer[0] == '\0') continue;

            getMapColors(game, buffer[0], &fg_color, &bg_color);
            set_color(fg_color, bg_color);
            print(buffer);
        }
    }
}

void renderGame(GameState* game, int to_render_map){
    renderMap(game);
    //renderEntities(game);
    //renderFooter(game);
}

void removeItem(GameState* game, Point position){
    game->map[position.y][position.x] = ' ';
}

//WIP
void renderLeaderboard(){

}

//WIP
void renderGameOver(){

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
    switch (rand_range(0, 4)) {
        case 0: return UP;  
        case 1: return DOWN;  
        case 2: return LEFT;  
        case 3: return RIGHT;
        default: return NONE;
    }
}

Point getNextPoint(Point initial, Direction direction){
    switch(direction){
        case UP: initial.y++; break;
        case DOWN: initial.y--; break;
        case RIGHT: initial.x++; break;
        case LEFT: initial.x--; break;
        case NONE: break;
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

//WIP
void updateLeaderboard(GameState* game, const char* name, int score){

}

void changeGhostsBehaviour(GameState* game, Behaviour bh){
    for(int i = 0; i < NUM_GHOSTS; ++i){
        game->ghosts[i].behaviour = bh;
        //Change the color of the ghost to blue and white
        if(bh == RUN_AWAY){
            game->ghosts[i].fg_color = 3;
        }
        else if(bh == CHASE || bh == STILL){
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
        case NONE: break;
    }

    //Check if any item is consumed
    if(!isWall(game, nextPosition)){
        game->map[game->pacman.position.y][game->pacman.position.x] = ' ';
        game->pacman.position = nextPosition;
        game->map[game->pacman.position.y][game->pacman.position.x] = 'C';

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
                    game->pacman.isAlive = 0;
                    game->pacman.respawnTimer = RESPAWN_TIME;
                    changeGhostsBehaviour(game, STILL);
                    break; //No need to check for more collisions
                }
            }
        }
    }

    // Update map
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

    if(!game->pacman.isAlive){
        game->pacman.respawnTimer -= elapsedTime;
        if(game->pacman.respawnTimer <= 0){
            game->pacman.isAlive = 1;
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
            Behaviour bh = game->ghosts[i].behaviour;
            Point targetPosition;
            switch(bh){
                case STILL: 
                    return;
                case CHASE:
                    targetPosition = game->pacman.position;
                    break;
                case RUN_AWAY:
                    targetPosition = game->ghosts[i].spawnPoint;
                    break;
            }

            switch (game->ghosts[i].algorithm) {
                case RANDOM:
                    dir = random();
                    break;
                case DFS:
                    //dir = dfs(game, initialPosition, targetPosition);
                    dir = random();
                    break;
                case BFS:
                    //dir = bfs(game, initialPosition, targetPosition);
                    dir = random();
                    break;
                case DIJKSTRA:
                    dir = dijkstra(game, initialPosition, targetPosition);
                    //dir = random();
                    break;
                default:
                    dir = random();
            }

            nextPosition = getNextPoint(initialPosition, dir);

            //If a wall is hitten, then the movement will be randomized
            //This can be infinite if you are infinitely unlucky
            while(isWall(game, nextPosition) || isGhost(game, nextPosition)){
                nextPosition = getNextPoint(initialPosition, random());
            }
            if(isValidMove(game, nextPosition)){
                game->map[initialPosition.y][initialPosition.x] = game->ghosts[i].tile;

                game->ghosts[i].tile = game->map[nextPosition.y][nextPosition.x];
                if (game->ghosts[i].tile != ' ' && game->ghosts[i].tile != '*' &&game->ghosts[i].tile != '.')
                    game->ghosts[i].tile = ' ';

                game->ghosts[i].position = nextPosition;

                char ic;
                switch(i)
                {
                    case 0:
                    ic = '0';
                    break;
                    case 1:
                    ic = '1';
                    break;
                    case 2:
                    ic = '2';
                    break;
                    case 3:
                    ic = '3';
                    break;
                }

                game->map[nextPosition.y][nextPosition.x] = ic;

                game->ghosts[i].direction = determineDirection(game->ghosts[i].position, nextPosition);
            }
        }
    }
}

int isRoundOver(GameState * game){
    return 0;
}

void updateGameState(GameState* game, uint elapsedTime){
    //updatePacmanPosition(game);
    updateGhostsPositions(game);
    updateTimerState(game, elapsedTime);

    if(isRoundOver(game)){
        //initializeRound();
    }
}