#ifndef _PACKET_H_
#define _PACKET_H_

#include <stdio.h>
#include <string.h>
#include "../link_layer/link_layer.h"


int writeControlPacket(unsigned char control, unsigned long fileSize, const char *filename);
int readControlPacket(unsigned char control, const unsigned char *buf, unsigned long *fileSize, char *filename);

#endif