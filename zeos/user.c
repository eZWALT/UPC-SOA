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
    srand(322321);

    int n_frames = 0;
    GameState game;
    initializeRound(&game, 1, 3, 0);
    //Time variables to track elapsed time between updates
    unsigned int realFrameTime, currentTime, elapsedTime, lastTime = get_time_ms();

    renderGame(&game, 1);

    while(1){
        n_frames++;
        //Handle input logic asynchronously
        get_input(&game);
        //Get elapsed time since last update
        currentTime = get_time_ms();
        elapsedTime = currentTime - lastTime;

        //Update the game 
        updateGameState(&game, elapsedTime);

        //Render the game
        renderGame(&game, 0);

        //Control the framerate
        realFrameTime = get_time_ms() - currentTime;
        if(realFrameTime < FRAME_TIME){
            sleep(FRAME_TIME - realFrameTime);
        }
    }
}

void get_input(GameState* game){

    char input = 'w';
    //Some logic should be added here that takes input from 
    //The shared memory region
    processInput(game, input);
}

//WIP: this still needs work
void input_processing()
{
    char * addr = shmat(0, (void *) 0x11C000);

    while (1)
    {
        // Read
    }

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
    else          gameloop();
}