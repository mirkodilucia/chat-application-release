#include <stdio.h>
#include <stdlib.h>

#include "inbox.h"

int main(int argc, char **argv) {
    
    initInbox();
    int r1 = addUser("ciao", "127.0.0.1", "3000");
    int r2 = addUser("ciao1", "127.0.0.1", "3000");
    int r3 = addUser("ciao2", "127.0.0.1", "3000");
    int r4 = addUser("ciao3", "127.0.0.1", "3000");
    int r5 = addUser("ciao4", "127.0.0.1", "3000");
    
    int rn = addUser("ciao5", "127.0.0.1", "3000");
    
    pushMessage("ciao", "ciao2", "Ciao come stai");
    pushMessage("ciao", "ciao2", "Noooooo");
    pushMessage("ciao", "ciao3", "Aaaaaaaaa");
    
    // Estrazione dei messaggi
    
    
    printInbox();
    
    
    removeUser("ciao");
    removeUser("ciao1");
    
    removeUser("ciao3");
    
    removeUser("ciao2");
    removeUser("ciao4");
    removeUser("ciao5");
    removeUser("ciao5");
    
    
    printInbox();
    
}
