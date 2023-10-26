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

