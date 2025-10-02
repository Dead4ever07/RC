#ifndef __HANDSHAKE__
#define __HANDSHAKE__


typedef struct
{
    int bytes;
    char flag_b;
    char address;
    char control;
    char bcc;
    char flag_e;
}Handshake;

/*
    Creates a new handshake initialize at 0
    @returns A pointer to the Handshake
*/
Handshake* create_Handshake();

/*
    @param byte - Byte to be processed
    @param Handshake - Handshake where the byte will be incerted into
    @returns 0 on success, 1 if the handshake is already full and -1 otherwise
*/
int process_byte(char byte, Handshake* handshake);

/*
    Returns number of bytes already filled in
    the Handshake on success, -1 otherwise
*/
int number_bytes_received(Handshake* handshake);


#endif