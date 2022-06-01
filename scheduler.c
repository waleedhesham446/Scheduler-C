#include "headers.h"
#include "priorityQ2.h"
FILE *file;
FILE *file2;
FILE *file3;
float CPU_Utilization;
int totalRunningTime=0;
int totalWaitingTime=0;
float totalWTA=0;
float AVG_Waiting=0;
float AVG_WTA=0;
float STD_WTA=0;
float* arrWTA;
struct msgbuff
{
    long mtype;
    char mtext[70];
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int memsize;
};
struct msgbuff1
{
    long mtype;
    int schNumber;
    int num;
    int quantum;
    int mypid;
};
struct msgbuff message;
int startTime, currentTime;
int rec_val, msgq_id;

void HPF(int n){
    for(int i=0; i<n; i++) PQ[i].priority = INT_MAX;
    int pid, wpid, status;
    char sruntime[20];
    int prevTime = getClk();
    isCurrentRunning = false;
    while(true){
        currentTime = getClk();
        rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext)+sizeof(message.id)*5, 1, IPC_NOWAIT);

        if (rec_val == -1) {
        }
        else{
            printf("\nProcess received : id-> : %d  \n",message.id);
            Process newProcess;
            newProcess.id = message.id;
            newProcess.arrival = message.arrivaltime;
            newProcess.priority = message.priority;
            newProcess.runingTime = message.runningtime;
            newProcess.status = 'N';
            newProcess.memsize = message.memsize;
            totalRunningTime=totalRunningTime+newProcess.runingTime;
            PQinsert(newProcess, newProcess.priority);

            printf("\n%d - %d - %d - %d - %d - %d - %d - %d\n", PQ[0].id, PQ[1].id, PQ[2].id, PQ[3].id, PQ[4].id, PQ[5].id, PQ[6].id, PQ[7].id);
            while(true){
                rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext)+sizeof(message.id)*5, 1, !IPC_NOWAIT);
                if(message.arrivaltime == -1) break;

                printf("\nProcess received : id-> : %d  \n",message.id);
                newProcess.id = message.id;
                newProcess.arrival = message.arrivaltime;
                newProcess.priority = message.priority;
                newProcess.runingTime = message.runningtime;
                newProcess.status = 'N';
                newProcess.memsize = message.memsize;
                totalRunningTime=totalRunningTime+newProcess.runingTime;
                PQinsert(newProcess, newProcess.priority);
                printf("\n%d - %d - %d - %d - %d - %d - %d - %d\n", PQ[0].id, PQ[1].id, PQ[2].id, PQ[3].id, PQ[4].id, PQ[5].id, PQ[6].id, PQ[7].id);
            }
        }

        if(currentTime == prevTime && isCurrentRunning) continue;

        if(!isCurrentRunning){
            if(PQ[0].status == 'F' && PQ[n-1].status == 'F') {
                printf("\n%d - %d - %d - %d - %d - %d - %d - %d\n", PQ[0].id, PQ[1].id, PQ[2].id, PQ[3].id, PQ[4].id, PQ[5].id, PQ[6].id, PQ[7].id);
                break;
            }

            if(PQ[0].status != 'N') continue;
            freeListNode* allocationLimits = allocate(PQ[0].memsize);
            if(allocationLimits == NULL) continue;
            PQ[0].memstart = allocationLimits->start;
            /////////////////////////////////memory allocate////////////////////////////////////////////////
            fprintf(file2, "At time  %d  allocated  %d  bytes for process  %d  from  %d  to  %d \n",currentTime,PQ[0].memsize,PQ[0].id,PQ[0].memstart,allocationLimits->end);
            ////////////////////////////////////////////////////////////////////////////////////////////////
            printf("\nProcess %d forked at %d\n", PQ[0].id, currentTime);
            /////////////////////////////////////////////////////////////////////////////////
            PQ[0].waitingTime=currentTime-PQ[0].arrival;
            PQ[0].totalTime=PQ[0].runingTime;
            fprintf(file, "At time  %d  process  %d  started  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,PQ[0].id,PQ[0].arrival,PQ[0].totalTime,PQ[0].runingTime,PQ[0].waitingTime);
            /////////////////////////////////////////////////////////////////////////////////
            prevTime = currentTime;
            pid = fork();
            if(pid == -1) perror("error in fork");
            else if(pid == 0){
                sprintf(sruntime, "%d", PQ[0].runingTime);
                execl("./process.out", "./process.out", sruntime, NULL);
            }else{
                PQ[0].pid = pid;
                PQ[0].status = 'R';
                isCurrentRunning = true;
            }
        }else if(currentTime != prevTime){
            PQ[0].runingTime--;
            //printf("\nCurrent Time: %d - Remaining: %d\n", getClk(), PQ[0].runingTime);
            prevTime = currentTime;
            if(!PQ[0].runingTime){
                kill(PQ[0].pid, SIGUSR1);
                wpid = wait(&status);
                status = status >> 8;
                ///////////////////////////////////////////////////////////////////////
                int TA=currentTime-PQ[0].arrival;
                float WTA =(float)TA/PQ[0].totalTime;
                arrWTA[PQ[0].id-1]=WTA;
                totalWTA=totalWTA+WTA;
                fprintf(file, "At time  %d  process  %d  finished  arr  %d  total  %d  remain  %d  wait  %d  TA  %d  WTA  %.2f \n",currentTime,PQ[0].id,PQ[0].arrival,PQ[0].totalTime,PQ[0].runingTime,PQ[0].waitingTime,TA,WTA);
                ///////////////////////////////////////////////////////////////////////
                printf("finished at %d\n\n", getClk());
                freeListNode* allocationLimits2=deallocate(PQ[0].memstart, PQ[0].memsize);
                /////////////////////////////////memory deallocate////////////////////////////////////////////////
                fprintf(file2, "At time  %d  freed  %d  bytes from process  %d  from  %d  to  %d \n",currentTime,PQ[0].memsize,PQ[0].id,PQ[0].memstart,allocationLimits2->end);
                /////////////////////////////////////////////////////////////////////////////////////////////////
                isCurrentRunning = false;
                PQ[0].status = 'F';
                PQ[0].priority = INT_MAX;
                printf("\nBefore: %d - %d - %d - %d - %d - %d - %d - %d\n", PQ[0].id, PQ[1].id, PQ[2].id, PQ[3].id, PQ[4].id, PQ[5].id, PQ[6].id, PQ[7].id);
                insertionSortHPF(PQ, n);
                printf("\nAfter%d - %d - %d - %d - %d - %d - %d - %d\n", PQ[0].id, PQ[1].id, PQ[2].id, PQ[3].id, PQ[4].id, PQ[5].id, PQ[6].id, PQ[7].id);
            }
        }
    }
}

