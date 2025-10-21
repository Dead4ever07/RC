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
    //pq o strcpy?
    strcpy(linkLayer.serialPort, serialPort);
    linkLayer.nRetransmissions = nTries;
    linkLayer.timeout = timeout;
    FILE *fptr;

    unsigned char buffPayload[MAX_PAYLOAD_SIZE];

    if (role[0] == 't')
    {
        fptr = fopen(filename, "r");
        if (fptr == NULL)
        {
            printf("Unable to open/create the file\n");
            return;
        }

        linkLayer.role = LlTx;
        if(llopen(linkLayer) != 0){
            printf("llopen failed\n");
            fclose(fptr);
            return;
        }
    
        while (TRUE)
        {

            int nbytes = fread(buffPayload, 1, MAX_PAYLOAD_SIZE, fptr);
            if (nbytes == 0)
            {
                printf("The file ended\n");
                break;
            }
            for(int i = 0; i<nbytes; i++){
                printf("aplication byte = %x\n", buffPayload[i]);
            }
            if(llwrite(buffPayload, nbytes) != 0)
            { 
                printf("Error sending the llwrite.\n");
                return;
            }
        }
        fclose(fptr);
        // The aplication layer needs to make the other aplication layer know that it is suposed to disconect, for that we should 
        // send another frame to make it disconect from one another.
        //o llclose pode estar mal aplicado!
        //llclose();
    }
    else
    {
        fptr = fopen(filename, "w+");
        if (fptr == NULL)
        {
            printf("Unable to open/create the file\n");
            return;
        }

        linkLayer.role = LlRx;
        if(llopen(linkLayer) != 0) return;
        int nBytes = 0;
        do
        {
            printf("writing to the file n = %d bytes\n",nBytes);
            nBytes = llread(buffPayload);
            fwrite(buffPayload, 1, nBytes, fptr);
        } while (nBytes > 0);
        fclose(fptr);
        //llclose();
    }
    return;
}
