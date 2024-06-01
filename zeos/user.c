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
    GameState game;
    initializeRound(&game, 0, 3, 0);

    int n_frames = 0;
    n_frames++;

    renderMap(&game);
}

void input_processing()
{
    // ...
}

void update_fps(int frames)
{
    // ...
}

int __attribute__((__section__(".text.main"))) main(void) {
    // Please modify this function with your desired user.c code

    int pid = fork();

    if (pid == 0) input_processing();
    else gameloop();

    while (1);

}