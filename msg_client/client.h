#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

#include "shared.h"

#define CMD_SIZE 60
#define BUFFER_SIZE 1024

struct thread_args {
	char *ip;
	char *port;
	char *username;
	int  *UDP_socket;
};

void initMessage(const char*, const char*, const char*);
void printCommand();

void splitCommand(const char *command, char ** arg_command);
int registerCommand(int sock, char *username, char *ip, char *port);
void sendOnline(int sock, char *username);
void sendOffline(int sock, char *username);
void whoCommand(int sock, char *myUsername);
void receiveOfflineMessage(int sock);

void *receiveUDP(void*);

#endif