void SRTN(int n){
    for(int i=0; i<n; i++) waitingQ[i].runingTime = INT_MAX;
    int pid, wpid, status, currentRunningId = -1, nextIndex = 0, currentRunningIdOriginal;
    char sruntime[20];
    Process  RunProcess;
    RunProcess.id = 0;
    for(int i = 0; i < n; i++){PQ[i].runingTime = INT_MAX;}
    int prevTime = getClk();
    while(true){
        int waitingIndex = 0;
        while(waitingIndex < n){
            if(waitingQ[waitingIndex].status == 'W'){
                freeListNode* allocationLimits = allocate(waitingQ[waitingIndex].memsize);
                if(allocationLimits != NULL){
                    waitingQ[waitingIndex].status = 'N';
                    waitingQ[waitingIndex].memstart = allocationLimits->start;
                    PQinsert2(waitingQ[waitingIndex], waitingQ[waitingIndex].runingTime);
                    waitingQ[waitingIndex].runingTime = INT_MAX;
                    printf("\n%d - %d - %d - %d - %d\n", PQ[0].id, PQ[1].id, PQ[2].id, PQ[3].id, PQ[4].id);
                }
            }
            waitingIndex++;
        }
        insertionSort(waitingQ, n);

        currentTime = getClk();
        rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext)+sizeof(message.id)*5, 0, IPC_NOWAIT);
        if (rec_val == -1){
        }
        else{
            printf("\nProcess received : id-> : %d  \n",message.id);
            Process newProcess;
            newProcess.id = message.id;
            newProcess.arrival = message.arrivaltime;
            newProcess.priority = message.priority;
            newProcess.runingTime = message.runningtime;
            newProcess.status = 'W';
            newProcess.memsize = message.memsize;
            totalRunningTime=totalRunningTime+newProcess.runingTime;
            PQinsertWaiting(newProcess, newProcess.runingTime);

            while(true){
                rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext)+sizeof(message.id)*5, 1, !IPC_NOWAIT);
                if(message.arrivaltime == -1) break;

                printf("\nProcess received : id-> : %d  \n",message.id);
                newProcess.id = message.id;
                newProcess.arrival = message.arrivaltime;
                newProcess.priority = message.priority;
                newProcess.runingTime = message.runningtime;
                newProcess.status = 'W';
                newProcess.memsize = message.memsize;
                totalRunningTime=totalRunningTime+newProcess.runingTime;
                PQinsertWaiting(newProcess, newProcess.runingTime);

            }

            int waitingIndex = 0;
            while(waitingIndex < n){
                //printf("\nindex: %d\n", waitingIndex);
                if(waitingQ[waitingIndex].status == 'W'){
                    freeListNode* allocationLimits = allocate(waitingQ[waitingIndex].memsize);
                    if(allocationLimits != NULL){
                        printf("\nProcess %d Allocateed, start: %d, end: %d\n", waitingQ[waitingIndex].id, allocationLimits->start, allocationLimits->end);
                        /////////////////////////////////memory allocate////////////////////////////////////////////////
                        fprintf(file2, "At time  %d  allocated  %d  bytes for process  %d  from  %d  to  %d \n",currentTime,waitingQ[waitingIndex].memsize,waitingQ[waitingIndex].id,allocationLimits->start,allocationLimits->end);
                        ////////////////////////////////////////////////////////////////////////////////////////////////
                        waitingQ[waitingIndex].status = 'N';
                        waitingQ[waitingIndex].memstart = allocationLimits->start;
                        PQinsert2(waitingQ[waitingIndex], waitingQ[waitingIndex].runingTime);
                        waitingQ[waitingIndex].runingTime = INT_MAX;

                        printf("\n%d : %d - %d : %d - %d - %d - %d\n", PQ[0].id, PQ[0].runingTime, PQ[1].id, PQ[1].runingTime, PQ[2].id, PQ[3].id, PQ[4].id);
                    }
                }
                waitingIndex++;
            }
            insertionSort(waitingQ, n);

            // preemption
            if(RunProcess.id != 0 && RunProcess.id != PQ[0].id){
                    printf("\nProcprev %d SWAP PROC id %d    \n",RunProcess.id,PQ[0].id);
                    kill(RunProcess.pid, SIGSTOP);
                    for (int i = 0; i < n; i++){
                        if(RunProcess.id == PQ[i].id){
                            PQ[i].status = 'T';
                            /////////////////////////////////////////////////////////////////////
                            PQ[i].lastStop=currentTime;
                            fprintf(file, "At time  %d  process  %d  stopped  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,PQ[i].id,PQ[i].arrival,PQ[i].totalTime,PQ[i].runingTime,PQ[i].waitingTime);
                            /////////////////////////////////////////////////////////////////////
                            break;
                        }
                    }
                    RunProcess.id=0;
                }
        }

        if(currentTime == prevTime && RunProcess.id != 0) continue;
                
        if(RunProcess.id == 0){
            
            if(PQ[0].status == 'F' && PQ[n-1].status == 'F') break;
                        
            if(PQ[0].status == 'N'){
                ///////////////////////////////////////////////////////////////////////
                PQ[0].waitingTime=currentTime-PQ[0].arrival;
                PQ[0].totalTime=PQ[0].runingTime;
                fprintf(file, "At time  %d  process  %d  started  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,PQ[0].id,PQ[0].arrival,PQ[0].totalTime,PQ[0].runingTime,PQ[0].waitingTime);
                ///////////////////////////////////////////////////////////////////////
                pid = fork();
                if(pid == -1) perror("error in fork");
                else if(pid == 0){
                    sprintf(sruntime, "%d", PQ[0].runingTime);
                    execl("./process.out", "./process.out", sruntime, NULL);
                }else{
                    printf("\nProcess %d Start at %d\n", pid, getClk());
                    currentRunningId=PQ[0].pid;
                    PQ[0].pid = pid;
                    PQ[0].status = 'R';
                    RunProcess.id = PQ[0].id;
                    RunProcess.pid = pid;
                    prevTime = currentTime;
                }
            }else if(PQ[0].status == 'T'){
                printf("\nProcess %d Continue at %d\n", PQ[0].pid, getClk());
                currentRunningId = PQ[0].pid;
                PQ[0].status = 'R';
                ////////////////////////////////////////////////////////////////////////
                PQ[0].waitingTime=PQ[0].waitingTime+(currentTime-PQ[0].lastStop);
                fprintf(file, "At time  %d  process  %d  resumed  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,PQ[0].id,PQ[0].arrival,PQ[0].totalTime,PQ[0].runingTime,PQ[0].waitingTime);
                ////////////////////////////////////////////////////////////////////////
                kill(PQ[0].pid,SIGCONT);
                RunProcess.id = PQ[0].id;
                prevTime = currentTime;
            }
        }

        if(prevTime != currentTime){
            if(RunProcess.id != 0){
                PQ[0].runingTime--;
                //printf("\nRT: %d - %d\n", PQ[0].runingTime, getClk());
                prevTime = currentTime;
                if(!PQ[0].runingTime){
                    kill(PQ[0].pid, SIGUSR1);
                    wpid = wait(&status);
                    status = status >> 8;
                    printf("\nFinished at %d\n", getClk());
                    ///////////////////////////////////////////////////////////////////////
                    int TA=currentTime-PQ[0].arrival;
                    float WTA =(float)TA/PQ[0].totalTime;
                    arrWTA[PQ[0].id-1]=WTA;
                    totalWTA=totalWTA+WTA;
                    fprintf(file, "At time  %d  process  %d  finished  arr  %d  total  %d  remain  %d  wait  %d  TA  %d  WTA  %.2f \n",currentTime,PQ[0].id,PQ[0].arrival,PQ[0].totalTime,PQ[0].runingTime,PQ[0].waitingTime,TA,WTA);
                    ///////////////////////////////////////////////////////////////////////
                    RunProcess.id = 0;
                    PQ[0].status = 'F';
                    PQ[0].runingTime = INT_MAX;
                    freeListNode* allocationLimits2=deallocate(PQ[0].memstart, PQ[0].memsize);
                    printf("\nDeallocation (%d): start=%d, size=%d\n", PQ[0].id, PQ[0].memstart, PQ[0].memsize);
                    /////////////////////////////////memory deallocate////////////////////////////////////////////////
                    fprintf(file2, "At time  %d  freed  %d  bytes from process  %d  from  %d  to  %d \n",currentTime,PQ[0].memsize,PQ[0].id,PQ[0].memstart,allocationLimits2->end);
                    /////////////////////////////////////////////////////////////////////////////////////////////////
                    insertionSort(PQ, n);
                }
            }
        }
    }
}

