// Link layer protocol implementation

#include "link_layer.h"
#include "serial_port.h"
#include <fcntl.h>
#include <termios.h>
#include "packet.h"
#include <signal.h>

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

int sendFrame(unsigned char *bytes, int nBytes, unsigned char* ackByte, int nAckBytes)
{
    int try = 0;
    while(try < config.nRetransmissions)
    {
        try++;
        if(writeBytesSerialPort(bytes, nBytes) != nBytes)
        {
            continue;
        }
    }
    while(try < config.nRetransmissions)
    {
        struct sigaction act = {0};
        act.sa_handler = &alarmHandler;
        if (sigaction(SIGALRM, &act, NULL) == -1)
        {
            perror("sigaction");
            exit(1);
        }

        int byteCounter = 0;
        while (alarmCount < config.timeout)
        {
            unsigned char *byteReceived;
            int flag = readByteSerialPort(byteReceived);
            if(flag == 0 && byteCounter == nAckBytes){
                return 0;
            }
            if(flag == -1){
                break;
            }
            if(byteReceived == ackByte[byteCounter]){
                byteCounter++;
            }
            if (alarmEnabled == FALSE)
            {
                alarm(config.timeout);
                alarmEnabled = TRUE;
            }
        }
    }
    return -1;
    
}

//definir melhor isto!!
unsigned char* byteSet(){
    unsigned char *bytes[4];
    bytes[0] = FLAG_VALUE;
    bytes[1] = ADDRESS_SET;
    bytes[2] = CTRL_SET;
    bytes[3] = CTRL_SET ^ ADDRESS_SET;
    return bytes;
}

////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters){
    config = connectionParameters;

    if(openSerialPort(config.serialPort, config.baudRate)  == -1){
        perror("Error opening the Serial Port.");
        return -1;
    }

    if(connectionParameters.role == LlTx){

    }

    return 0;
}


////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int llwrite(const unsigned char *buf, int bufSize)
{
    // TODO: Implement this function

    return 0;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{
    // TODO: Implement this function

    return 0;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose()
{
    // TODO: Implement this function

    return 0;
}
