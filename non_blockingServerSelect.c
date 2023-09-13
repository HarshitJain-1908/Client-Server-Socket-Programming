/* Basic template of the non-blocking socket programming is taken from the Tutorial video posted on GC.*/

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define DATA_BUFFER 5000
#define MAX_CONNECTIONS 10   

int create_tcp_server_socket(){
    struct sockaddr_in saddr;
    int fd, ret_val;

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1){
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }
    
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    saddr.sin_addr.s_addr = INADDR_ANY;

    ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val != 0){
        fprintf(stderr, "bind failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }
    ret_val = listen(fd, 5);
    if (ret_val != 0){
        fprintf(stderr, "listen failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }
    return fd;
}

long long int factorial(int n){
    long long int fact = 1;
    for (int i = 1; i <= n; i++) fact = (long)(fact * i);
    return fact;
}

int main()
{
	fd_set read_fd_set;
    struct sockaddr_in new_addr;
    int server_fd, new_fd, ret_val;
    socklen_t addrlen;
    char buf[DATA_BUFFER];
    int all_connections[MAX_CONNECTIONS];

    server_fd = create_tcp_server_socket();
    if (server_fd == -1){
        fprintf(stderr, "Failed to create a server.\n");
        return -1;
    }

    for (int i = 0; i < MAX_CONNECTIONS; i++){
        all_connections[i] = -1;
    }
    all_connections[0] = server_fd;
    int fd = open("results.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0) { 
        perror("r1"); 
        exit(1); 
	}else{

        while (1){
            
            FD_ZERO(&read_fd_set);
            FD_SET(server_fd, &read_fd_set);
            for (int i = 0; i < MAX_CONNECTIONS; i++){
                if (all_connections[i] >= 0){
                    FD_SET(all_connections[i], &read_fd_set);
                }
            }
            ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);
            if (ret_val > 0){
                if (FD_ISSET(server_fd, &read_fd_set)){
                    new_fd = accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);
                    if (new_fd >= 0){
                        for (int i = 0; i < MAX_CONNECTIONS; i++){
                            if (all_connections[i] < 0){
                                all_connections[i] = new_fd;
                                break;
                            }
                        }
                    }else{
                        fprintf(stderr, "accept failed [%s]\n", strerror(errno));
                    }
                    ret_val--;
                    if (!ret_val) continue;
                }
                for (int i = 1; i < MAX_CONNECTIONS; i++){
                    if ((all_connections[i] > 0) && (FD_ISSET(all_connections[i], &read_fd_set))){
                            char str[1024], str1[1024], msg[1024];
                            bzero(str, 1024);
                            bzero(str1, 1024);
                            bzero(msg, 1024);
                            bzero(buf, DATA_BUFFER);
                            ret_val = recv(all_connections[i], buf, DATA_BUFFER, 0);
                            if (ret_val == 0){
                                printf("Closing connection for fd:%d\n", all_connections[i]);
                                close(all_connections[i]);
                                all_connections[i] = -1;
                            }
                            if (ret_val == -1){
                                printf("recv() failed for fd:%d [%s]\n", all_connections[i], strerror(errno));
                                break;
                            }
                            if (ret_val > 0) {
                                
                                //getting IP address and port number of the client
                                char* ip_addr = inet_ntoa(new_addr.sin_addr);
                                int port = ntohs(new_addr.sin_port);
                                int num = atoi(buf);
                                printf("Message received from client%d: %d\n", all_connections[i], num);
                                long long int ans = factorial(num);
                                sprintf(str, "%lld", ans);
                                strcpy(msg, str);
                                send(all_connections[i], msg, strlen(msg), 0);
                                char s[1024] = "Client's IP address: ";
                                char res[1024];
                                strcpy(res, s);
                                strcat(res, ip_addr);
                                strcat(res, " ");
                                sprintf(str1, "%d", port);
                                strcat(res, " Client's Port Number: ");
                                strcat(res, str1);
                                strcat(res, " Result: ");
                                strcat(res, buf);
                                strcat(res, "! = ");
                                strcat(res, str);
                                strcat(res, "\n");
                                write(fd, res, strlen(res));
		                    }
                    }
                }
                        
            }
            ret_val--;
            if (!ret_val) continue;
        }
        close(fd);
    }

    for (int i = 0; i < MAX_CONNECTIONS; i++){
        if (all_connections[i] > 0)
            close(all_connections[i]);
    }
    return 0;
}
