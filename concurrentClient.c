#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define PORT 8080

pthread_mutex_t lock;

void* thread(){

	pthread_mutex_lock(&lock);
    int sock = 0, valread, client_fd;
	struct sockaddr_in serv_addr;
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
    }

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
	}

	if ((client_fd = connect(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0) {
		printf("\nConnection Failed \n");
	}
    for (int i = 1; i <= 20; i++) {
		char msg[1024];
		sprintf(msg, "%d", i);
	    send(sock, msg, strlen(msg), 0);
		printf("Message sent to server: %s\n", msg);
		char buffer[1024];
		bzero(buffer, 1024);
		valread = read(sock, buffer, 1024);
		char* temp;
		long long int fact = strtoll(buffer, &temp, 10);
		printf("Factorial is: %lld\n", fact);
	}
	pthread_mutex_unlock(&lock);
	close(client_fd);
}

int main()
{	
	clock_t begin = clock();

	int i;
	pthread_t tid;

	for (i = 1; i <= 10; i++)
		pthread_create(&tid, NULL, &thread, NULL);

	clock_t end = clock();
    printf("Time taken: %lf\n", (double)(end - begin));
	pthread_exit(NULL);

	

	return 0;
}
