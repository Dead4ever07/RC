#ifndef __STATISTICS__
#define __STATISTICS__

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define COL_WIDTH 8

typedef struct
{
    int sentPackets;
    int framesSentCounter;
    int framesRecivedCounter;
    int rejectCounter;
    int timeoutCounter;
    struct timeval startTime;
    struct timeval startPacket;
    unsigned long time_taken;
    int sizeOfArray;
    unsigned long *timeOfPackets;

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