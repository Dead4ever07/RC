#include "statistics.h"
Statistics stat;

void startStatistics()
{
    stat.rejectCounter = 0;
    stat.framesRecivedCounter = 0;
    stat.framesSentCounter = 0;
    stat.sentPackets = 0;
    stat.timeoutCounter = 0;
    stat.sizeOfArray = 100;
    stat.timeOfPackets = malloc(sizeof(clock_t) * stat.sizeOfArray);
    stat.startTime = clock();
}

void printStatistics();
void printTable();

void startPacketTrack()
{
    stat.timeOfPackets[stat.sentPackets] = clock();
}

void endPacketTrack()
{
    stat.timeOfPackets[stat.sentPackets] = clock() - stat.timeOfPackets[stat.sentPackets];
    stat.sentPackets++;
    if (stat.sentPackets >= stat.sizeOfArray)
    {
        stat.timeOfPackets = (clock_t *)realloc(stat.timeOfPackets, stat.sizeOfArray * 2);
    }
}

void framesSentIncrement()
{
    stat.framesSentCounter++;
}

void framesRecivedIncrement()
{
    stat.framesRecivedCounter++;
}

void rejectIncrement()
{
    stat.rejectCounter++;
}

void timeOutIncrement()
{
    stat.timeoutCounter++;
}

void endStatistics()
{
    printf("\n");
    
    stat.endTime = clock();
    printStatistics();
}

void printStatistics()
{
    double totalTime = ((double)(stat.endTime - stat.startTime)) / CLOCKS_PER_SEC;
    printf("Statistics\n");
    printf("Number of packets recived/sent: %d\n", stat.sentPackets);
    printf("Total time: %.02f\n", totalTime);
    printf("Time taken by each packet:\n");
    //printTable();
    printf("Number of frames sent: %d\n", stat.framesSentCounter);
    printf("Number of frames recived: %d\n", stat.framesRecivedCounter);
    printf("Number of timeouts: %d\n", stat.timeoutCounter);
    printf("Number of reject frames: %d\n", stat.rejectCounter);
}

void printCenteredInt(int x)
{
    char buffer[COL_WIDTH + 1];
    snprintf(buffer, sizeof(buffer), "%d", x);
    int padding = (COL_WIDTH - strlen(buffer)) / 2;
    printf("%*s%s%*s",
           (int)padding, "",
           buffer,
           (int)(COL_WIDTH - padding - strlen(buffer)), "");
}

void printCenteredDouble(double x)
{
    char buffer[COL_WIDTH + 1];
    snprintf(buffer, sizeof(buffer), "%.2f", x);
    int padding = (COL_WIDTH - strlen(buffer)) / 2;
    printf("%*s%s%*s",
           (int)padding, "",
           buffer,
           (int)(COL_WIDTH - padding - strlen(buffer)), "");
}

void printTable()
{
    for (int i = 1; i <= stat.sentPackets; i++)
    {
        printCenteredInt(i);
        if (i < stat.sentPackets)
            printf("|");
    }
    printf("\n");

    for (int i = 0; i < stat.sentPackets; i++)
    {
        for (int j = 0; j < COL_WIDTH; j++)
        {
            if (j == COL_WIDTH - 1 && i < stat.sentPackets - 1)
                printf("+");
            else
                printf("-");
        }
    }
    printf("\n");

    for (int i = 0; i < stat.sentPackets; i++)
    {
        double t = ((double)stat.timeOfPackets[i]) / CLOCKS_PER_SEC;
        printCenteredDouble(t);
        if (i < stat.sentPackets - 1)
            printf("|");
    }
    printf("\n");
}
