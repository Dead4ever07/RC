#include "statistics.h"


void startStatistics(int expectedPackets){
    stat.expectedPackets = expectedPackets;
    stat.rejectCounter = 0;
    stat.framesRecivedCounter = 0;
    stat.framesSentCounter = 0;
    stat.sentPackets = 0;
    stat.timeoutCounter = 0;   
    stat.timeOfPackets = (clock_t*) malloc(expectedPackets*sizeof(clock_t));
    stat.startTime = clock();
}

void startPacketTrack(){
    printf("Status: %d%%\n", (stat.sentPackets/stat.expectedPackets)*100);
    stat.timeOfPackets[stat.sentPackets] = clock();
}

void endPacketTrack(){
    stat.timeOfPackets[stat.sentPackets] = clock() - stat.timeOfPackets[stat.sentPackets];
    stat.sentPackets++; 
}


/// @brief increments the number of frame recive/sent
/// @param sent a boolean value telling the function if the frame is being sent or recived
void frameIncrement(int sent){
    if(sent){
        stat.framesSentCounter++;
    }else{
        stat.framesRecivedCounter++;
    }
}

void rejectIncrement(){
    stat.rejectCounter++;
}

void printStatistics(int Rx){
    if(Rx){
        printf("Statistics of Reciver:\n");
        
    }
}

void endStatistics(int Rx){
    stat.endTime = clock();
    printStatistics(Rx);
}

