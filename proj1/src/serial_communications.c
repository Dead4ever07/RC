#include "serial_communications.h"


extern int alarmEnabled;
extern int timeout;

/// @brief Responsible to send a package in N tries
/// @param bytes The frame to be sent
/// @param nBytes The number of bytes in the frame
/// @param ackByte The Response byte expected
/// @return returns 0 on success
int sendFrame(unsigned char *bytes, int nBytes, unsigned char *ackByte, int nRetransmissions)
{
    int try = 0;
    while (try < nRetransmissions)
    {
        try++;
        //printf("Sending frame try nº%d\n",try);
        //printf("[");
        // for(int i = 0; i<nBytes; i++){
        //     printf(",%x", bytes[i]);
        // }
        // printf("]\n");
        if (writeBytesSerialPort(bytes, nBytes) != nBytes)
        {
            printf("Error writing the frame to the serial port\n");
            continue;
        }
        if (readBytesAndCompare(ackByte) == 0)
        {
            return 0;
        }
        else
        {
            printf("Error while reading/comparing bytes\n");
        }
    }
    printf("Coudn't send Frame in the nRetransmissions\n");
    return -1;
}


int readBytesAndCompare(unsigned char *bytesRef)
{
    int pos = 0;
    int isWrong = FALSE;
    unsigned char buff[COMMAND_SIZE];
    while (pos < COMMAND_SIZE)
    {
        int res = readByteWithAlarm(buff + pos);
        if (res == 0)
        {
            //fazer um try
            continue;
        }
        else if (res == -1)
        {
            //dar o erro certo!
            return -1;
        }
        else
        {
            if (buff[pos] != bytesRef[pos])
            {
                //perguntar ao professor sobre o lixo!
                printf("Wrong pos = %d\n", pos);
                printf("%d != %d\n", buff[pos], bytesRef[pos]);
                printf("Erro while reading bytes wrong response\n");
                isWrong = TRUE;
            }
            pos++;
        }
    }
    return isWrong;
}

int readByteWithAlarm(unsigned char *byte)
{
    if (alarmEnabled == FALSE)
    {
        alarm(timeout);
        alarmEnabled = TRUE;
    }
    int nbytes = readByteSerialPort(byte);
    alarm(0);
    alarmEnabled = FALSE;
    return nbytes;
}