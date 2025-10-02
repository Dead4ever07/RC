#include "Handshake.h"
#include <stdlib.h>

Packet *new_packet()
{
    Packet *package_handler = malloc(sizeof(Packet));
    memset(package_handler,0,sizeof(Packet));
    return package_handler;
}

int process_byte(char byte, Packet *packet)
{
    switch (packet->bytes)
    {
    case 0:
        packet->flag_b = byte;
        break;
    case 1:
        packet->address = byte;
        break;
    case 2:
        packet->control = byte;
        break;
    case 3:
        packet->bcc = byte;
        if (byte != packet->address ^ packet->control)
            return -1;
        break;
    case 4:
        packet->flag_e = byte;
        break;
    default:
        return 1;
        break;
    }
    packet->bytes++;
    return 0;
}

int number_bytes_received(Packet* packet){
    return packet->bytes;
}
