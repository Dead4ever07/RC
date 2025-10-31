#ifndef _ALARM_H_
#define _ALARM_H_

#include <signal.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "../macros.h"
#include "statistics.h"
#include "../debug.h"



///@brief Signal handler function for the alarm signal (SIGALRM).
///@param signal The signal number received (should be SIGALRM).
void alarmHandler(int signal);


///@brief Installs `alarmHandler` as the SIGALRM handler.
void alarmSetup();



#endif