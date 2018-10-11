#ifndef INBOX_H
#define INBOX_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct Inbox {
    struct User *users;
    int usersSize;
} inbox;

struct User {
    
    char *username;
    int socket;
    
    struct InfoSocket *infoSocket;
    struct OfflineMessage *firstMessage;
    struct User *next;
    
};

struct InfoSocket {
    char *address;
    char *port;
};

struct OfflineMessage {
    char *username;
    struct Message *msg;
    int count;
    struct OfflineMessage *next;
};

struct Message {
    char *text;
    struct Message *next;
};

/** Inizializzazione dell'inbox
 *
 */
void initInbox();

/** Inserimento in lista di un nuovo utente registrato
 *  int addUser(char *username, char *address, char *port)
 *      @username puntatore a carattere dello username dell'utente da aggiungere
 *      @address puntatore a carattere dell'indirizzo dell'utente da aggiungere
 *      @port puntatore a carattere della porta dell'utente da aggiungere
 *  retistuisce 0 se esiste un utente con lo stesso username già connesso,
 *  1 se l'utente viene inserito ex-novo
 *  2 se l'utente si riconette
 */
int addUser(char *username, char *address, char *port);

/** Rimozione dalla lista di un utente
 *  int removeUser(char *username)
 *      @username puntatore a carattere dello username dell'utente da aggiungere
 */
void removeUser(char *username);

/** Inserimento nella lista dei messaggi di username del messaggio text di sender
 *  void pushMessage(char *username, char *sender, char *text);
 *
 */
void pushMessage(char *username, char *sender, char *text);
struct OfflineMessage* popMessage(char *username);

void setOffline(char *username);
void printInbox();

/****************************************************************
 * Funzioni di utilità per l'allocazione e la deallocazione degli utenti
 ****************************************************************/
struct User* findUser(char *username, int sock);
struct User* allocUser(char *username, char *address, char *port);
void updateUserInfo(struct User *user, char *address, char *port);
void freeUser(struct User*);

/****************************************************************
 * Funzioni di utilità per l'allocazione e la deallocazione dei messaggi
 ****************************************************************/
struct OfflineMessage* allocOfflineMessage(char *username);
void freeOfflineMessage(struct OfflineMessage*);
struct Message* allocMessage(char *text);

#endif
