// Application layer protocol implementation

#include "application_layer.h"
#include "link_layer.h"
#include <stdio.h>
#include <string.h>

void applicationLayer(const char *serialPort, const char *role, int baudRate,
                      int nTries, int timeout, const char *filename)
{
    LinkLayer linkLayer;
    linkLayer.baudRate = baudRate;
    strcpy(linkLayer.serialPort ,serialPort);
    linkLayer.nRetransmissions = nTries;
    linkLayer.timeout = timeout;
    linkLayer.role = (role[0] == 't'?LlTx:LlRx);
    printf(role[0] == 'T'?"role Tx":"role Rx");

    llopen(linkLayer);
    

}
