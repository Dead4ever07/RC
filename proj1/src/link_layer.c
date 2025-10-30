#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "link_layer.h"
#include "serial_port.h"
#include "alarm.h"
#include "data_protocol.h"
#include "serial_communication.h"


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

    unsigned char setCommand[] = COMMAND(ADDRESS_SET,CTRL_SET);
    unsigned char uaCommand[] = COMMAND(ADDRESS_SET,CTRL_UA);
    if (connectionParameters.role == LlTx)
    {
        if (sendFrame(setCommand, COMMAND_SIZE, uaCommand, config.nRetransmissions) != 0)
        {
            perror("Error sending the set command or receiving the ua.\n");
            return -1;
        }
    }
    else
    {
        if (readBytesAndCompare(setCommand,NULL) != 0)
        {
            perror("Error receiving the set command.\n");
            return -1;
        }
        if (writeBytesSerialPort(uaCommand, COMMAND_SIZE) != COMMAND_SIZE)
        {
            perror("Error sending the UA command through the serial port.\n");
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
        pos++;
        if(writeBytesSerialPort(frame, pos) != pos){
            perror("Error writing the frame to the serial port\n");
            return -1;
        }
        unsigned char response1[COMMAND_SIZE] = COMMAND(ADDRESS_SET, CTRL_RR((currFrame^1)));
        unsigned char response2[COMMAND_SIZE] = COMMAND(ADDRESS_SET, CTRL_REJ((currFrame)));
        switch (readBytesAndCompare(response1, response2))
        {
        case 0:
            currFrame ^=1;
            return pos;
            break;
        case 1:
            perror("The frame was rejected\n");
            break;    
        default:
            perror("The response was an invalid frame\n");
            break;
        }
    }
    perror("TX:Number of tries excided\n");
    return -1;
}

int llread(unsigned char *packet)
{
    while (TRUE)
    {
        unsigned char byte;
        int resp = readByteWithAlarm(&byte);
        if (resp == 1)
        {
            int res = processByte(byte, packet, currFrame);
            if (res < 0)
            {
                perror("There was an error while reciving the package.\n");
                unsigned char response[COMMAND_SIZE] = COMMAND(ADDRESS_SET, CTRL_REJ(currFrame));
                int response_size = writeBytesSerialPort(response, COMMAND_SIZE);
                if(response_size != COMMAND_SIZE){
                    perror("Error sending Reject Frame!\n");
                }
            }
            else if (res > 0)
            {
                currFrame ^= 1;
                unsigned char response[COMMAND_SIZE] = COMMAND(ADDRESS_SET, CTRL_RR(currFrame));
                int responseSize = writeBytesSerialPort(response, COMMAND_SIZE);
                if(responseSize != COMMAND_SIZE){
                    perror("Error sending the RR!\n");
                }
                return res;
            }
        }
        else if (resp < 0)
        {
            perror("Couldn't read the byte with alarm from the serial port, an error ocurred\n");
        }else{
            perror("Couldn't read with alarm.\n");
        }
        
    }
        return -1;

}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose()
{
    //mudar as macros!!
    unsigned char discCommandTx[] = COMMAND(ADDRESS_SET,CTRL_DISC);
    unsigned char discCommandRx[] = COMMAND(ADDRESS_RECEIVER_DISC,CTRL_DISC);
    unsigned char uaCommandTx[] = COMMAND(ADDRESS_RECEIVER_DISC,CTRL_UA);
    if (config.role == LlTx)
    {
        if (sendFrame(discCommandTx, COMMAND_SIZE, discCommandRx, config.nRetransmissions) != 0)
        {
            perror("Error sending DISC and/or receiving the DISC.\n");
            return -1;
        }
        //tenho que dar handle destas duas funções, tipo tentar varias vezes!?
        if (writeBytesSerialPort(uaCommandTx, COMMAND_SIZE) != COMMAND_SIZE)
        {
            perror("Error sending the UA command through the serial port.\n");
            return -1;
        }

        if (closeSerialPort() != 0)
        {
            perror("Error closing the Serial port\n");
            return -1;
        }
    }
    else
    {
        if (readBytesAndCompare(discCommandTx,NULL) != 0)
        {
            perror("Error receiving the disc command.\n");
            return -1;
        }
        if (sendFrame(discCommandRx, COMMAND_SIZE, uaCommandTx, config.nRetransmissions) != 0)
        {
            perror("Error sending the DISC command and/or receiving the UA.\n");
            return -1;
        }
        
        if (closeSerialPort() != 0)
        {
            perror("Error closing the Serial port\n");
            return -1;
        }
    }
    return 0;
}


