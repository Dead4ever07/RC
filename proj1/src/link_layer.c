#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "link_layer.h"
#include "serial_port.h"
#include "alarm.h"
#include "data_protocol.h"
#include "macros.h"
#include "serial_communications.h"


#define _POSIX_SOURCE 1 

LinkLayer config;
int timeout;
int currFrame = 0;
    
int sendDiscCommand(unsigned char *discCommand);

////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{
    config = connectionParameters;
    timeout = config.timeout;

    // Open port
    if (openSerialPort(config.serialPort, config.baudRate) == -1)
    {
        perror("Error opening the Serial Port.");
        return -1;
    }
    // Start alarm signals
    alarmSetup();

    unsigned char setCommand[] = SET_COMMAND;
    unsigned char uaCommand[] = UA_COMMAND;

    if (connectionParameters.role == LlTx)
    {
        //printf("Running as Transmitter\n");
        if (sendFrame(setCommand, COMMAND_SIZE, uaCommand, config.nRetransmissions) != 0)
        {
            printf("Error sending the set command or receiving the ua.\n");
            return -1;
        }
    }
    else
    {
        //printf("Running as Reciver\n");
        if (readBytesAndCompare(setCommand) != 0)
        {
            printf("Error receiving the set command.\n");
            return -1;
        }
        //printf("responding to the transmiter\n");
        if (writeBytesSerialPort(uaCommand, COMMAND_SIZE) != COMMAND_SIZE)
        {
            printf("Error sending the UA command through the serial port.\n");
            return -1;
        }
    }

    return 0;
}

int llwrite(const unsigned char *buf, int bufSize)
{
    unsigned char frame[MAX_PAYLOAD_SIZE*2+7] = COMMAND(ADDRESS_SET, CTRL_I(currFrame));
    int pos = 4;
    unsigned char BCC2 = 0;
    for(int i = 0; i<bufSize; i++){
        
        BCC2 ^= buf[i];
        pos += byteStuffing(&frame[pos], buf[i]);
    }
    pos += byteStuffing(&frame[pos], BCC2);
    frame[pos] = FLAG_VALUE;
    int tries = 0;
    while (tries<config.nRetransmissions)
    {
        
        tries++;
        printf("sending %d bytes\n", pos+1);
        if(writeBytesSerialPort(frame, pos+1) != pos+1){
            printf("Error writing the frame to the serial port\n");
            return -1;
        }
        char response[COMMAND_SIZE] = COMMAND(ADDRESS_SET, CTRL_RR((currFrame^1)));
        if(readBytesAndCompare(response) == 0){
            currFrame ^=1;
            return 0;
        }
    }
    printf("TX:Number of tries excided\n");
    return -1;
}

int llread(unsigned char *packet)
{
    int tries = 0;
    while (tries<config.nRetransmissions)
    {
        tries++;
        unsigned char byte;
        int resp = readByteWithAlarm(&byte);
        if (resp == 1)
        {
            tries=0;
            int res = processByte(byte, packet, currFrame);
            if (res < 0)
            {
                printf("There was an error while reciving the package.\n");
                unsigned char response[COMMAND_SIZE] = COMMAND(ADDRESS_SET, CTRL_REJ(currFrame));
                int response_size = writeBytesSerialPort(response, COMMAND_SIZE);
                if(response_size != COMMAND_SIZE){
                    printf("Error sending Reject!\n");
                }
            }
            else if (res > 0)
            {
                currFrame ^= 1;
                unsigned char response[COMMAND_SIZE] = COMMAND(ADDRESS_SET, CTRL_RR(currFrame));
                int responseSize = writeBytesSerialPort(response, COMMAND_SIZE);
                if(responseSize != COMMAND_SIZE){
                    printf("Error sending the RR!\n");
                }
                return res;
            }
        }
        else if (resp < 0)
        {
            printf("Couldn't read the byte with alarm from the serial port, an error ocurred\n");
        }else{
            printf("Couldn't read with alarm.\n");
        }
        
    }
    printf("Couldn't read from the transmitor in %d tentatives\n", tries);
    return -1;

}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose()
{
    if (config.role == LlRx)
    {
        unsigned char discCommand[] = SENDER_DISC_COMMAND;

        if (sendDiscCommand(discCommand) != 0)
        {
            return -1;
        }
    }
    else
    {
        unsigned char discCommand[] = RECEIVER_DISC_COMMAND;

        if (sendDiscCommand(discCommand) != 0)
        {
            return -1;
        }
    }
    return 0;
}

/// @brief Responsible to the sending the disc command and the ua response
/// @param discCommand it depends on the role
/// @return 0 if nothing went wrong
int sendDiscCommand(unsigned char *discCommand)
{

    unsigned char uaCommand[] = UA_COMMAND;

    if (sendFrame(discCommand, COMMAND_SIZE, discCommand, config.nRetransmissions) != 0)
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

