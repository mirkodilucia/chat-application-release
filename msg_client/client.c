#include "client.h"

int main(int argc, char **argv) {

	int sock;
	struct sockaddr_in sv_addr;

	sleep(1);

	char *cmd = malloc(CMD_SIZE);
	char *argCommand = NULL;

	char prompt[50] = "> ";
	char *username = NULL;

	pthread_t thread;
	struct thread_args args;

	if (argc != 5) {
		printf("Per avviare il programma ./client <IP locale> <porta locale> <IP server> <porta server>");
		return 0;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	memset(&sv_addr, 0, sizeof(sv_addr));
	sv_addr.sin_family = AF_INET;
	sv_addr.sin_port = htons(atoi(argv[4]));
	
	inet_pton(AF_INET, argv[3], &sv_addr.sin_addr);

	if (connect(sock, (struct sockaddr*)&sv_addr, sizeof(sv_addr)) < 0) {
		perror("Errore durante la connessione");
		exit(1);
	}

	initMessage(argv[3], argv[4], argv[2]);

	while (printf("%s", prompt) && fgets(cmd, CMD_SIZE, stdin)) {
		splitCommand(cmd, &argCommand);

		if(!strcmp("!register\0", cmd) && !argCommand){
			printf("Hai dimenticato a specificare l'username oppure contiene degli spazi\n");
			continue;
		}else if(!strcmp("!register\0", cmd) && username!=NULL){
			printf("Sei già registrato\n");
			continue;
		}else if(!strcmp("!send\0", cmd) && !username){
			printf("Non sei ancora registrato, registrati tramite !register <username>\n");
			continue;
		}else if(!strcmp("!send\0", cmd) && username){
			if(argCommand==NULL || !strlen(argCommand)){
				printf("Nessun username specificato per l'invio del messaggio\n");
				continue;
			}else if(!strcmp(username, argCommand)){
				printf("Tentativo di invio messaggio a te stesso\n");
				continue;
			}
		}

		serveCommand(sock, cmd);

		if (!strcmp("!register\0", cmd)) {

			if (!registerCommand(sock, argCommand, argv[1], argv[2])) {

				memset(prompt, 0, strlen(prompt));
				sprintf(prompt, "%s>", argCommand);
		
				username = malloc(strlen(argCommand));
				sprintf(username, "%s", argCommand);

				args.ip = malloc(strlen(argv[1]));
				args.port = malloc(strlen(argv[2]));
				args.username = malloc(strlen(username));

				sprintf(args.ip, "%s", argv[1]);
				sprintf(args.port, "%s", argv[2]);
				sprintf(args.username, "%s", username);

				pthread_create(&thread, NULL, receiveUDP, (void *)&args);

			}
		}else if (!strcmp("!who\0", cmd)) {
			whoCommand(sock, username);
		}else if (!strcmp("!help\0", cmd)) {
			printCommand(sock);
		}else if (!strcmp("!send\0", cmd)) {

			printf("\n");
			sendUsername(sock, argCommand);

			switch(receiveSize(sock)) {
				case 0:
					printf("Impossibile connettersi a %s: utente inesistente\n", argCommand);
					break;
				case 1:
					sendUsername(sock, username);
					sendOffline(sock, argCommand);
					printf("Messaggio inviato correttamente\n");
					break;
				case 2:
					sendOnline(sock, argCommand);
					printf("Messaggio istantaneo inviato correttamente\n");
					break;
			}

		}else if (!strcmp("!deregister\0", cmd)) {

			sendUsername(sock, username);
			free(username);

			username = NULL;
			memset(prompt, 0, strlen(prompt));

			free(args.username);
			free(args.ip);
			free(args.port);

			pthread_cancel(thread);
			close(*args.UDP_socket);

			free(args.UDP_socket);
	
			printf("Deregistrato correttamente\n");

			sprintf(prompt, "> ");

		}else if (!strcmp("!quit\0", cmd)) {

			sendUsername(sock, username);
			if (username)
				free(username);

			close(sock);
			printf("\nClient disconnesso\n");
			exit(0);

		}

		memset(cmd, 0, CMD_SIZE);
		free(argCommand);
		argCommand = NULL;
	}
}

/*
 * Comunicazione tra client
 *
 */

void *receiveUDP(void *args) {

	struct sockaddr_in my_addr;
	int sock;
	char *buffer, *username = NULL;
	struct thread_args* t_args = (struct thread_args*)args;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	t_args->UDP_socket = malloc(sizeof(int));
	*(t_args->UDP_socket) = sock;
	memset(&my_addr, 0, sizeof(my_addr));

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(atoi(t_args->port));
	inet_pton(AF_INET, t_args->ip, &my_addr.sin_addr);

	if (bind(sock, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0) {
		perror("Errore in fase di bind del soket UDP");
	}

	while (t_args->username != NULL && (username=receiveUsername(sock))) {
		buffer = receiveString(sock);

		printf("\n%s (msg istantaneo)>\n%s\n", username, buffer);
		free(username);
		free(buffer);
		printf("%s>", t_args->username);
		fflush(stdout);
	}

	return NULL;

}

void receiveOfflineMessage(int sock, char *username) {
    
	sendUsername(sock, username);

	char msg[BUFFER_SIZE];
	char *sender;
	char *buffer;
	
	while (receiveSize(sock)) {
		sender = receiveUsername(sock);

		memset(msg, 0, sizeof(msg));
		sprintf(msg, "%s (msg offline)>", sender);
		printf("%s\n", msg);

		while (receiveSize(sock)) {
			memset(msg, 0, BUFFER_SIZE);
			buffer = receiveString(sock);
			printf("%s\n", buffer);
			free(buffer);
		}

		free(sender);

	}

}

void splitCommand(const char *command, char ** arg_command) {
    
    char *tmp;
    *(strchr(command, '\n')) = '\0';
    
    tmp = strchr(command, ' ');
    if (tmp) {
        *tmp = '\0';
    }else{
        return;
    }
    
    
    if (!(int)strlen(tmp + 1) || strchr(tmp+1, ' ')) {
        return;
    }
    
    *arg_command = malloc(strlen(tmp+1) + 1);
    sprintf(*arg_command, "%s", tmp + 1);
    
}

void serveCommand(int sock, char *cmd) {

	uint16_t length = htons(strlen(cmd) + 1);

	
	if (send(sock, (void*)&length, sizeof(uint16_t), 0) < 0) {
		perror("Errore durante l'invio della lunghezza del comando");
		return;
	}
	

	if (send(sock, (void*)cmd, strlen(cmd) + 1, 0) < 0) {
		perror("Errore durante l'invio del comando");
		return;
	}

}

int registerCommand(int sock, char *username, char *ip, char *port) {
    unsigned int result;
    
    // Invio username
    sendUsername(sock, username);
    
    // Invio IP e porta
    sendString(sock, ip);
    sendString(sock, port);
    
    result = receiveSize(sock);
    switch(result) {
    	case 0:
            printf("Registrato correttamente\n");
        	break;
        case 1:
            printf("Registrazione fallita, ti sei già registrato\n");
            return 1;
            break;
        case 2:
            printf("Riconnessione completata\n");
            receiveOfflineMessage(sock, username);
            break;
    }
    return 0;
}

void whoCommand(int sock, char *myUsername) {
    char *username;
    unsigned int state = 0;
    
    char **status = malloc(sizeof(char*));
    status[0] = malloc(strlen("Offline"));
    status[1] = malloc(strlen("Online"));
    
    strcpy(status[0], "Offline");
    strcpy(status[1], "Online");
    
    printf("Client registrati:\n");
    while(receiveSize(sock)) {
		
        username = receiveUsername(sock);
        memset(&state, 0, sizeof(unsigned int));
        state = receiveSize(sock);
        
        if (myUsername && !strcmp(username, myUsername)) {
            free(username);
            continue;
        }
        
        printf("\t%s (%s)\t\n", username, status[state]);
        free(username);
		
    }
    
    free(status[0]);
    free(status[1]);
    free(status);
    
}

void sendOnline(int sock, char *username) {
    
    char *ip;
    char *port;
    
    char buffer[BUFFER_SIZE];
    struct sockaddr_in sv_addr;
    
    int sock_msg;
    
    // Ricezione separata di ip e port UPD del client da contattare
    ip = receiveString(sock);
    port = receiveString(sock);
    
    writeMessagePrompt(buffer);
    
    sock_msg = socket(AF_INET, SOCK_DGRAM, 0);
    
    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(atoi(port));
    
    inet_pton(AF_INET, ip, &sv_addr.sin_addr);
    
    if (connect(sock_msg, (struct sockaddr*)&sv_addr, sizeof(sv_addr)) < 0) {
        perror("Errore durante la comunicazione al client UDP");
    }
    
    sendUsername(sock_msg, username);
    sendString(sock_msg, buffer);
    close(sock_msg);
    
}

void sendOffline(int sock, char *username) {
    
    char buffer[BUFFER_SIZE];
    
    writeMessagePrompt(buffer);
    sendString(sock, buffer);
    
}

void writeMessagePrompt(char *buf) {

	char buffer[BUFFER_SIZE], tmp[BUFFER_SIZE];

	memset(buffer, 0, sizeof(buffer));
    while(fgets(tmp, BUFFER_SIZE, stdin)) {
        if (*tmp == '.' && *(tmp + 1) == '\n')
        	break;
        
        strcat(buffer, tmp);
        memset(tmp, 0, sizeof(tmp));
    }

	strcpy(buf, buffer);

}



void printCommand() {
	printf("\nSono disponibili i seguenti comandi: \n");
    printf("!help --> mostra l\'elenco dei comandi disponibili \n");
    printf("!register username --> registra il client presso il server \n");
    printf("!deregister --> de-registra il client presso il server \n");
    printf("!who --> mostra l'elenco degli utenti disponibili \n");
    printf("!send username --> invia un messaggio ad un altro utente \n");
    printf("!quit --> disconnette il client dal server ed esce \n");
}

void initMessage(const char *server, const char *s_port, const char *i_port) {
	printf("\n");
	printf("Conessione al server %s (porta %s) effettuata con successo\n", server, s_port);
	printf("Ricezione messaggi istantanei su porta %s\n\n", i_port);
	printCommand();
}
