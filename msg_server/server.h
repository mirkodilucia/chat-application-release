#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "shared.h"
#include "inbox.h"
#include <time.h>

#define BUFFER_SIZE 1024

void logger(const char *msg);
void command(int sock, char *buffer, fd_set *master);

int registerCommand(int sock);
void deregisterCommand(int sock);

void whoCommand(int sock);

void sendCommand(int sock);
void sendOfflineMessage(int sock);
void quitCommand(int sock);

#endif
