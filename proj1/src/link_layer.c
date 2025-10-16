// Link layer protocol implementation

#include "link_layer.h"
#include "serial_port.h"
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // Added for alarm()

// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source

LinkLayer config;

int alarmEnabled = FALSE;
int alarmCount = 0;

// Alarm function handler.
// This function will run whenever the signal SIGALRM is received.
void alarmHandler(int signal)
{
    alarmEnabled = FALSE;
    alarmCount++;

    printf("Alarm #%d received\n", alarmCount);
}

int sendFrame(unsigned char *bytes, int nBytes, unsigned char *ackByte);
int readByteWithAlarm(unsigned char *byte);
int readBytesAndCompare(unsigned char *ackRef);
int sendDiscCommand(unsigned char *discCommand);

////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{
    config = connectionParameters;
    // Open port
    if (openSerialPort(config.serialPort, config.baudRate) == -1)
    {
        perror("Error opening the Serial Port.");
        return -1;
    }
    // Start alarm signals
    struct sigaction act = {0};
    act.sa_handler = &alarmHandler;
    if (sigaction(SIGALRM, &act, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    if (connectionParameters.role == LlTx)
    {  
        unsigned char setCommand[] = SET_COMMAND;
        unsigned char uaCommand[] = UA_COMMAND;
        if(sendFrame(setCommand, 4, uaCommand) != 0)
        {
            return -1;
        }

    }else{
        unsigned char setCommand[] = SET_COMMAND;
        unsigned char uaCommand[] = UA_COMMAND;
        if (readBytesAndCompare(setCommand) != 0)
        {
            return -1;
        }

        if (writeBytesSerialPort(uaCommand, COMMAND_SIZE) != COMMAND_SIZE)
        {
            printf("Error sending the UA command through the serial port.\n");
            return -1;
        }
        
    }

    return 0;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int llwrite(const unsigned char *buf, int bufSize)
{
    return 0;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{
    /*
    llread cookbook:
        - receive first 4 bytes {FLAG,ADRSS,CTRL,BCC}
        - receive each byte from serial port until it's a FLAG 
        - BCC2 is the last byte execpt from FLAG
        - To process the BCC2 the only thing you need to do is Xor d1,d2,d3... BCC2
        and compare the result with 0.
        - process the bytes checking the BCC1 
        - If every thing right send a package with the RR ctrl else REJ.
        - If the frame is sent again with the same number just discard the last one
    */
    return 0;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose()
{
    if(config.role == LlRx)
    {
        unsigned char discCommand[] = SENDER_DISC_COMMAND;

        if (sendDiscCommand(discCommand) != 0){
            return -1;
        }
    }else {
        unsigned char discCommand[] = RECEIVER_DISC_COMMAND;

        if (sendDiscCommand(discCommand) != 0){
            return -1;
        }
    }
    return 0;
}

/// @brief Responsible to the sending the disc command and the ua response
/// @param discCommand it depends on the role
/// @return 0 if nothing went wrong
int sendDiscCommand(unsigned char *discCommand){

    unsigned char uaCommand[] = UA_COMMAND;

    if(sendFrame(discCommand, 4, discCommand) != 0)
    {
        return -1;
    }
    if (writeBytesSerialPort(uaCommand, COMMAND_SIZE) != COMMAND_SIZE)
    {
        printf("Error sending the UA command through the serial port.\n");
        return 1;
    }

    if (closeSerialPort() != 0)
    {
        printf("Error closing the Serial port\n");
        return -1;
    }
    return 0;
}




/// @brief Responsible to the sent of a package
/// @param bytes 
/// @param nBytes 
/// @param ackByte 
/// @return 
int sendFrame(unsigned char *bytes, int nBytes, unsigned char *ackByte)
{
    int try = 0;
    while (try<config.nRetransmissions)
    {
        try++;
        if (writeBytesSerialPort(bytes, nBytes) != nBytes)
        {
            continue;
        }     
        if (readBytesAndCompare(ackByte) != 0)
        {
            continue;
        }else{
            return 0;
        }
    }
    printf("Coudn't send Frame in the nRetransmissions\n");
    return -1;
}

/// @brief
/// @param ackRef
/// @return returns 0 if successfull
int readBytesAndCompare(unsigned char *bytesRef)
{
    int pos = 0;  // Changed from char to int
    unsigned char buff[COMMAND_SIZE];
    while (pos < COMMAND_SIZE)
    {
        if (readByteWithAlarm(&buff[pos]) != 1 || buff[pos] != bytesRef[pos])
        {
            return -1;
        }
        pos++;
    }
    return 0;
}
/// @brief
/// @param byte
/// @return the number of bytes read or -1 in case of error
int readByteWithAlarm(unsigned char *byte)
{
    if (alarmEnabled == FALSE)
    {
        alarm(config.timeout);
        alarmEnabled = TRUE;
    }

    int nbytes = readByteSerialPort(byte);
    alarm(0);
    alarmEnabled = FALSE;
    return nbytes;
}