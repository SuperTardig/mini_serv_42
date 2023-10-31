#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

void sendAll(int id, int max, int *clients, char *msg){
	for (int i = 0; i < max; i++)
		if (clients[i] != id)
			send(clients[i], msg, strlen(msg), SO_NOSIGPIPE);
}

int getId(int id, int max, int *clients){
	for (int i = 0; i < max; i++)
		if (clients[i] == id)
			return i;
	return -1;
}

void writeError(char *msg){
	write(2, msg, strlen(msg));
	exit(1);
}

int main(int argc, char **argv) {
	if (argc != 2)
		writeError("Wrong number of arguments\n");

	int maxId = 0, clients[FD_SETSIZE], sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	fd_set active, ready;
	char buffer[200000];
	if (sockfd < 0)
		writeError("Fatal error\n");

	struct sockaddr_in servaddr = {0}; 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1])); 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) 
		writeError("Fatal error\n");
	if (listen(sockfd, SOMAXCONN) < 0) 
		writeError("Fatal error\n");

	FD_ZERO(&active);
	FD_SET(sockfd, &active);
	int max = sockfd;

	while (1){
		ready = active;
		if (select(max + 1, &ready, NULL, NULL, NULL) < 0)
			writeError("Fatal error\n");
		for (int id = 0; id <= max; id++){
			if (FD_ISSET(id, &ready)){
				if (id == sockfd){
					int client = accept(sockfd, NULL, NULL);
					if (client < 0)
						continue;
					max = client > max ? client : max;
					FD_SET(client, &active);
					clients[maxId] = client;
					sprintf(buffer, "server: client %d just arrived\n", maxId++);
					sendAll(client, maxId, clients, buffer); 
				} else {
					int read = recv(id, buffer, sizeof(buffer), 0);
					if (read <= 0){
						FD_CLR(id, &active);
						close(id);
						sprintf(buffer, "server: client %d just left\n", getId(id, maxId, clients));
						clients[getId(id, maxId, clients)] = 0;
						sendAll(id, maxId, clients, buffer);
					} else {
						char temp[200000];
						buffer[read] = '\0';
						if (buffer[read - 1] != '\n')
							sprintf(temp, "client %d: %s\n", getId(id, maxId, clients), buffer);
						else
							sprintf(temp, "client %d: %s", getId(id, maxId, clients), buffer);
						sendAll(id, maxId, clients, temp);
					}
				}
			}
		}
	}
}
