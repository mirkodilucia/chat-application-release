#include "shared.h"

void sendUsername(int sock, char *username) {

	uint16_t length;
	uint16_t lenc = (username)?strlen(username) + 1 : 0;

	length = htons(lenc);
	
	while (send(sock, (void *)&length, sizeof(uint16_t), 0) < 0) {
		perror("Errore durante l'invio della lunghezza dei dati");
	}

	//printf("%s\n", username);

	if (lenc) {
		while (send(sock, (void*)username, lenc, 0) < 0) {
			perror("Errore nell'invio dell'username'");
		}
	}

}

char *receiveUsername(int sock) {

	uint16_t length;
	char *username = NULL;

	while (recv(sock, (void *)&length, sizeof(uint16_t), 0) < 0) {
		perror("Errore durante la ricezione della lunghezza del dati");
	}

	//printf("Lunghezza %d\n", length);

	if (ntohs(length)) {
		username = malloc(ntohs(length));
		memset(username, 0, ntohs(length));
		while (recv(sock, (void *)username, ntohs(length), 0) < 0) {
			perror("Errore duratne la ricezione dei dati");
		}
	}

	//printf("Ricevuto %s\n", username);

	return username;
}

void sendSize(int sock, unsigned int val) {

	unsigned int uint = htons(val);

	while(send(sock, &uint, sizeof(unsigned int), 0) < 0) {
		perror("Errore durante l'invio' della dimensione dei dati");
	}

}

unsigned int receiveSize(int sock) {

	unsigned int uint;

	while (recv(sock, &uint, sizeof(unsigned int), 0) < 0) {
		perror("Errore durante la ricezione della lunghezza dei dati");
	}

	return ntohs(uint);
}

void sendString(int sock, char *buffer) {

	unsigned int ltos, length;

	ltos = strlen(buffer) + 1;
	length = htons(ltos);

	while(send(sock, &length, sizeof(uint16_t), 0) < 0) {
		perror("Errore durante l'invio della dimensione dei dati");
	}

	while(send(sock, buffer, ltos, 0) < 0) {
		perror("Errore durante l'invio dei dati");
	}
}

char *receiveString(int sock) {
	unsigned int length;
	char *buffer;

	while (recv(sock, &length, sizeof(uint16_t), 0) < 0) {
		perror("Errore durante la ricezione della lunghezza dei dati");
	}

	buffer = malloc(ntohs(length));
	memset(buffer, 0, ntohs(length));

	while (recv(sock, buffer, ntohs(length), 0) < 0) {
		perror("Errore nel ricevere il messaggio");
	}

	return buffer;
}
