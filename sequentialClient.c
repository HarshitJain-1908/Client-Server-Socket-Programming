/* Basic template of the socket programming is taken from 
https://www.geeksforgeeks.org/socket-programming-cc/ */

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080


int main(int argc, char const* argv[])
{	
	
	int sock = 0, valread, client_fd;
	struct sockaddr_in serv_addr;
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((client_fd = connect(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}
    for (int i = 1; i <= 20; i++) {
        
		char* str;
		sprintf(str, "%d", i);
		char* msg = str;
		char buffer[1024];
	    send(sock, msg, strlen(msg), 0);
	    printf("Message sent to server: %s\n", msg);
		bzero(buffer, 1024);
		valread = read(sock, buffer, 1024);
		char* temp;
		long long int fact = strtoll(buffer, &temp, 10);
		printf("Factorial is: %lld\n", fact);
	}

	close(client_fd);
	return 0;
}
