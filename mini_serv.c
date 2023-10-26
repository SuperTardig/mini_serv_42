#include <string.h>#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

void writeError(char *msg){
	write(2, *msg, strlen(msg));
	exit(1);
}

void send_all(char *msg, int clients[1000], int id, int max){
	for (int i = 0; i < max; id++)
		if (clients[i] != id)
			send(clients[id], buffer, strlen(buffer), 0);
}

int main(int argc, char **argv) {

	if (argc != 2)
		writeError("Wrong number of arguments\n")

	int clients[1000], id = 0, socket = socket(AF_INET, SOCK_STREAM, 0);
	fd_set active, ready;
	char buffer[200000];

	if (sockfd == -1)
		writeError("Fatal Error\n");

	struct sockaddr_in addres = {0};

	addres.sin_family = AF_INET; 
	addres.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addres.sin_port = htons(atoi(argv[1])); 

	if ((bind(socket, (const struct sockaddr *)&addres, sizeof(addres))) < 0)
		writeError("Fatal Error\n");

	if (listen(socket, 1000) < 0)
		writeError("Fatal Error\n");

	FD_ZERO(&active)
	FD_SET(socket, &active)
	int max = socket;

	while(1) {
		ready = active
		if (select(max + 1, &ready, NULL, NULL, NULL) < 0)
				writeError("Fatal Error\n");

		for (int socketId = 0; socketId < max; socketId++){
			if (FD_ISSET(socketId, &ready)){
				if (socketId == socket){
					int client = accept(socket, NULL, NULL);
					if (client < 0)
								writeError("Fatal Error\n");
					FD_SET(client, &active)
					max = client > max ? client : max;
					sprintf(buffer, "server: client %d just arrived\n", id);
					send_all(buffer, clients, socketId, id);
					clients[id++] = client;
				} else{
					int read = recv(socketId, buffer, sizeof(buffer), -1, 0);

					if (read <= 0){
						sprintf(buffer, "server: client %d just left\n", socketId);
						send_all(buffer, clients, socketId, id);
						close(socketId);
						FD_CLR(socketId, &active);
					}
					else{
						buffer[read] = '\0';
						sprintf(buffer, "client: %d: %s\n", socketId, buffer);
						send_all(buffer, client, socketId, id);
					}
				}
			}
		}
	}
	return 0;
	}
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_CLIENTS 128
#define BUFFER_SIZE 200000

int main(int argc, char **argv) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int clientSockets[MAX_CLIENTS];
    int next_id = 0;
    fd_set activeSockets, readySockets;
    char buffer[BUFFER_SIZE];
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) 
    {
        perror("Error creating server socket");
        exit(1);
    }

    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serverAddress.sin_port = htons(atoi(argv[1]));

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) 
    {
        perror("Error binding server socket");
        exit(1);
    }

    if (listen(serverSocket, MAX_CLIENTS) < 0) 
    {
        perror("Error listening on server socket");
        exit(1);
    }

    FD_ZERO(&activeSockets);
    FD_SET(serverSocket, &activeSockets);
    int maxSocket = serverSocket;

    while (1) 
    {
        readySockets = activeSockets;
        if (select(maxSocket + 1, &readySockets, NULL, NULL, NULL) < 0) 
        {
            perror("Error in select");
            exit(1);
        }

        for (int socketId = 0; socketId <= maxSocket; socketId++) 
        {
            if (FD_ISSET(socketId, &readySockets)) 
            {
                if (socketId == serverSocket) 
                {
                    int clientSocket = accept(serverSocket, NULL, NULL);
                    if (clientSocket < 0) 
                    {
                        perror("Error accepting client connection");
                        exit(1);
                    }

                    FD_SET(clientSocket, &activeSockets);
                    maxSocket = (clientSocket > maxSocket) ? clientSocket : maxSocket;
                    sprintf(buffer, "server: client %d just arrived\n", next_id);
                    send(clientSocket, buffer, strlen(buffer), 0);
                    clientSo-ckets[next_id++] = clientSocket;

                } 
                else 
                {
                    int bytesRead = recv(socketId, buffer, sizeof(buffer) - 1, 0);

                    if (bytesRead <= 0) 
                    {
                        sprintf(buffer, "server: client %d just left\n", socketId);

                        for (int i = 0; i < next_id; i++)
                        {
                            if (clientSockets[i] != socketId) 
                            {
                                send(clientSockets[i], buffer, strlen(buffer), 0);
                            }
                        }
                        close(socketId);
                        FD_CLR(socketId, &activeSockets);
                    } 
                    else 
                    {
                        buffer[bytesRead] = '\0';
                        sprintf(buffer, "client %d: %s\n", socketId, buffer);
                        for (int i = 0; i < next_id; i++) 
                        {
                            if (clientSockets[i] != socketId) 
                            {
                                send(clientSockets[i], buffer, strlen(buffer), 0);
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
