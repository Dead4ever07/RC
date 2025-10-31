#ifndef _ALARM_H_
#define _ALARM_H_

#include <signal.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "statistics.h"




void alarmSetup();
void alarmHandler(int signal);



#endif