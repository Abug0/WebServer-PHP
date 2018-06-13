#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

int error(const int errorNum, char *responseBody);
int error_404(char *responseBody);
int error_500(char *responseBody);

#endif