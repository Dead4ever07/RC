// Application layer protocol implementation

#include "application_layer.h"
#include "link_layer.h"
#include "packet_handler.h"
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include "macros.h"
#include "packet_handler.h"


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
            printf("Unable to open/create the file\n");
            return;
        }
        
        struct stat st;
        if (stat(filename, &st) == -1)
        {
            printf("Unable to see the file size\n");
            return;
        }
        long fileSize = st.st_size;

        linkLayer.role = LlTx;
        if(llopen(linkLayer) != 0)
        {
            printf("llopen failed\n");
            fclose(fptr);
            return;
        }
        if(writeControlPacket(START_CONTROL,fileSize,filename) != 0)
        {
            printf("Error sending the START package.\n");
            return;
        }

        int nBytes = fread(buffPayload + DATA_HEADER_SIZE, 1, MAX_DATA_FIELD, fptr);
        while (nBytes > 0)
        {

            buffPayload[0] = DATA_CONTROL;
            buffPayload[1] = nBytes / 256;
            buffPayload[2] = nBytes % 256;
            
            if(llwrite(buffPayload, nBytes + DATA_HEADER_SIZE) != 0)
            { 
                printf("Error sending the llwrite.\n");
                return;
            }

            nBytes = fread(buffPayload + DATA_HEADER_SIZE, 1, MAX_DATA_FIELD, fptr);
        }
        if(nBytes == 0)
        {
            printf("The file ended\n.");
            if (writeControlPacket(END_CONTROL,fileSize,filename) != 0){
                printf("There was an error sending the END packet.\n");
                return;
            }

        }
        else 
        {
            printf("Error reading the file\n.");
            return;
        }
        
        fclose(fptr);
        //llclose();
    }
    else if (role[0] == 'r' && role[1] == 'x')
    {
        fptr = fopen(filename, "w+");
        if (fptr == NULL)
        {
            printf("Unable to open/create the file\n");
            return;
        }

        linkLayer.role = LlRx;
        if(llopen(linkLayer) != 0){ 
            return;
        }

        int nBytes = 0;
        nBytes = llread(buffPayload);
        unsigned long fileSize = 0;
        char filenameReceived[MAX_FILENAME_SIZE + 1]; 

        if(readControlPacket(START_CONTROL,buffPayload,&fileSize, filenameReceived) != 0){
            printf("Error receiving the START package.\n");
            return;
        }
        
        //mudar ligeiramente a estrutura para um while norma e verificar o controlo!
        nBytes = llread(buffPayload);
        while(nBytes > 0 && buffPayload[0] == DATA_CONTROL)
        {
            int l2 = buffPayload[1];
            int l1 = buffPayload[2];
            int size = l2*256 + l1;

            if(size + DATA_HEADER_SIZE != nBytes){
                printf("Error the number of data read(%d) is different from the data send(%d).\n", nBytes, size+DATA_HEADER_SIZE);
                return;
            }
            printf("writing to the file n = %d bytes\n",size);

            fwrite(buffPayload + DATA_HEADER_SIZE, 1, size, fptr);
            nBytes = llread(buffPayload);
        }
        if(buffPayload[0] == END_CONTROL)
        {
            unsigned long fileSizeEnd = 0;
            char filenameReceivedEnd[MAX_FILENAME_SIZE + 1]; 

            if (readControlPacket(END_CONTROL,buffPayload,&fileSizeEnd,filenameReceivedEnd) != 0){
                printf("There was an error receiving the END packet.\n");
                return;
            }
            if (fileSizeEnd != fileSize || strcmp(filenameReceived,filenameReceivedEnd) != 0){
                printf("The information in the start and the END packet were different.\n");
                return;
            }
        }
        else 
        {
            printf("Error reading the file\n.");
            return;
        }
        
        fclose(fptr);
        //llclose();
    } 
    else
    {
        printf("Invalid role.\n");
    }
    return;
}


//DAR HANDLE AOS CASOS DE ERRO GARVE PARA DEPOIS FECAHAR A PORTA E POINTER, TER BASTANTE ATENÇÃO A ISSO!