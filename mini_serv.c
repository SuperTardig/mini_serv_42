#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

int sockfd;
struct sockaddr_in servaddr = {0};
int max;
int ids[FD_SETSIZE];
int maxId = 0;
fd_set active, ready;
char msg[100] = {};
char sending[2] = {};

void writeError(char* message){
	write(2, message, strlen(message));
	exit(1);
}

void sendAll(int clientFd, char* message){
	for (int fd = 0; fd < max + 1; fd++)
		if (fd != clientFd)
			send(fd, message, strlen(message), SO_NOSIGPIPE);
}

int main(int argc, char** argv) {
	if (argc != 2)
		writeError("Wrong number of arguments\n");
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1)
		writeError("Fatal error\n");
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433);
	servaddr.sin_port = htons(atoi(argv[1]));
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0 || listen(sockfd, SOMAXCONN) != 0)
		writeError("Fatal error\n");

	FD_ZERO(&active);
	FD_SET(sockfd, &active);
	max = sockfd;

	while (1){
		ready = active;
		if (select(max + 1, &ready, NULL, NULL, NULL) < 0) 
			continue;
		for (int fd = 0; fd < max + 1; fd++){
			if (!(FD_ISSET(fd, &ready)))
				continue;
			if (fd == sockfd){
				int clientFd = accept(sockfd, NULL, NULL);
				if (clientFd < 0)
					continue;
				max = clientFd > max ? clientFd : max;
				ids[clientFd] = maxId++;
				FD_SET(clientFd, &active);
				sprintf(msg, "server: client %d just arrived\n", ids[clientFd]);
				sendAll(clientFd, msg);
			} else {
				char clientBuf[200000];
				memset(clientBuf, 0, sizeof(clientBuf));
				int recvLen = recv(fd, clientBuf, 200000, 0);

				if (recvLen > 0){
					sprintf(msg, "client %d: ", ids[fd]);
					sendAll(fd, msg);
					for (int i = 0; i < recvLen; i++){
						sending[0] = clientBuf[i];
						sendAll(fd, sending);
						if (i + 1 < recvLen && sending[0] == '\n')
							sendAll(fd, msg);
					}
					if (sending[0] != '\n')
						sendAll(fd, "\n");
				} else {
					sprintf(msg, "server: client %d just left\n", ids[fd]);
					sendAll(fd, msg);
					FD_CLR(fd, &active);
					close(fd);
				}
			}
		}
	}
}
