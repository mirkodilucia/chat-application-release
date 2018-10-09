#include "inbox.h"


void initInbox() {
    inbox.users = NULL;
    inbox.usersSize = 0;
}


int addUser(char *username, char *address, char *port) {
    struct User *newUser = allocUser(username, address, port);
    
    newUser->next = NULL;
    
    if (inbox.users == NULL) {
        inbox.users = newUser;
        inbox.usersSize++;
        return 1;
    }
    
    struct User *user = inbox.users;
    struct User *tmp = NULL;
    while (user != NULL) {
        if (!strcmp(user->username, username)) {
            if (user->infoSocket != NULL) {
                return 0;
            }else{
                updateUserInfo(user, address, port);
                return 2;
            }
            
        }
        tmp = user;
        user = user->next;
    }
    
    tmp->next = newUser;
    inbox.usersSize++;
    
    return 1;
}

int updateUserConnection() {
    return 0;
}

void removeUser(char *username) {
    
    struct User* users = inbox.users;
    struct User* prec = NULL;
    
    for (; users; prec=users, users=users->next) {
        if (!strcmp(username, users->username)) {
            break;
        }
    }
    
    if (users) {
        if (users == inbox.users) {
            inbox.users = users->next;
        }else{
            prec->next = users->next;
        }
    }
    
}

void pushMessage(char *username, char *sender, char *text) {
    
    struct User *user = findUser(username, -1);
    
    struct OfflineMessage *search = user->firstMessage;
    for (; search; search=search->next) {
        if (!strcmp(search->username, sender)) {
            break;
        }
    }
    
    struct Message *append = allocMessage(text);
    
    if (search) {
        struct Message *messageList = search->msg;
    
        for (; messageList->next; messageList = messageList->next);
    
        if (messageList) {
            messageList->next = append;
            append->next = NULL;
        }else{
            messageList = append;
            append->next = NULL;
        }
    }else{
        struct OfflineMessage *offlineMessage =  allocOfflineMessage(sender);
        struct Message *append = allocMessage(text);
        
        if (user->firstMessage) {
            offlineMessage->next = user->firstMessage;
        }
        user->firstMessage = offlineMessage;

        offlineMessage->msg = append;
        append->next = NULL;
        
    }
}

struct OfflineMessage* popMessage(char *username) {
    
    struct User *user = findUser(username, -1);
    
    struct OfflineMessage *msg = user->firstMessage;
    user->firstMessage = msg->next;
    
    return msg;
}

void setOffline(char *username) {
    
    struct User* users = findUser(username, -1);

    free(users->infoSocket->address);
    free(users->infoSocket->port);
    free(users->infoSocket);
        
    users->infoSocket = NULL;
    users->firstMessage = NULL;
        
    return;
}

void printInbox() {
    struct User *user = inbox.users;
    
    printf("Who users (%d) -------------------- \n", inbox.usersSize);
    while (user != NULL) {
        printf("%s\n", user->username);
        
        if (user->firstMessage) {
            struct OfflineMessage *offlineMessage = user->firstMessage;
            while (offlineMessage) {
             
                printf("\tMessage %s from %d users -------------------- \n", offlineMessage->username, offlineMessage->count);
            
                struct Message *message = offlineMessage->msg;
                while (message) {
                    printf("\t\t%s\n", message->text);
                    message = message->next;
                }
             
                offlineMessage = offlineMessage->next;
            }
        }else{
            printf("\tNo Message -------------------- \n");
        }
        
        user = user->next;
    }
}

/****************************************************************
 * Funzioni di utilità per l'allocazione e la deallocazione degli utenti
 ****************************************************************/

struct User* findUser(char *username, int sock) {
    struct User *user = inbox.users;
    for (; user; user=user->next) {
        if (sock != -1) {
            if (user->socket == sock)
                break;
        }else{
            if (!strcmp(user->username, username))
                break;
        }
        
    }
    
    return user;
}

struct User* allocUser(char *username, char *address, char *port) {
    
    struct User *user;
    
    user = malloc(sizeof(struct User));
    
    user->username = malloc(sizeof(username) + 1);
    strcpy(user->username, username);
    
    user->infoSocket = malloc(sizeof(struct InfoSocket));
    user->infoSocket->address = malloc(sizeof(address) + 1);
    user->infoSocket->port = malloc(sizeof(port) + 1);
    
    strcpy(user->infoSocket->address, address);
    strcpy(user->infoSocket->port, port);
    
    user->firstMessage = NULL;
    
    return user;
}

void updateUserInfo(struct User *user, char *address, char *port) {
    
    user->infoSocket = malloc(sizeof(struct InfoSocket));
    user->infoSocket->address = malloc(sizeof(address) + 1);
    user->infoSocket->port = malloc(sizeof(port) + 1);
    
    strcpy(user->infoSocket->address, address);
    strcpy(user->infoSocket->port, port);
    
}

void freeUser(struct User *user) {
    free(user->username);
    free(user->infoSocket->address);
    free(user->infoSocket->port);
    free(user->infoSocket);
}


/****************************************************************
 * Funzioni di utilità per l'allocazione e la deallocazione dei messaggi
 ****************************************************************/

struct OfflineMessage* allocOfflineMessage(char *username) {
    struct OfflineMessage *offlineMessage = malloc(sizeof(struct OfflineMessage));
    
    offlineMessage->username = malloc(sizeof(username) + 1);
    strcpy(offlineMessage->username, username);
    offlineMessage->count = 0;
    offlineMessage->next = NULL;
    offlineMessage->msg = NULL;
    
    return offlineMessage;
}

struct Message* allocMessage(char *text) {
 
    struct Message *message = malloc(sizeof(struct Message));
    message->text = malloc(sizeof(text) + 1);
    message->next = NULL;
    
    strcpy(message->text, text);
    
    return message;
}

void freeOfflineMessage(struct OfflineMessage *message) {
    
    struct Message *msg = message->msg;
    while(msg) {
        free(msg->text);
        
        struct Message *m = msg;
        msg = msg->next;
        
        free(m);
    }
    free(message->username);
    
}
