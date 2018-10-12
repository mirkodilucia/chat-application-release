#include "server.h"

int main(int argc, char **argv) {

	int listener, new_sock;
	
	// Socket di invio e ricezione
	struct sockaddr_in my_addr;
	struct sockaddr_in cl_addr;

	// Variabili accessorie per la gestione delle connessioni con i client
	int i, fdmax, result;
	unsigned int addrlen;

	char buffer[BUFFER_SIZE];

	// SET di socket per la listen
	fd_set master, master_cpy;

	listener = 	socket(AF_INET, SOCK_STREAM, 0);
    
    FD_ZERO(&master);
    FD_ZERO(&master_cpy);

	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(atoi(argv[1]));
	my_addr.sin_addr.s_addr = INADDR_ANY;
    
	if (bind(listener, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0) {
		perror("Bind error");
		return 1;
	}else{
		logger("Bind eseguita correttamenete");
	}

	if (listen(listener, 10) < 0) {
		perror("Listen error");
		return 1;
	}

	FD_SET(listener, &master);
	fdmax = listener;

	for (;;) {
		master_cpy = master;
		select(fdmax + 1, &master_cpy, NULL, NULL, NULL);

		for (i=0; i<=fdmax; ++i) {
			// Descrittore pronto
			if (FD_ISSET(i, &master_cpy)) {
        
                // Nuova connessione
                if (i==listener) {
                    
                    addrlen = (sizeof(cl_addr));
                    new_sock = accept(listener, (struct sockaddr*)&cl_addr, &addrlen);
                    
                    FD_SET(new_sock, &master);
                    fdmax = (new_sock > fdmax)?new_sock:fdmax;
                    
                    logger("Nuovo client connesso");
                }else{
                    
                    memset(buffer, 0, BUFFER_SIZE);
                    command(i, buffer, &master);
                    
                    if (!strcmp(buffer, "!register\0")) {
                        logger("Register\n");
                        result = registerCommand(i);
                        sendSize(i, result);
                        
                        if (result == 2) {
                            sendOfflineMessage(i);
                        }
                    }else if (!strcmp(buffer, "!who\0")) {
                        logger("Who\n");
                        
                        whoCommand(i);
                        
                    }else if (!strcmp(buffer, "!quit\0")) {
                        logger("Quit\n");
                        
                        quitCommand(i);
                        close(i);
                        FD_CLR(i, &master);
                        
                    }else if (!strcmp(buffer, "!deregister\0")) {
                        logger("Deregister\n");
                        deregisterCommand(i);
                    }else if (!strcmp(buffer, "!send\0")) {
                        logger("Send\n");
                        sendCommand(i);
                    }

					printInbox();
                }
            }
        }
    }
    
    close(listener);
}

void command(int sock, char *buffer, fd_set *master) {
    uint16_t length;
	int ret, registrato = 0;

	memset(&length, 0, sizeof(uint16_t));

	ret = recv(sock, (void*)&length, sizeof(uint16_t), 0);

	if (ret < 0) {
		perror("Errore durante la ricezione della lunghezza del comando");
		return;
	}else if (!ret) {

        struct User *disconnettere = findUser(NULL, sock);
        
        if (registrato) {
            setOffline(disconnettere->username);
        }

 		logger("Un client registrato sta per essere disconnesso");
        
        FD_CLR(sock, master);
        close(sock);
        return;
    }
    
    // Ricezione del comando
    if (recv(sock, (void*)buffer, ntohs(length), 0) < 0) {
        perror("Comando non ricevuto");
    }
    
    return;
}

int registerCommand(int sock) {

    char *username = receiveUsername(sock);
    char *address = receiveString(sock);
    char *port = receiveString(sock);

    int result = addUser(username, address, port);
    return result;
}

void sendCommand(int sock) {
 
    char *username = receiveUsername(sock);
    struct User *user = findUser(username, -1);
    
    // Utente non trovato
    if (!user) {
        free(username);
        sendSize(sock, 0);
        return;
    }
    
    // Utente offline
    if (!user->infoSocket) {
        sendSize(sock, 1);
		
        char *sender = receiveUsername(sock);
        char *buffer = receiveString(sock);
        
        pushMessage(username, sender, buffer);

		free(sender);
		free(buffer);
        
    }else{
    // Utente online
        sendSize(sock, 2);
        
        sendString(sock, user->infoSocket->address);
        sendString(sock, user->infoSocket->port);

    }
    
	//freeUser(user);
    free(username);
}

void whoCommand(int sock) {
    
    unsigned int status;
    
    struct User *users = inbox.users;
    
    for (;users; users = users->next) {
        sendSize(sock, 1);
        sendUsername(sock, users->username);
        
        status = (users->infoSocket == NULL)?0:1;
        sendSize(sock, status);
    }
    
    sendSize(sock, 0);
}

void sendOfflineMessage(int sock) {
    
    char *username = receiveUsername(sock);
    struct OfflineMessage *offlineMsg;
	struct Message *delMessage;

    while ((offlineMsg = popMessage(username))) {
        sendSize(sock, 1);
        sendUsername(sock, offlineMsg->username); 
        
        struct Message *msg = offlineMsg->msg;
        while (msg) {
            sendSize(sock, 1);
            sendString(sock, msg->text);

			delMessage = msg;
            msg = msg->next;

			free(delMessage->text);
			free(delMessage);
        }
		
        
        sendSize(sock, 0);
    }
	
	
    sendSize(sock, 0);
}

void deregisterCommand(int sock) {
    char *username = receiveUsername(sock);
    removeUser(username);
}

void quitCommand(int sock) {
    
    char *username;
    username = receiveUsername(sock);
    
    if (username) {
        setOffline(username);
    }
    
    logger("Disconnessione di un client non registrato");
    
}

void logger(const char *msg){
    time_t current_time;
    char ti[10];
    
    struct tm* time_info;
    
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(ti, 26, "[%Y-%m-%d %H:%M:%S]", time_info);
    
    printf("%s %s\n", ti, msg);
    
    return;
}