void RR(int n, int Q){
    for(int i=0; i<n; i++) waitingQ[i].runingTime = INT_MAX;
    int  pid, wpid, status, currentRunningId = -1, nextIndex = 0, currentRunningIdOriginal, actualQ, spentTime = 0;
    char sruntime[20];
    struct node* head= NULL ;
    struct node *tail = NULL;
    struct node *currentRunning = NULL;
    struct node *previousProcess = NULL;
    int prevTime = getClk();
    while(true){


        int waitingIndex = 0;
        while(waitingIndex < n){
            if(waitingQ[waitingIndex].status == 'W'){
                freeListNode* allocationLimits = allocate(waitingQ[waitingIndex].memsize);
                if(allocationLimits != NULL){
                    waitingQ[waitingIndex].status = 'N';
                    waitingQ[waitingIndex].memstart = allocationLimits->start;


                    struct node *newProcess = malloc(sizeof(node));
                    Process* newProcess2 = malloc(sizeof(Process));
                    *newProcess2 = waitingQ[waitingIndex];
                    newProcess->data = newProcess2;
                    if (!head) {
                        head = newProcess;
                        tail = newProcess;
                        head->next = tail;
                        tail->next = head;
                        currentRunning=head;
                    }else {
                        tail->next = newProcess;
                        tail = newProcess;
                        tail->next = head;
                    }

                    waitingQ[waitingIndex].runingTime = INT_MAX;

                    struct node *newProcessBB = head;
                    printf("\n");
                    printf("%d    ", newProcessBB->data->id);
                    newProcessBB = newProcessBB->next;
                    while(newProcessBB != head){
                        printf("%d    ", newProcessBB->data->id);
                        newProcessBB = newProcessBB->next;
                    }
                    printf("\n");
                    printf("tail1 %d\n", tail->data->id);
                }
            }
            waitingIndex++;
        }
        // insertionSort(waitingQ, n);

        currentTime = getClk();
        rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext) + sizeof(message.id) * 5, 0, IPC_NOWAIT);
        if (rec_val == -1)
        {
        }
        else
        {
            // printf("\nPROCESS %d CREATED AT %d\n", processes[nextIndex].id, getClk());
            printf("\nMessage received: %s : %d  \n", message.mtext, message.id);
            Process*newProcess2=malloc(sizeof(Process));
            newProcess2->id = message.id;
            newProcess2->arrival = message.arrivaltime;
            newProcess2->priority = message.priority;
            newProcess2->runingTime = message.runningtime;
            newProcess2->status = 'W';
            newProcess2->memsize = message.memsize;
            totalRunningTime=totalRunningTime+newProcess2->runingTime;
            // PQinsertWaiting(*newProcess2, newProcess2->runingTime);
            waitingQ[waitingFront++] = *newProcess2;

            nextIndex++;


            while(true){
                rec_val = msgrcv(msgq_id, &message, sizeof(message.mtext)+sizeof(message.id)*5, 1, !IPC_NOWAIT);
                if(message.arrivaltime == -1) break;

                printf("\nMessage received: %s : %d  \n", message.mtext,message.id);
                Process* newProcess3 = malloc(sizeof(Process));
                newProcess3->id = message.id;
                newProcess3->arrival = message.arrivaltime;
                newProcess3->priority = message.priority;
                newProcess3->runingTime = message.runningtime;
                newProcess3->status = 'W';
                newProcess3->memsize = message.memsize;
                totalRunningTime=totalRunningTime+newProcess3->runingTime;
                nextIndex++;
                // struct node *newProcess4 = malloc(sizeof(node));
                // newProcess4->data = newProcess3;
                // tail->next = newProcess4;
                // tail = newProcess4;
                // tail->next = head;
                // PQinsertWaiting(*newProcess3, newProcess3->runingTime);
                waitingQ[waitingFront++] = *newProcess3;
            }

            int waitingIndex = 0;
            while(waitingIndex < n){
                printf("%d ", waitingIndex);
                if(waitingQ[waitingIndex].status == 'W'){
                    printf("\nWAITING\n");
                    freeListNode* allocationLimits = allocate(waitingQ[waitingIndex].memsize);
                    if(allocationLimits != NULL){
                        /////////////////////////////////memory allocate////////////////////////////////////////////////
                        fprintf(file2, "At time  %d  allocated  %d  bytes for process  %d  from  %d  to  %d \n",currentTime,waitingQ[waitingIndex].memsize,waitingQ[waitingIndex].id,allocationLimits->start,allocationLimits->end);
                        ////////////////////////////////////////////////////////////////////////////////////////////////
                        waitingQ[waitingIndex].status = 'N';
                        waitingQ[waitingIndex].memstart = allocationLimits->start;


                        struct node *newProcess = malloc(sizeof(node));
                        newProcess2 = malloc(sizeof(Process));
                        *newProcess2 = waitingQ[waitingIndex];
                        newProcess->data = newProcess2;
                        if (!head) {
                            head = newProcess;
                            tail = newProcess;
                            head->next = tail;
                            tail->next = head;
                            currentRunning=head;
                        }else {
                            tail->next = newProcess;
                            tail = newProcess;
                            tail->next = head;
                        }
                        waitingQ[waitingIndex].runingTime = INT_MAX;

                        struct node *newProcessBB = head;
                        printf("\n");
                        printf("%d    ", newProcessBB->data->id);
                        newProcessBB = newProcessBB->next;
                        while(newProcessBB != head){
                            printf("%d    ", newProcessBB->data->id);
                            newProcessBB = newProcessBB->next;
                        }
                        printf("\n");
                        printf("tail2 %d\n", tail->data->id);
                    }
                }
                waitingIndex++;
            }
            // insertionSort(waitingQ, n);
            printf("\nfffffffffff\n");
        }
        if(!head){
            if (nextIndex == n) break;
            continue;
        }

        actualQ = Q;
        if(currentRunning->data->runingTime < Q) actualQ = currentRunning->data->runingTime;

        if(currentRunning->data->status == 'N'){
            //////////////////////////////////////////////////////////////////////
            currentRunning->data->waitingTime=currentTime-currentRunning->data->arrival;
            currentRunning->data->totalTime=currentRunning->data->runingTime;
            fprintf(file, "At time  %d  process  %d  started  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,currentRunning->data->id,currentRunning->data->arrival,currentRunning->data->totalTime,currentRunning->data->runingTime,currentRunning->data->waitingTime);
            //////////////////////////////////////////////////////////////////////
            pid = fork();
            if(pid == -1) perror("error in fork");
            else if(pid == 0) {
                sprintf(sruntime, "%d", currentRunning->data->runingTime);
                execl("./process.out", "./process.out", sruntime, NULL);
            }else{
                currentRunning->data->pid = pid;
                printf("\nPROCESS %d START AT %d\n", currentRunning->data->id,getClk());
                currentRunning->data->status = 'R';
                spentTime = 0;
                prevTime=currentTime;
            }
        }else if(currentRunning->data->status == 'T'){
            kill(currentRunning->data->pid, SIGCONT);
            printf("\nPROCESS %d CONTINUE AT %d\n", currentRunning->data->id, getClk());
            ////////////////////////////////////////////////////////////////////////
            currentRunning->data->waitingTime=currentRunning->data->waitingTime+(currentTime-currentRunning->data->lastStop);
            fprintf(file, "At time  %d  process  %d  resumed  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,currentRunning->data->id,currentRunning->data->arrival,currentRunning->data->totalTime,currentRunning->data->runingTime,currentRunning->data->waitingTime);
            ////////////////////////////////////////////////////////////////////////
        currentRunning->data->status = 'R';
        spentTime = 0;
        }

        if (prevTime==currentTime)
        {
            continue;
        }
        else{
            spentTime++;
            prevTime=currentTime;
        }

        if(spentTime != actualQ){
            continue;
        }

        currentRunning->data->status = 'T';

        printf("\nPROCESS %d STOP AT %d\n", currentRunning->data->id, getClk());
        /////////////////////////////////////////////////////////////////////
        currentRunning->data->lastStop=currentTime;
        int tempRemain=currentRunning->data->runingTime - actualQ;
        if(tempRemain > 0)
        {
            fprintf(file, "At time  %d  process  %d  stopped  arr  %d  total  %d  remain  %d  wait  %d \n",currentTime,currentRunning->data->id,currentRunning->data->arrival,currentRunning->data->totalTime,tempRemain,currentRunning->data->waitingTime);
        }
        /////////////////////////////////////////////////////////////////////

        kill(currentRunning->data->pid, SIGSTOP);
        currentRunning->data->runingTime -= actualQ;

        if(!currentRunning->data->runingTime){
            printf("\nFINISHED %d AT %d\n", currentRunning->data->pid, getClk());
            ///////////////////////////////////////////////////////////////////////
            int TA=currentTime-currentRunning->data->arrival;
            float WTA =(float)TA/currentRunning->data->totalTime;
            arrWTA[currentRunning->data->id-1]=WTA;
            totalWTA=totalWTA+WTA;
            fprintf(file, "At time  %d  process  %d  finished  arr  %d  total  %d  remain  %d  wait  %d  TA  %d  WTA  %.2f \n",currentTime,currentRunning->data->id,currentRunning->data->arrival,currentRunning->data->totalTime,currentRunning->data->runingTime,currentRunning->data->waitingTime,TA,WTA);
            ///////////////////////////////////////////////////////////////////////
            freeListNode* allocationLimits2= deallocate(currentRunning->data->memstart, currentRunning->data->memsize);
            printf("\nDeallocation (%d): start=%d, size=%d\n", currentRunning->data->id, currentRunning->data->memstart, currentRunning->data->memsize);
            /////////////////////////////////memory deallocate////////////////////////////////////////////////
            fprintf(file2, "At time  %d  freed  %d  bytes from process  %d  from  %d  to  %d \n",currentTime,currentRunning->data->memsize,currentRunning->data->id,currentRunning->data->memstart,allocationLimits2->end);
            /////////////////////////////////////////////////////////////////////////////////////////////////
            currentRunning->data->status = 'F';
            kill(currentRunning->data->pid, SIGUSR1);

            struct node *newProcessAA = head;
            printf("\n");
            printf("%d    ", newProcessAA->data->id);
            newProcessAA = newProcessAA->next;
            while(newProcessAA != head){
                printf("%d    ", newProcessAA->data->id);
                newProcessAA = newProcessAA->next;
            }
            printf("\n");
            printf("\n%d\n", currentRunning->next->data->id);
            if(currentRunning == head){
                printf("%d -------------- %d\n", head->data->id, tail->data->id);
                previousProcess = tail;
                head = head->next;
                tail->next = head->next;
            }
            if(currentRunning == tail){
                tail = previousProcess;
                tail->next = head;
            }
            if(previousProcess&&previousProcess!=currentRunning) previousProcess->next = currentRunning->next;
            else{
                head=NULL;
                printf("\n head=null\n");
            }
            if(currentRunning == currentRunning->next){
                head=NULL;
                printf("\n head=null\n");
            }
        }
        if(currentRunning->data->runingTime) previousProcess = currentRunning;
        currentRunning = currentRunning->next;
       // if(currentRunning == currentRunning->next && currentRunning->data->status == 'F') head = NULL;
    }
}

