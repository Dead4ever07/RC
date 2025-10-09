#include "packet.h"
#include "serial_port.h"
#include <stdlib.h>

Packet *create_packet()
{
    Packet *package_handler = malloc(sizeof(Packet));
    memset(package_handler, 0, sizeof(Packet));
    return package_handler;
}

int process_byte_receiver(char byte, Packet *packet)
{
    switch (packet->state)
    {
    case START:
        process_flag(byte, packet);
        break;
    case FLAG:
        process_address(byte, packet);
        break;
    case ADDRESS:
        process_control(byte, packet);
        break;
    case CONTROL:
        process_bcc(byte, packet);
        break;
    case CHECKED:
        return process_data(byte,packet);
        break;
    default:
        return -1;
        break;
    }
    return 0;
}

// int number_bytes_received(Packet *packet)
// {
// //     return packet->bytes;
// }

void process_flag(char byte, Packet *packet)
{
    if (byte == FLAG_VALUE)
    {
        packet->state = FLAG;
        packet->flag_b = byte;
    }
}

void process_address(char byte, Packet *packet)
{
    switch (byte)
    {
    case ADDRESS_UA:
        packet->state = ADDRESS;
        packet->address = byte;
        break;
    case FLAG_VALUE:
        break;
    default:
        packet->state = START;
        break;
    }
}

void process_control(char byte, Packet *packet)
{
    switch (byte)
    {
    case CTRL_UA:
        packet->state = ADDRESS;
        packet->address = byte;
        break;
    case FLAG_VALUE:
        packet->state = FLAG;
        break;
    default:
        packet->state = START;
        break;
    }
}

void process_bcc(char byte, Packet *packet)
{
    switch (byte)
    {
    case FLAG_VALUE:
        packet->state = FLAG;
        break;
    default:
        if (byte == packet->address ^ packet->control)
        {
            packet->state = CHECKED;
            packet->bcc = byte;
        }
        break;
    }
    char buff[4] = {FLAG_VALUE, ADDRESS_UA, CTRL_UA, ADDRESS_UA^CTRL_UA};
    writeBytesSerialPort(buff,4);
}
int process_data(char byte, Packet *packet){
    if(byte == FLAG_VALUE){
        packet->state = FLAG;
        return 1;
    }
    /// Another state machine responsible for checking byte errors and packet tracking
}
