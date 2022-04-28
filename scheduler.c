#include "headers.h"

void HPF(Process* processes, int n){
    for(int i = 0; i < n; i++){
        printf("Process #%d", processes[i].id);
    }
}

int main(int argc, char * argv[])
{
    initClk();
    
    //TODO implement the scheduler :)
    Process* processes = malloc(sizeof(Process*)*3);
    for(int i = 0; i < 3; i++){
        processes[i].id = i;
    }
    // processes[0] = new Process(0, 3, 5, 0);
    // processes[1] = new Process(1, 2, 7, 0);
    // processes[2] = new Process(2, 0, 4, 0);
    //upon termination release the clock resources.
    HPF(processes, 3);

    destroyClk(true);
}
