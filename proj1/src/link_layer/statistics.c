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
    stat.timeOfPackets = malloc(sizeof(unsigned long) * stat.sizeOfArray);
    stat.time_taken = 0;
    gettimeofday(&stat.startTime, NULL);
}

void printStatistics();
void printTable();

void startPacketTrack()
{
    gettimeofday(&stat.startPacket, NULL);
}

void endPacketTrack()
{
    struct timeval end_time;
    gettimeofday(&end_time, NULL);
    stat.timeOfPackets[stat.sentPackets] = (end_time.tv_sec - stat.startPacket.tv_sec)*1000 + (end_time.tv_usec - stat.startPacket.tv_usec)/1000;
    stat.sentPackets++;
    if (stat.sentPackets >= stat.sizeOfArray)
    {
        stat.timeOfPackets = (unsigned long *)realloc(stat.timeOfPackets, stat.sizeOfArray * 2);
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
    struct timeval end_time;
    gettimeofday(&end_time, NULL);
    stat.time_taken =  (end_time.tv_sec-stat.startTime.tv_sec)*1000 + (end_time.tv_usec - stat.startTime.tv_usec)/1000;
    printStatistics();
}

void printStatistics()
{
    printf("Statistics\n");
    printf("Number of packets recived/sent: %d\n", stat.sentPackets);
    printf("Total time: %lu ms\n", stat.time_taken);
    printf("Time taken by each packet:\n");
    printTable();
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

void printCenteredDouble(unsigned long x)
{
    char buffer[COL_WIDTH + 1];
    snprintf(buffer, sizeof(buffer), "%lu", x);
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
        for (int j = 0; j <= COL_WIDTH; j++)
        {
            if (j == COL_WIDTH && i < stat.sentPackets - 1)
                printf("+");
            else
                printf("-");
        }
    }
    printf("\n");

    for (int i = 0; i < stat.sentPackets; i++)
    {
        printCenteredDouble(stat.timeOfPackets[i]);
        if (i < stat.sentPackets - 1)
            printf("|");
    }
    printf("\n");
}
