#include "data_protocol.h"

RX_State state = START;
int pos = 0;
int BCC2 = 0;

int byteStuffing(unsigned char *data, unsigned char byte)
{
    if (byte == FLAG_VALUE || byte == EXCAPE_CHAR)
    {
        *data = EXCAPE_CHAR;
        *(data + 1) = byte ^ XOR_BYTE;

        return 2;
    }
    else
    {
        *data = byte;
        return 1;
    }
}

int byteDestuffing(unsigned char *data, unsigned char byte)
{
    static int doDestuffing = FALSE;
    if (byte == EXCAPE_CHAR)
    {
        doDestuffing = TRUE;
        return 1;
    }
    else if (doDestuffing == TRUE)
    {
        *data = byte ^ XOR_BYTE;
        doDestuffing = FALSE;
    }
    else
    {
        *data = byte;
    }
    return 0;
}

int processStart(unsigned char byte)
{
    if (byte == FLAG_VALUE)
    {
        state = ADDRESS;
    }
    return 0;
}

int processAddress(unsigned char byte)
{
    if (byte == ADDRESS_SET)
    {
        state = CTRL;
    }
    else if (byte == FLAG_VALUE)
    {
        perror("Receive the flag value again when should receive address.\n");
        state = ADDRESS;
    }
    else
    {
        perror("Didn't receive a valid address value.\n");
        state = START;
        return -1;
    }
    return 0;
}

int processControl(unsigned char byte, int curr_frame)
{
    if (byte == CTRL_I(curr_frame))
    {
        state = BCC1;
    }
    else if (byte == FLAG_VALUE)
    {
        perror("Receive the flag value again when should receive control.\n");
        state = ADDRESS;
    }
    else if (byte == CTRL_I(curr_frame ^ 1))
    {
        perror("Recived the control from the previous frame\n");
        state = START;
    }
    else
    {
        perror("Receive the wrong control.\n");
        state = START;
        return -1;
    }
    return 0;
}

int processBCC1(unsigned char byte, int curr_frame)
{
    if (byte == (ADDRESS_SET ^ CTRL_I(curr_frame)))
    {
        state = DATA;
    }
    else
    {
        perror("Receive the wrong BCC1.\n");
        state = START;

    }
    return 0;
}

int processData(unsigned char byte, unsigned char *payload)
{
    // questão de receber a trama de controlo na data não damos erro e da return -1, rejeita a trama!
    // ou caso de so receber o bcc2!
    if (byte == FLAG_VALUE)
    {
        if (BCC2 == 0)
        {
            int ret = --pos;
            pos = 0;
            state = START;
            return ret;
        }
        else
        {
            perror("The BCC2 was not correct\n");
            state = START;
            BCC2 = 0;
            pos = 0;
            return -1;
        }
    }
    if (pos >= MAX_PAYLOAD_SIZE + 1)
    {
        state = START;
        pos = 0;
        perror("The data size was bigger than the maximum.\n");
        return -1;
    }
    if (byteDestuffing(&payload[pos], byte) != 0)
    {
        return 0;
    }
    BCC2 ^= payload[pos];
    pos++;
    return 0;
}

int processByte(unsigned char byte, unsigned char *payload, int curr_frame)
{

    switch (state)
    {
    case START:
        return processStart(byte);
        break;
    case ADDRESS:
        return processAddress(byte);
        break;
    case CTRL:
        return processControl(byte, curr_frame);
        break;
    case BCC1:
        return processBCC1(byte, curr_frame);
        break;
    case DATA:
        return processData(byte, payload);
        break;
    default:
        state = START;
        return -1;
        break;
    }
}
