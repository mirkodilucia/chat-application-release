#ifndef SHARED_H
#define SHARED_H

#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
 
char *receiveUsername(int);
void sendUsername(int, char*);

unsigned int receiveSize(int);
void sendSize(int, unsigned int);

void sendString(int, char*);
char *receiveString(int);

#endif
