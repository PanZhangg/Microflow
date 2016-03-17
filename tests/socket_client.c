#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLINE 4096

int main(int argc, char** argv)
{
    int    sockfd, n;
    char    recvline[4096], sendline[4096];
    struct sockaddr_in    servaddr;
    int socket_fd_array[1000];
    if( argc != 2){
    printf("usage: ./client <ipaddress>\n");
    exit(0);
    }


    int i;
    for(i = 0; i < 10; i++)
    {
        if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
            exit(0);
        }
        socket_fd_array[i] = sockfd;

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(6633);
        if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
            printf("inet_pton error for %s\n",argv[1]);
            exit(0);
        }
        if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
            printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
            exit(0);
        }
        char c[256] = "hello from a socket";
        send(sockfd, c, sizeof(c), MSG_CONFIRM);
        usleep(100);
    }

    //sleep(2);
    for(i = 0; i < 10; i++)
    {
        close(socket_fd_array[i]);
        usleep(100);
    }

    return 0;
}