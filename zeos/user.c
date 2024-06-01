#include <colors.h>
#include <libc.h>
#include <list.h>
#include <pacman.h>

int print(char* xd) {
    write(1, xd, strlen(xd));
    return 1;
}

void gameloop()
{
    int n_frames = 0;
    GameState game;
    initializeRound(&game, 0, 3, 0);
    //Time variables to track elapsed time between updates
    unsigned int realFrameTime, currentTime, elapsedTime, lastTime = get_time_ms();

    while(1){
        n_frames++;
        //Handle input logic asynchronously
        get_input();
        //Get elapsed time since last update
        currentTime = get_time_ms();
        elapsedTime = currentTime - lastTime;
        //Update the game 
        updateGameState(game, elapsedTime);

        //Render the game
        renderGame(&game);

        //Control the framerate
        realFrameTime = get_time_ms() - currentTime;
        if(realFrameTime < FRAME_TIME){
            sleep(FRAME_TIME - realFrameTime);
        }
    }
}

//WIP: this still needs work
//Gets 1 char input from the shared pages
void get_input(GameState* game){

    char input = 'w';
    //Some logic should be added here that takes input from 
    //The shared memory region
    processInput(game, input);
}

//WIP: this still needs work
void input_processing()
{
    //Some logic should be added here that reads the kbd 
    //and updates the shared memory region
}

void update_fps(int frames)
{
    // ...
}

int __attribute__((__section__(".text.main"))) main(void) {
    // Please modify this function with your desired user.c code
    int pid = fork();

    if (pid == 0) input_processing();
    else {
        gameloop();
    }
    while(1);
}