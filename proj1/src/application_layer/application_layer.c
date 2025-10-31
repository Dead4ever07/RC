// Application layer protocol implementation
#include "application_layer.h"



void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    LinkLayer linkLayer;
    linkLayer.baudRate = baudRate;
    strcpy(linkLayer.serialPort, serialPort);
    linkLayer.nRetransmissions = nTries;
    linkLayer.timeout = timeout;
    FILE *fptr;

    unsigned char buffPayload[MAX_PAYLOAD_SIZE*2];

    if (role[0] == 't' && role[1] == 'x')
    {
        fptr = fopen(filename, "r");
        if (fptr == NULL)
        {
            printError(__func__, "Unable to open/create the file\n");
            return;
        }
        
        struct stat st;
        if (stat(filename, &st) == -1)
        {
            printError(__func__, "Unable to see the file size\n");
            return;
        }
        long fileSize = st.st_size;

        linkLayer.role = LlTx;
        if(llopen(linkLayer) != 0)
        {
            printError(__func__, "llopen failed\n");
            fclose(fptr);
            return;
        }
        if(writeControlPacket(START_CONTROL,fileSize,filename) != 0)
        {
            printError(__func__, "Error sending the START package.\n");
            fclose(fptr);
            llclose();
            return;
        }

        int nBytes = fread(buffPayload + DATA_HEADER_SIZE, 1, MAX_DATA_FIELD, fptr);
        while (nBytes > 0)
        {
            buffPayload[0] = DATA_CONTROL;
            buffPayload[1] = nBytes / 256;
            buffPayload[2] = nBytes % 256;
            
            if(llwrite(buffPayload, nBytes + DATA_HEADER_SIZE) < 0)
            { 
                printError(__func__, "Error sending the llwrite.\n");
                fclose(fptr);
                return;
            }
            nBytes = fread(buffPayload + DATA_HEADER_SIZE, 1, MAX_DATA_FIELD, fptr);
        }
        if(nBytes == 0)
        {
            if (writeControlPacket(END_CONTROL,fileSize,filename) != 0){
                printError(__func__, "There was an error sending the END packet.\n");
                fclose(fptr);
                return;
            }

        }
        else 
        {
            printError(__func__, "Error reading the file\n.");
            fclose(fptr);
            return;
        }
        
        fclose(fptr);
        llclose();
    }
    else if (role[0] == 'r' && role[1] == 'x')
    {
        fptr = fopen(filename, "w+");
        if (fptr == NULL)
        {
            printError(__func__, "Unable to open/create the file\n");
            fclose(fptr);
            return;
        }
        
        linkLayer.role = LlRx;
        if(llopen(linkLayer) != 0){ 
            fclose(fptr);
            return;
        }

        int bytesRead = 0;
        int nBytes = 0;
        nBytes = llread(buffPayload);
        unsigned long fileSize = 0;
        char filenameReceived[MAX_FILENAME_SIZE + 1]; 

        if(readControlPacket(START_CONTROL,buffPayload,&fileSize, filenameReceived) != 0){
            printError(__func__, "Error receiving the START package.\n");
            fclose(fptr);
            return;
        }

        nBytes = llread(buffPayload);
        while(nBytes > 0 && buffPayload[0] == DATA_CONTROL)
        {
            int l2 = buffPayload[1];
            int l1 = buffPayload[2];
            int size = l2*256 + l1;

            if(size + DATA_HEADER_SIZE != nBytes){
                printError(__func__, "Error the number of data read is different from the data send.\n");
                fclose(fptr);
                return;
            }
            bytesRead += size;

            fwrite(buffPayload + DATA_HEADER_SIZE, 1, size, fptr);
            nBytes = llread(buffPayload);
        }
        if(buffPayload[0] == END_CONTROL)
        {
            unsigned long fileSizeEnd = 0;
            char filenameReceivedEnd[MAX_FILENAME_SIZE + 1]; 

            if (readControlPacket(END_CONTROL,buffPayload,&fileSizeEnd,filenameReceivedEnd) != 0){
                printError(__func__, "There was an error receiving the END packet.\n");
                fclose(fptr);
                llclose();
                return;
            }
            if (fileSizeEnd != fileSize || strcmp(filenameReceived,filenameReceivedEnd) != 0){
                printError(__func__, "The information in the start and the END packet were different.\n");
                fclose(fptr);
                llclose();
                return;
            }
            if(bytesRead != fileSizeEnd){
                printError(__func__, "We didn't read all the bytes in the file.\n");
                fclose(fptr);
                llclose();
                return;
            }
        }
        else 
        {
            printError(__func__, "Error reading the file\n.");
            fclose(fptr);
            return;
        }
        
        fclose(fptr);
        llclose();
    } 
    else
    {
        printError(__func__, "Invalid role.\n");
    }
    return;
}

