/* Basic template of the non-blocking socket programming is taken from Linux Man pages of epoll() system call.*/

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10
#define DATA_BUFFER 5000

struct epoll_event ev, events[MAX_EVENTS];
int listen_sock, conn_sock, nfds, epollfd;

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
static void set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    perror("fcntl()");
    return;
  }
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
    perror("fcntl()");
  }
}
int main() {

    struct sockaddr_in new_addr;
    socklen_t addrlen;
    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }
    
    int fd = open("results.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0) { 
        perror("r1"); 
        exit(1); 
	}else{
        
        while (1) {
            
            nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
            printf("boliyega\n");
            if (nfds == -1) {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }

            for (int n = 0; n < nfds; ++n) {
                
                if (events[n].data.fd == listen_sock) {
                    conn_sock = accept(listen_sock,
                                        (struct sockaddr *) &new_addr, &addrlen);
                    if (conn_sock == -1) {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    set_nonblocking(conn_sock);
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = conn_sock;
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
                                &ev) == -1) {
                        perror("epoll_ctl: conn_sock");
                        exit(EXIT_FAILURE);
                    }
                } else {
                    char str[1024], str1[1024], msg[1024], buf[1024];
                    int bufsize = read(events[n].data.fd, buf, DATA_BUFFER);
                    if (bufsize == -1){
                        events[n].data.fd = 0;
                    }else if (bufsize == 0){
                        events[n].data.fd = 0;
                    }else{
                        //getting IP address and port number of the client
                        char* ip_addr = inet_ntoa(new_addr.sin_addr);
                        int port = ntohs(new_addr.sin_port);
                        int num = atoi(buf);
                        if (num != 0) {
                            printf("Message received from client: %d\n", num);
                            long long int ans = factorial(num);
                            sprintf(str, "%lld", ans);
                            strcpy(msg, str);
                            send(events[n].data.fd, msg, strlen(msg), 0);
                            
                            char s[1024] = "Client's IP address: ";
                            char res[1024];
                            strcpy(res, s);
                            printf("ip_addr: %s\n", ip_addr);
                            strcat(res, ip_addr);
                            strcat(res, " ");
                            printf("hi\n");
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
        }
    }
}