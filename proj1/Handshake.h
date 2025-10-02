#ifndef __HANDSHAKE__
#define __HANDSHAKE__


typedef struct
{
    int bytes;
    char flag_b;
    char address;
    char control;
    char bcc;
    char data;
    char flag_e;
    char curr_byte;

}Packet;

/*
    Creates a new handshake initialize at 0
    @returns A pointer to the Handshake
*/
Packet* create_packet();

/*
    @param byte - Byte to be processed
    @param Handshake - Handshake where the byte will be incerted into
    @returns 0 on success, 1 if the handshake is already full and -1 otherwise
*/
int process_byte(char byte, Packet* handshake);

/*
    Returns number of bytes already filled in
    the Handshake on success, -1 otherwise
*/
int number_bytes_received(Packet* handshake);


#endif