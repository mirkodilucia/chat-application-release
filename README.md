# Chat Application

Computer networking project assigned in 2018 by University of Pisa Computer Engineering course.
The project consist of client and server programs that can run on different system.
Client program comunicate with server for registration, and retrieving user's informations like IP address and open PORT for listening.
More details are available on course [site](http://www2.ing.unipi.it/c.vallati/files/reti/Progetto2017.pdf), italian only.

#### TL;DR

Chat application (client and server) written in C, using UNIX socket.

###### Fast start
To try application follow this steps
    
1. Clone project
2. Navigate inside project's root folder and type

###### Compile
 Compile binaries
 
```sh
make all
```

###### Run server
Launch server

```sh
cd msg_server
./server 4242
```

###### Run client
Launch client
```sh
cd msg_client
./client "127.0.0.1" 8081 "127.0.0.1" 4242
```

#### Command available for client

  - **!help**
  - **!register <username>**
  - **!send <username>**
  - **!who**
  - **!deregister <username>**
  - **!quit**

# Getting started

#### Build

For building application you can type:

  - **make** or make all
  - **make server** - compile only server binaries in msg_server folder
  - **make inbox** - compile only inbox binaries for server and put it in msg_server folder
  - **make client** - compile only client binaries in msg_client folder
  - **make shared** - compile only shared binaries used in both client and server binaries
  - **make test** - run 3 client instances and server in terminals for testing porpouse

####  Run

If you want to run server application you need to specify
```sh
cd msg_server
./server <server-address> <listen-port> 
```

If you want to run client application you need to specify
```sh
cd msg_client
./client <client-address> <listen-port> <server-address> <server-port>
```
