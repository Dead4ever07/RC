#ifndef __STATISTICS__
#define __STATISTICS__

#include <time.h>
#include <stdio.h> 
#include <stdlib.h>

typedef struct
{
    int expectedPackets;
    int sentPackets;
    int framesSentCounter;
    int framesRecivedCounter;
    int rejectCounter;
    int timeoutCounter;
    clock_t startTime;
    clock_t endTime;
    clock_t* timeOfPackets;

} Statistics;

Statistics stat;

void startStatistics(int expectedPackets);

/// @brief function responsible to register the start of a packet
void startPacketTrack();

/// @brief function responsible to register the end of a packet
void endPacketTrack();

/// @brief increments the number of frame recive/sent
/// @param sent a boolean value telling the function if the frame is being sent or recived
void frameIncrement(int sent);

/// @brief increments the number of reject frames sent
void rejectIncrement();

/// @brief Ends the track of statistics and prints the statistics collected
/// @param Rx Bolean value representing if the user is the reciver or not
void endStatistics(int Rx);



#endif