#include <colors.h>
#include <libc.h>
#include <list.h>
#include <pacman.h>

#define TICKS_PER_FRAME 250

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
    initializeRound(&game, 1, 3, 0);

    //Time variables to track elapsed time between updates
    int currentTime, elapsedTime, lastTime = get_time_ms();

    renderGame(&game, 1);

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

        //Render the game
        renderGame(&game, 0);

        while (gettime() - prevTimer < TICKS_PER_FRAME);
        update_fps(gettime() - prevTimer);

        prevTimer = gettime();
    }
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

void update_fps(int ticks)
{
    // 400 ticks equals approximately 1 second
    char buf[10] = "FPS:    ";

    int fps = 400 / ticks;
    
    itodeca(fps, &buf[5]);
    print(buf);
}

int __attribute__((__section__(".text.main"))) main(void) {
    // Please modify this function with your desired user.c code
    int pid = fork();

    if (pid == 0) input_processing();
    else          gameloop();

}