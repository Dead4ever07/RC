#ifndef __DEBUG__
#define __DEBUG__
#include <stdio.h>

/// @brief function responsible of printing a human friendly error
/// @param funcName function where the error ocured 
/// @param errorMessage message of the error
void printError(const char* funcName, const char* errorMessage);


#endif