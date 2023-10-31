#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

void sendAll(int id, int max, int *clients, char *msg){
	for (int i = 0; i <= max; i++)
		if (clients[i] != id)
			send(clients[i], msg, strlen(msg), SO_NOSIGPIPE);
}

int getId(int id, int max, int *clients){
	for (int i = 0; i <= max; i++)
		if (clients[i] == id)
			return i;
	return -1;
}

void write_error(char *msg){
	write(2, msg, strlen(msg));
	exit(1);
}


int main(int argc, char **argv) {
	if (argc != 2)
		write_error("Wrong number of arguments\n");

	int clients[FD_SETSIZE], id = 0, sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	fd_set active, ready;
	char buffer[200000];

	if (sockfd < 0)
		write_error("Fatal error\n");

	struct sockaddr_in servaddr = {0};
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1])); 
  
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0)
		write_error("Fatal error\n");

	if (listen(sockfd, SOMAXCONN) < 0)
		write_error("Fatal error\n");

	int max = sockfd;
	FD_ZERO(&active);
	FD_SET(sockfd, &active);

	while (1) {
		ready = active;
		if (select(max + 1, &ready, NULL, NULL, NULL) < 0)
			write_error("Fatal error\n");

		for (int i = 0; i <= max; i++){
			if (FD_ISSET(i, &ready)){
				if (i == sockfd){
					int client = accept(sockfd, NULL, NULL);
					if (client < 0)
						continue;
					FD_SET(client, &active);
					max = client > max ? client : max;
					clients[id] = client;
					sprintf(buffer, "server: client %d just arrived\n", id++);
					sendAll(client, id, clients, buffer);
				} else {
					int read = recv(i, buffer, sizeof(buffer), 0);

					if (read <= 0){
						FD_CLR(i, &active);
						close(i);
						sprintf(buffer, "server: client %d just left\n", getId(i, id, clients));
						clients[getId(i, id, clients)] = 0;
						sendAll(i, id, clients, buffer);
					} else {
						char temp[200000];
						buffer[read] = '\0';
						if (buffer[read - 1] != '\n'){
							buffer[read] = '\n';
							buffer[read + 1] = '\0';
						}
						sprintf(temp, "client %d: %s", getId(i, id, clients), buffer);
						sendAll(i, id, clients, temp);
					}
				}
			}
		}		
	}
	return 0;
}
