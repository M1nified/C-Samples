#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// #include <ncurses.h>
#include <sys/time.h>
#include <time.h>

enum Action
{
    JUMP,
    NONE
};

int game_loop();
int get_char();

int update_state();
int update_view();

int pid;
enum Action next_action;

static int received = 0;

int main()
{
    game_loop();
}

void receive(int sig)
{
    if(sig == SIGUSR1)
    {
        received = 1;
    }
}

int game_loop()
{
    pid = fork();

    signal(SIGUSR1, receive);

    if(pid > 0)
    {
        while(1)
        {
            get_char();
        }
    }
    else
    {
        while(1)
        {
            while(!received)
            {
                update_view();
            }
            update_state();
            sleep(1);
        }
    }
}

int get_char()
{
    char buff[10];
    // alarm(0);
    fgets(buff, 10, stdin);
    printf("DID READ\n");
    if(buff[0] == '\n')
    {
        printf("JUMP");
        next_action = JUMP;
    }
    else
    {

    }
    printf("%d %d", buff[0], buff[1]);
    kill(pid, SIGUSR1);
    return 1;
}

int tmp = 0;

int update_state()
{
    printf("UPDATE_STATE");
    received = 0;
    if(next_action == JUMP)
    {
        tmp++;
        next_action = NONE;
    }
}

int update_view()
{
    system("clear");
    printf("\n%d\n", tmp);
}