// Link layer header.
// DO NOT CHANGE THIS FILE

#ifndef _LINK_LAYER_H_
#define _LINK_LAYER_H_

#define FLAG_VALUE 0x7E
#define ADDRESS_UA 0x03 // Address field in frames that are commands sent by the Transmitter or replies sent by the Receiver
#define ADDRESS_SET 0x03 // Address field in frames that are commands sent by the Transmitter or replies sent by the Receiver
#define ADDRESS_SET_RECIVER 0x01 // Address field in frames that are commands sent by the Receiver or replies sent by the Transmitter
#define ADDRESS_UA_RECIVER 0x01 // Address field in frames that are commands sent by the Receiver or replies sent by the Transmitter
#define CTRL_UA 0x07 // confirmation to the reception of a valid supervision frame
#define CTRL_SET 0x03 // Sent by the transmitter to initiate a connection
// The control function is dependent on the frame number (0,1)
#define CTRL_RR(x) 0xAA+x

#define CTRL_I(x) x<<6
// The REJ command is dependet on the frame being rejected(same as RR)
#define CTRL_REJ(x) 0x54+x
// The DISC can be sent both ways, if the DISC was sent by the transmiter, the code being sent would be ADDRESS_UA,
// if sent by the reciver would be the ADDRESS_SET_RECEIVER
#define CTRL_DISC 0x0B
#define COMMAND_SIZE 4
// The UA_COMMAND is right but only because of casuality because if the code was to be transmition both ways, this is Wrong
#define UA_COMMAND {FLAG_VALUE, ADDRESS_UA, CTRL_UA, ADDRESS_UA^CTRL_UA}
//The same thing applies to the SET_COMMAND
#define SET_COMMAND {FLAG_VALUE, ADDRESS_SET, CTRL_SET, ADDRESS_SET^CTRL_SET}
// The rr command is using ADDRESS_RR that needs to be set depending on the frame being Recive Ready (0,1) 
#define COMMAND(x,y)  {FLAG_VALUE, x, y, x^y}

#define RR_COMMAND(x) {FLAG_VALUE, ADDRESS_UA, CTRL_RR(x), ADDRESS_UA^(CTRL_RR(x))}

#define REJ_COMMAND(x) {FLAG_VALUE, ADDRESS_UA, CTRL_REJ(x), ADDRESS_UA^(CTRL_REJ(x))}

typedef enum
{
    LlTx,
    LlRx,
} LinkLayerRole;

typedef struct
{
    char serialPort[50];
    LinkLayerRole role;
    int baudRate;
    int nRetransmissions;
    int timeout;
} LinkLayer;

// Size of maximum acceptable payload.
// Maximum number of bytes that application layer should send to link layer.
#define MAX_PAYLOAD_SIZE 1000

// MISC
#define FALSE 0
#define TRUE 1

// Open a connection using the "port" parameters defined in struct linkLayer.
// Return 0 on success or -1 on error.
int llopen(LinkLayer connectionParameters);

// Send data in buf with size bufSize.
// Return number of chars written, or -1 on error.
int llwrite(const unsigned char *buf, int bufSize);

// Receive data in packet.
// Return number of chars read, or -1 on error.
int llread(unsigned char *packet);

// Close previously opened connection and print transmission statistics in the console.
// Return 0 on success or -1 on error.
int llclose();

#endif // _LINK_LAYER_H_
