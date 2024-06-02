#include <colors.h>
#include <libc.h>
#include <list.h>
#include <pacman.h>

#define TICKS_PER_FRAME TICKS_PER_SECOND/FPS 

int print(char* xd) {
    write(1, xd, strlen(xd));
    return 1;
}

void gameloop()
{
    srand(322321);
    char * addr = (char *) shmat(0, (void *) 0x16C000);
    int n_frames = 0;
    GameState game;
    char currentPlayer[MAX_NAME_LENGTH+1];
    currentPlayer[MAX_NAME_LENGTH] = '\0';
    initializeLeaderboard(&game);
    //renderLeaderboard(&game);

    char buff[32];
    itodeca(game.leaderboardSize, buff);
    print(buff);

    while(1){
        if(game.leaderboardSize >= MAX_LEADERBOARD_ENTRIES){
            renderGameOver(&game);
            while(1);
        }
        //Get player name
        renderIntroduceName(&game);
        get_player_name(currentPlayer, addr);

        //Init round
        initializeRound(&game, 0, NUM_LIVES, 0, currentPlayer);

        //Time variables to track elapsed time between updates
        int currentTime, elapsedTime, lastTime = get_time_ms();
        renderGame(&game, 1, gettime());
        unsigned long initTimer = gettime();
        unsigned long prevTimer = initTimer;

        while(1){
            n_frames++;
            //Handle input logic asynchronously
            processInput(&game, addr[0]);
            //Get elapsed time since last update
            lastTime    = currentTime;
            currentTime = get_time_ms();
            elapsedTime = currentTime - lastTime;
            //Update the game 
            updateGameState(&game, elapsedTime);

            //The player has been acting sus lately...
            
            if(playerHasNoLife(&game)){
                updateLeaderboard(&game, game.currentPlayer, game.score);
                renderGameOver(&game);
                sleep(3000);
                renderLeaderboard(&game);
                sleep(3000);
                break;
            }
            if(isRoundOver(&game)){
                if(game.level == 0){
                    initializeRound(&game, 1, game.lives, game.score, game.currentPlayer);
                    sleep(2000);
                }
                else if(game.level >= 1){
                    updateLeaderboard(&game, game.currentPlayer, game.score);
                    renderGameOver(&game);
                    sleep(3000);
                    renderLeaderboard(&game);
                    sleep(3000);
                    break;
                }
            }
            
            //Render the game

            while (gettime() - prevTimer < TICKS_PER_FRAME);
            renderGame(&game, 0, gettime() - prevTimer);
            prevTimer = gettime();
        }

    }
}

void get_player_name(char name[MAX_NAME_LENGTH + 1], volatile char x[MAX_NAME_LENGTH+1]) {
    int numChars = 0;
    name[MAX_NAME_LENGTH] = '\0';
    char lastChar = '\0';

    while (numChars < MAX_NAME_LENGTH) {
        char input = *x;
        if(input == '.') break;
        // Check if input is a new valid character
        if (input != '\0' && input != lastChar) {
            name[numChars] = input;
            numChars++;
            lastChar = input;  // Update the lastChar to the current input
        }
        sleep(100);
    }
    print(name);
}

void get_input(GameState* game, char * x)
{
    processInput(game, *x);
}

void input_processing()
{

    char * addr = (char *) shmat(0, (void *) 0x16C000);

    while (1)
    {

        char x[2];
        int nc = read(x, 1);
        x[2] = '\0';

        if (nc == 0) continue;

        (*addr) = x[0];
    }
}

int __attribute__((__section__(".text.main"))) main(void) {
    // Please modify this function with your desired user.c code
    int pid = fork();

    if (pid == 0) input_processing();
    else          gameloop();

}