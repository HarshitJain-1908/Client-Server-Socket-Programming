/* Basic template of the socket programming is taken from 
https://www.geeksforgeeks.org/socket-programming-cc/ */

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h> 
#include <arpa/inet.h>
#define PORT 8080

long long int factorial(int n){
    long long int fact = 1;
    for (int i = 1; i <= n; i++) fact = (long)(fact * i);
    return fact;
}

int main(int argc, char const* argv[])
{

	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address,sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	int fd = open("results.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0) { 
    perror("r1"); 
    exit(1); 
	}else{

		//getting IP address and port number of the client
		char* ip_addr = inet_ntoa(address.sin_addr);
		int port = ntohs(address.sin_port);
		while (1) {
		
			char buffer[1024], str[1024], str1[1024], msg[1024];
			bzero(buffer, 1024);
			bzero(str, 1024);
			bzero(str1, 1024);
			bzero(msg, 1024);
			valread = read(new_socket, buffer, 1024);
			int num = atoi(buffer);
			if (num != 0) {
				printf("Message received from client: %d\n", num);
				long long int ans = factorial(num);
				sprintf(str, "%lld", ans);
				strcpy(msg, str);
				send(new_socket, msg, strlen(msg), 0);
				char s[1024] = "Client's IP address: ";
				char res[1024];
				strcpy(res, s);
				strcat(res, ip_addr);
				strcat(res, " ");
				sprintf(str1, "%d", port);
				strcat(res, " Client's Port Number: ");
				strcat(res, str1);
				strcat(res, " Result: ");
				strcat(res, buffer);
				strcat(res, "! = ");
				strcat(res, str);
				strcat(res, "\n");
				write(fd, res, strlen(res));
			}
		}
		
		close(fd);
	}
	
	close(new_socket);
	shutdown(server_fd, SHUT_RDWR);
	return 0;
}
