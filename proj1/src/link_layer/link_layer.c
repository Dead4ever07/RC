#include "link_layer.h"


#define _POSIX_SOURCE 1 

LinkLayer config;
int timeout;
int currFrame = 0;
    

int llopen(LinkLayer connectionParameters)
{
    config = connectionParameters;
    timeout = config.timeout;
    startStatistics();

    if (openSerialPort(config.serialPort, config.baudRate) == -1)
    {
        printError(__func__, "Error opening the Serial Port.");
        return -1;
    }

    alarmSetup();

    unsigned char setCommand[] = COMMAND(ADDRESS_SET,CTRL_SET);
    unsigned char uaCommand[] = COMMAND(ADDRESS_SET,CTRL_UA);
    if (connectionParameters.role == LlTx)
    {
        if (sendFrame(setCommand, COMMAND_SIZE, uaCommand, config.nRetransmissions) != 0)
        {
            printError(__func__, "Error sending the set command or receiving the ua.\n");
            return -1;
        }
    }
    else
    {
        while(readBytesAndCompare(setCommand,NULL) != 0)
        {
            printError(__func__, "Error receiving the set command.\n");
        }
        if (writeBytesToSerialPort(uaCommand, COMMAND_SIZE) != COMMAND_SIZE)
        {
            printError(__func__, "Error sending the UA command through the serial port.\n");
            return -1;
        }
    }

    return 0;
}

int llwrite(const unsigned char *buf, int bufSize)
{
    startPacketTrack();
    unsigned char frame[MAX_PAYLOAD_SIZE*2+COMMAND_SIZE+2] = COMMAND(ADDRESS_SET, CTRL_I(currFrame));
    int pos = 4;
    unsigned char BCC2 = 0;
    for(int i = 0; i<bufSize; i++){
        
        BCC2 ^= buf[i];
        pos += byteStuffing(&frame[pos], buf[i]);
    }
    pos += byteStuffing(&frame[pos], BCC2);
    frame[pos++] = FLAG_VALUE;

    int tries = 0;
    while (tries<=config.nRetransmissions)
    {

        tries++;
        if(writeBytesToSerialPort(frame, pos) != pos){
            printError(__func__, "Error writing the frame to the serial port\n");
            return -1;
        }
        unsigned char response1[COMMAND_SIZE] = COMMAND(ADDRESS_SET, CTRL_RR((currFrame^1)));
        unsigned char response2[COMMAND_SIZE] = COMMAND(ADDRESS_SET, CTRL_REJ((currFrame)));
        switch (readBytesAndCompare(response1, response2))
        {
        case 0:
            currFrame ^=1;
            endPacketTrack();
            return pos;
            break;
        case 1:
            rejectIncrement();
            printError(__func__, "The frame was rejected\n");
            break;    
        default:
            printError(__func__, "The response was an invalid frame\n");
            break;
        }
    }
    printError(__func__, "TX:Number of tries excided\n");
    endPacketTrack();
    return -1;
}

int llread(unsigned char *packet)
{
    startPacketTrack();
    while (TRUE)
    {
        unsigned char byte;
        int resp = readByteWithAlarm(&byte);
        if (resp == 1)
        {
            int res = processByte(byte, packet, currFrame);
            if (res == -1)
            {
                printError(__func__, "There was an error while reciving the package.\n");
                unsigned char response[COMMAND_SIZE] = COMMAND(ADDRESS_SET, CTRL_REJ(currFrame));
                rejectIncrement();
                framesRecivedIncrement();
                int response_size = writeBytesToSerialPort(response, COMMAND_SIZE);
                if(response_size != COMMAND_SIZE){
                    printError(__func__, "Error sending Reject Frame!\n");
                }
            }else if(res == -2){
                printError(__func__, "There was an error while reciving the package.\n");
                unsigned char response[COMMAND_SIZE] = COMMAND(ADDRESS_SET, CTRL_RR(currFrame));
                framesRecivedIncrement();
                int response_size = writeBytesToSerialPort(response, COMMAND_SIZE);
                if(response_size != COMMAND_SIZE){
                    printError(__func__, "Error sending Reject Frame!\n");
                }

            }
            else if (res > 0)
            {
                currFrame ^= 1;
                framesRecivedIncrement();
                unsigned char response[COMMAND_SIZE] = COMMAND(ADDRESS_SET, CTRL_RR(currFrame));
                int responseSize = writeBytesToSerialPort(response, COMMAND_SIZE);
                if(responseSize != COMMAND_SIZE){
                    printError(__func__, "Error sending the RR!\n");
                }
                endPacketTrack();
                return res;
            }
        }
        else if (resp < 0)
        {
            printError(__func__, "Couldn't read the byte with alarm from the serial port, an error ocurred\n");
        }else{
            printError(__func__, "Couldn't read with alarm.\n");
        }
        
    }
    endPacketTrack();
    return -1;

}

int llclose()
{
    unsigned char discCommandTx[] = COMMAND(ADDRESS_SET,CTRL_DISC);
    unsigned char discCommandRx[] = COMMAND(ADDRESS_RECEIVER_DISC,CTRL_DISC);
    unsigned char uaCommandTx[] = COMMAND(ADDRESS_RECEIVER_DISC,CTRL_UA);
    if (config.role == LlTx)
    {
        if (sendFrame(discCommandTx, COMMAND_SIZE, discCommandRx, config.nRetransmissions) != 0)
        {
            printError(__func__, "Error sending DISC and/or receiving the DISC.\n");
            return -1;
        }
        
        if (writeBytesToSerialPort(uaCommandTx, COMMAND_SIZE) != COMMAND_SIZE)
        {
            printError(__func__, "Error sending the UA command through the serial port.\n");
            return -1;
        }

        if (closeSerialPort() != 0)
        {
            printError(__func__, "Error closing the Serial port\n");
            return -1;
        }
    }
    else
    {
        if (readBytesAndCompare(discCommandTx,NULL) != 0)
        {
            printError(__func__, "Error receiving the disc command.\n");
            return -1;
        }
        if (sendFrame(discCommandRx, COMMAND_SIZE, uaCommandTx, config.nRetransmissions) != 0)
        {
            printError(__func__, "Error sending the DISC command and/or receiving the UA.\n");
            return -1;
        }
        
        if (closeSerialPort() != 0)
        {
            printError(__func__, "Error closing the Serial port\n");
            return -1;
        }
    }
    endStatistics();
    return 0;
}