int main(int argc, char * argv[])
{
    initClk();

    Buddy(1024);
    file = fopen("scheduler.log", "w");
    file2=fopen("memory.log", "w");
    file3=fopen("scheduler.perf", "w");
    fprintf(file, "#At time  x  process  y  state  arr  w  total  z  remain  y  wait  k \n");
    fprintf(file2, "#At time  x  allocated  y  bytes for process  z  from  i  to  j \n");
    key_t key_id,key_id2;
    int rec_val2,msgq_id2;
    key_id = ftok("keyfile", 65);              
    key_id2 = ftok("keyfile", 66);             
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    msgq_id2 = msgget(key_id2, 0666 | IPC_CREAT);

    if (msgq_id == -1 || msgq_id2==-1)
    {
        perror("Error in create");
        exit(-1);
    }
    struct msgbuff1 message1;
    rec_val2 = msgrcv(msgq_id2, &message1, sizeof(message1.mypid)*4, 2, !IPC_NOWAIT);
    if(rec_val2 == -1) perror("Error in receive");
    else{
    }

    printf("Message Queue ID = %d\n", msgq_id);

    int n = message1.num;

    PQ = malloc(sizeof(Process)*n);
    waitingQ = malloc(sizeof(Process)*n);
    arrWTA=malloc(sizeof(float)*n);
    startTime = getClk();
    printf("\nSCHEDULER STARTED AT %d\n", startTime);
    if(message1.schNumber == 1)
    {
        HPF(message1.num);
    }
    else if(message1.schNumber == 2)
    {
        printf("\nSRTN\n");
        SRTN(message1.num);
    }
    else
    {
        RR(message1.num,message1.quantum);
    }
    CPU_Utilization=((float)totalRunningTime/(getClk()-startTime))*100;
    totalWaitingTime=getClk()-startTime-totalRunningTime;
    AVG_Waiting=(float)totalWaitingTime/n;
    AVG_WTA=(float)totalWTA/n;
    float sumSquares=0;
    for (int i=0; i<n; i++)
    {
        sumSquares=sumSquares+pow(arrWTA[i]-AVG_WTA,2);
    }
    STD_WTA=sqrt(sumSquares/n);
    fprintf(file3,"CPU utilization = %d%%\n",(int)CPU_Utilization);
    fprintf(file3,"AVG WTA = %.2f \n",AVG_WTA);
    fprintf(file3,"AVG Waiting = %.2f \n",AVG_Waiting);
    fprintf(file3,"Std WTA = %.2f \n",STD_WTA);
    printf("\nSCHEDULER TERMINATED AT %d\n", getClk());
    kill(message1.mypid,SIGINT);
    return 0;
}
