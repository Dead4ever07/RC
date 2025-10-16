// Application layer protocol implementation

#include "application_layer.h"
#include "link_layer.h"
#include <stdio.h>
#include <string.h>
#include "macros.h"

void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    LinkLayer linkLayer;
    linkLayer.baudRate = baudRate;
    strcpy(linkLayer.serialPort, serialPort);
    linkLayer.nRetransmissions = nTries;
    linkLayer.timeout = timeout;
    FILE *fptr;

    fptr = fopen(filename, "wb+");
    if (fptr == NULL)
    {
        printf("Unable to open/create the file\n");
        return;
    }
    unsigned char buffPayload[MAX_PAYLOAD_SIZE];

    if (role[0] == 't')
    {
        linkLayer.role = LlTx;
        llopen(linkLayer);
        while (TRUE)
        {

            int nbytes = fread(buffPayload, 1, MAX_PAYLOAD_SIZE, fptr);
            if (nbytes == 0)
            {
                return;
            }
            llwrite(buffPayload, nbytes);
        }
        fclose(fptr);
        llclose();
    }
    else
    {
        linkLayer.role = LlRx;
        llopen(linkLayer);
        int nBytes = 0;
        do{
            nBytes = llread(buffPayload);
            fwrite(buffPayload, nBytes, 1, fptr);
        }while(nBytes>0);
        fclose(fptr);
        llclose();        
        
    }
    return;
}
