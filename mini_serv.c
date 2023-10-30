#include <string.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

void writeError(char *msg){
	write(2, msg, strlen(msg));
	exit(1);
}

void send_all(char *msg, int clients[1000], int id, int max){
	for (int i = 0; i < max; i++)
		if (clients[i] != id)
			send(clients[i], msg, strlen(msg), SO_NOSIGPIPE);
}

int getId(int id, int maxId, int *clients){
	for (int i = 0; i < maxId; i++)
		if (clients[i] == id)
			return i;
	return -1;
}

int main(int argc, char **argv) {

	if (argc != 2)
		writeError("Wrong number of arguments\n");

	int clients[FD_SETSIZE], id = 0, server = socket(AF_INET, SOCK_STREAM, 0);
	fd_set active, ready;
	char buffer[200000];

	if (server== -1)
		writeError("Fatal Error\n");

	struct sockaddr_in addres = {0};

	addres.sin_family = AF_INET; 
	addres.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	addres.sin_port = htons(atoi(argv[1])); 

	if ((bind(server, (const struct sockaddr *)&addres, sizeof(addres))) < 0)
		writeError("Fatal Error\n");

	if (listen(server, SOMAXCONN) < 0)
		writeError("Fatal Error\n");

	FD_ZERO(&active);
	FD_SET(server, &active);
	int max = server;

	while(1) {
		ready = active;
		if (select(max + 1, &ready, NULL, NULL, NULL) < 0)
				writeError("Fatal Error\n");

		for (int socketId = 0; socketId <= max; socketId++){
			if (FD_ISSET(socketId, &ready)){
				printf("allo %d\n", socketId);
				if (socketId == server){
					int client = accept(server, NULL, NULL);
					if (client > 0){
						FD_SET(client, &active);
						max = client > max ? client : max;
						sprintf(buffer, "server: client %d just arrived\n", id);
						send_all(buffer, clients, socketId, id);
						clients[id++] = client;
					}
				} else{
					int read = recv(socketId, buffer, sizeof(buffer), 0);

					if (read <= 0){
						sprintf(buffer, "server: client %d just left\n", getId(socketId, id, clients));
						send_all(buffer, clients, socketId, id);
						close(socketId);
						clients[getId(socketId, id, clients)] = 0;
						FD_CLR(socketId, &active);
					}
					else{
						char temp[200000];
						buffer[read] = '\0';
						if (buffer[read - 1] != '\n'){
							buffer[read] = '\n';
							buffer[read + 1] = '\0';
						}
						sprintf(temp, "client: %d: %s", getId(socketId, id, clients), buffer);
						send_all(temp, clients, socketId, id);
					}
				}
			}
		}
	}
	return 0;
}
