#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SIZEBUF (1024)

int main(int argc, char** argv) {
    int sockfd;
    char buffer[SIZEBUF];
    char res[SIZEBUF];
    struct sockaddr_in server = {
        PF_INET,
        3216
    };

    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);//inet_addr("197.45.10.2");

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Error with socket()\n%s\n", strerror(errno));
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) == -1) {
        printf("Error with connect()\n%s\n", strerror(errno));
        exit(1);
    }

    while(strcmp(buffer, "close") != 0) {  
        printf("\nInput string:\n");
        scanf("%s", &buffer);
        send(sockfd, buffer, SIZEBUF, 0);
        if (recv(sockfd, res, SIZEBUF, 0) > 0)
            printf("%s", res);
        else
        {
            printf("The server is not responding\n");
            close(sockfd);
            exit(1);
        }
    }

    exit(0);
}
