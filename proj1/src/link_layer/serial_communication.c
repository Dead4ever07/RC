#include "serial_communication.h"

extern int alarmEnabled;
extern int timeout;

int byteReadStateMachine(unsigned char *frame);

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
        if (writeBytesToSerialPort(bytes, nBytes) != nBytes)
        {
            printError(__func__, "Error writing the frame to the serial port\n");
            continue;
        }
        if (readBytesAndCompare(ackByte, NULL) == 0)
        {
            return 0;
        }
        else
        {
            printError(__func__, "Error while reading/comparing bytes\n");
        }
    }
    printError(__func__, "Coudn't send Frame in the nRetransmissions\n");
    return -1;
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

int readBytesAndCompare(unsigned char bytesRef1[5], unsigned char bytesRef2[5])
{
    unsigned char buff[COMMAND_SIZE];
    int wrong = FALSE;
    if(bytesRef1 == NULL){
        printError(__func__, "The expected response was NULL.\n");
        return -1;
    } 
    while (TRUE)
    {
        if (byteReadStateMachine(buff) != 0)
        {
            return -1;
        }
        for (int i = 0; i < COMMAND_SIZE; i++)
        {
            if (buff[i] != bytesRef1[i])
            {
                wrong = TRUE;
            }
        }
        if (!wrong)
        {
            framesRecivedIncrement();
            return 0;
        }
        wrong = FALSE;
        if (bytesRef2 == NULL){
            continue;
        }
        for (int i = 0; i < COMMAND_SIZE; i++)
        {
            if (buff[i] != bytesRef2[i])
            {
                wrong = TRUE;
            }
        }
        if (!wrong)
        {
            framesRecivedIncrement();
            return 1;
        }
    }
    return -1;
}

int byteReadStateMachine(unsigned char *frame)
{
    int pos = 0;
    Read_State state_reader = START_READ;
    while (state_reader != END_READ)
    {
        int res = readByteWithAlarm(&frame[pos]);
        if (res <= 0)
        {
            printError(__func__, "Couldn't read from the serial port\n");
            return -1;
        }

        switch (state_reader)
        {
        case START_READ:
            if (frame[pos] == FLAG_VALUE)
            {
                state_reader = ADDRESS_READ;
                pos = 1;
            }
            break;
        case ADDRESS_READ:
            if (frame[pos] == FLAG_VALUE)
            {
                state_reader = ADDRESS_READ;
                pos = 1;
            }
            else
            {
                state_reader = CONTROL_READ;
                pos = 2;
            }
            break;
        case CONTROL_READ:
            if (frame[pos] == FLAG_VALUE)
            {
                state_reader = ADDRESS_READ;
                pos = 1;
            }
            else
            {
                state_reader = BCC1_READ;
                pos = 3;
            }
            break;
        case BCC1_READ:
            if (frame[pos] == FLAG_VALUE)
            {
                state_reader = ADDRESS_READ;
                pos = 1;
            }
            else
            {
                if (frame[pos] == (frame[pos - 1] ^ frame[pos - 2]))
                {
                    state_reader = FLAG_END_READ;
                    pos = 4;
                }
                else
                {
                    state_reader = START_READ;
                    pos = 0;
                }
            }
            break;
        case FLAG_END_READ:
            if (frame[pos] == FLAG_VALUE)
            {
                state_reader = END_READ;
            }
            else
            {
                state_reader = START_READ;
                pos = 0;
            }
            break;
        default:
            break;
        }
    }
    return 0;
}

int writeBytesToSerialPort(unsigned char *byte, int n)
{
    framesSentIncrement();
    return writeBytesSerialPort(byte, n);
}
