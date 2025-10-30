#ifndef __STATISTICS__
#define __STATISTICS__

#include <time.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#define COL_WIDTH 8

typedef struct
{
    int sentPackets;
    int framesSentCounter;
    int framesRecivedCounter;
    int rejectCounter;
    int timeoutCounter;
    clock_t startTime;
    int sizeOfArray;
    clock_t endTime;
    clock_t* timeOfPackets;

} Statistics;


void startStatistics();

/// @brief function responsible to register the start of a packet
void startPacketTrack();

/// @brief function responsible to register the end of a packet
void endPacketTrack();

/// @brief increments the number of frames sent
void framesSentIncrement();

/// @brief increments the number of frames recived
void framesRecivedIncrement();


/// @brief increments the number of timeouts occured
void timeOutIncrement();


/// @brief increments the number of reject frames sent
void rejectIncrement();


/// @brief Ends the track of statistics and prints the statistics collected
/// @param Rx Bolean value representing if the user is the reciver or not
void endStatistics();



#endif