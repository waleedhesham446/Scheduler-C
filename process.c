#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

// void handleRemainingTime(){
//     remainingtime--;
// }

int main(int agrc, char * argv[])
{
    initClk();
    
    // signal(SIGUSR1, handleRemainingTime);
    //TODO it needs to get the remaining time from somewhere
    remainingtime = atoi(argv[1]);
    printf("\nrunning time: %d\n", remainingtime);
    while (remainingtime > 0)
    {
        // remainingtime = ??;
        sleep(1);
        kill(getppid(), SIGCONT);
        remainingtime--;
    }
    printf("\nProcess %d Terminated\n", getpid());
    // kill(getppid(), SIGCONT);
    destroyClk(false);
    
    return 0;
}
