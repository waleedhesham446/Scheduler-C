#include "headers.h"
#include "priorityQ2.h"

int startTime, currentTime;

void HPF(Process* processes, int n){
    int pid, wpid, status;
    char sruntime[20];
    printf("id \t priority \t runtime \t arrival\n");
    for(int i = 0; i < n; i++){
        currentTime = getClk();
        if(currentTime < (startTime+processes[i].arrival)){
            sleep(startTime+processes[i].arrival-currentTime);
        }
        printf("%d \t %d \t\t %d \t\t %d\n", processes[i].id, processes[i].priority, processes[i].runingTime, processes[i].arrival);
        printf("started at %d\n", getClk());
        pid = fork();
        if(pid == -1) perror("error in fork");
        else if(pid == 0){
            sprintf(sruntime, "%d", processes[i].runingTime);
            execl("./process.out", "./process.out", sruntime, NULL);
        }else{
            // sleep(processes[i].runingTime);
            wpid = wait(&status);
            status = status >> 8;
            printf("finished at %d\n\n", getClk());
        }
    }
}

void SRTN(Process* processes, int n){
    int pid, wpid, status, currentRunningId = -1, nextIndex = 0, currentRunningIdOriginal;
    char sruntime[20];
    for(int i = 0; i < n; i++){PQ[i].runingTime = INT_MAX;}
    while(true){
        printf("\n[%d, %d, %c] - [%d, %d, %c] - [%d, %d, %c]\n", PQ[0].id, PQ[0].pid, PQ[0].status, PQ[1].id, PQ[1].pid, PQ[1].status, PQ[2].id, PQ[2].pid, PQ[2].status);
        currentTime = getClk();
        if(currentTime-startTime == processes[nextIndex].arrival && nextIndex < n){
            PQinsert(processes[nextIndex], processes[nextIndex].runingTime);
            printf("\nINSERT %d AT %d\n", nextIndex, currentTime);
            if(currentRunningId == -1){
                // pid = fork();
                // if(pid == -1) perror("fork failed");
                // else if(pid == 0){
                //     sprintf(sruntime, "%d", processes[nextIndex].runingTime);
                //     execl("./process.out", "./process.out", sruntime, NULL);
                // }else{
                //     printf("\nCREATE-1 %d AT %d\n", pid, currentTime);
                //     currentRunningIdOriginal = pid;
                //     currentRunningId = nextIndex;
                //     processes[currentRunningId].status = 'R';
                //     processes[currentRunningId].pid = pid;
                // }
            }else{
                if(PQ[0].runingTime > processes[nextIndex].runingTime){
                    pid = fork();
                    if(pid == -1) perror("fork failed");
                    else if(pid == 0){
                        sprintf(sruntime, "%d", processes[nextIndex].runingTime);
                        execl("./process.out", "./process.out", sruntime, NULL);
                    }else{
                        printf("\nCREATE-2 %d AT %d\n", pid, currentTime);
                        processes[currentRunningId].status = 'T';
                        kill(currentRunningIdOriginal, SIGSTOP);
                        currentRunningId = nextIndex;
                        processes[currentRunningId].status = 'R';
                        currentRunningIdOriginal = pid;
                        processes[currentRunningId].pid = pid;
                        insertionSort(PQ, n);
                    }
                }
            }
            nextIndex++;
            printf("\nNEXT_INDEX %d\n", nextIndex);
        }
        if(currentRunningId == -1){
            if(nextIndex == 0 || PQ[0].status == 'F'){
                if(nextIndex < n){
                    printf("\nCONTINUE %d\n", nextIndex);
                    sleep(1);
                    continue;
                }else{
                    break;
                }
            }else if(PQ[0].status == 'T'){
                printf("\nCONTINUE %d AT %d\n", PQ[0].pid, currentTime);
                kill(PQ[0].pid, SIGCONT);
                processes[PQ[0].id].status = 'R';
                currentRunningId = PQ[0].id;
                currentRunningIdOriginal = PQ[0].pid;
            }else if(PQ[0].status == 'N'){
                pid = fork();
                if(pid == -1) perror("error in fork");
                else if(pid == 0) {
                    sprintf(sruntime, "%d", PQ[0].runingTime);
                    execl("./process.out", "./process.out", sruntime, NULL);
                }else{
                    printf("\nCREATE-3 %d AT %d\n", pid, currentTime);
                    processes[PQ[0].id].status = 'R';
                    PQ[0].status = 'R';
                    currentRunningId = PQ[0].id;   //
                    processes[PQ[0].id].pid = pid;
                    PQ[0].pid = pid;
                    currentRunningIdOriginal = pid;
                }
            }
        }

        if(currentRunningId != -1){
            printf("\nPARENT STOP AT %d\n", getClk());
            raise(SIGSTOP);
            printf("\nPARENT CONTINUE AT %d\n", getClk());
            processes[currentRunningId].runingTime--;
            PQ[0].runingTime--;
            if(!processes[currentRunningId].runingTime){
                printf("\nFINISHED %d AT %d\n", processes[currentRunningId].pid, getClk()); //  currentRunningId is not index swaping
                processes[currentRunningId].status = 'F';
                PQ[0].status = 'F';
                PQ[0].runingTime = INT_MAX;
                insertionSort(PQ, n);
                currentRunningId = -1;
            }
        }
    }
}

int main(int argc, char * argv[])
{
    initClk();
    //TODO implement the scheduler :)
    int n = 3;
    Process* processes = malloc(sizeof(Process)*n);
    for(int i = 0; i < n; i++){
        processes[i].id = i;
        processes[i].priority = (i) * 2;
        processes[i].runingTime = rand() % 10 +1;
        processes[i].arrival = (i+1) * 2;
        processes[i].status = 'N';
        // totalRemainingTime += processes[i].runingTime;
    }
    //upon termination release the clock resources.
    
    startTime = getClk();
    printf("\nSCHEDULER STARTED AT %d\n", startTime);
    
    // HPF(processes, n);
    SRTN(processes, n);

    printf("\nSCHEDULER TERMINATED AT %d\n", getClk());

    destroyClk(true);
    return 0;
}
