#include "Handshake.h"
#include <stdlib.h>

Handshake *create_Handshake()
{
    Handshake *handshake = malloc(sizeof(Handshake));
    handshake->bytes = 0;
}

int process_byte(char byte, Handshake *handshake)
{
    switch (handshake->bytes)
    {
    case 0:
        handshake->flag_b = byte;
        break;
    case 1:
        handshake->address = byte;
        break;
    case 2:
        handshake->control = byte;
        break;
    case 3:
        handshake->bcc = byte;
        if (byte != handshake->address ^ handshake->control)
            return -1;
        break;
    case 4:
        handshake->flag_e = byte;
        break;
    default:
        return 1;
        break;
    }
    handshake->bytes++;
    return 0;
}

int number_bytes_received(Handshake* handshake){
    return handshake->bytes;
}
