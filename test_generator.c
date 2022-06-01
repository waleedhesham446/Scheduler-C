#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define null 0

struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int memsize;
};

int main(int argc, char * argv[])
{
    FILE * pFile;
    pFile = fopen("processes.txt", "w");
    int no;
    struct processData pData;
    printf("Please enter the number of processes you want to generate: ");
    scanf("%d", &no);
    srand(time(null));
    fprintf(pFile, "#id\tarrival\truntime\tpriority\tmemsize\n");
    pData.arrivaltime = 1;
    for (int i = 1 ; i <= no ; i++)
    {
        pData.id = i;
        pData.arrivaltime += rand() % (11);
        pData.runningtime = rand() % (30);
        pData.priority = rand() % (11);
        pData.memsize = (rand() % (256))+1;
        fprintf(pFile, "%d\t%d\t%d\t%d\t%d\n", pData.id, pData.arrivaltime, pData.runningtime, pData.priority,pData.memsize);
    }
    fclose(pFile);
}
