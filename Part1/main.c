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
#include <signal.h>

#define SIZEBUF (1024)
#define CHMOD (0644)

int newsockfd;

void catcher(int sig) {
    close(newsockfd);
    exit(0);
}

int main(int argc, char** argv) {
    pid_t pid;
    int logFile;
    char* str;
    size_t memNeeded;
    char* path = malloc(SIZEBUF * sizeof * path);
    int sockfd;
    char c[SIZEBUF];

    struct sockaddr_in server = {
        PF_INET,
        3216,
        INADDR_ANY
    };

    if ((logFile = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, CHMOD)) == -1) {
        printf("Error with open(log.txt, ...)\n%s\n", strerror(errno));
        exit(1);
    }

    if ((write(logFile, "Parent process started\n", sizeof("Parent process started\n") - 1)) == -1) {
        printf("Error with write()\n%s\n", strerror(errno));
        exit(1);
    }

    memNeeded = snprintf(NULL, 0, "Parent process:\npid = %d;\ngid = %d;\nsid = %d.\n\n", getpid(), getgid(), getsid(getpid())) + sizeof('\0');
    str = malloc(memNeeded);
    snprintf(str, memNeeded, "Parent process:\npid = %d;\ngid = %d;\nsid = %d.\n\n", getpid(), getgid(), getsid(getpid()));

    if ((write(logFile, str, strlen(str))) == -1) {
        printf("Error with write()\n%s\n", strerror(errno));
        exit(1);
    }

    if ((pid = fork()) < 0) {
        printf("Error with fork()\n%s\n", strerror(errno));
        exit(1);
    }

    else if (pid == 0) {
        memNeeded = snprintf(NULL, 0, "Demon process:\npid = %d;\ngid = %d;\nsid = %d.\n\n", getpid(), getgid(), getsid(getpid())) + sizeof('\0');
        str = malloc(memNeeded);
        snprintf(str, memNeeded, "Demon process:\npid = %d;\ngid = %d;\nsid = %d.\n\n", getpid(), getgid(), getsid(getpid()));

        if ((write(logFile, str, strlen(str))) == -1) {
            printf("Error with write()\n%s\n", strerror(errno));
            exit(1);
        }

        static struct sigaction act;
        act.sa_handler = catcher;
        sigfillset(&(act.sa_mask));
        sigaction(SIGPIPE, &act, NULL);

        if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
            printf("Error with socket()\n%s\n", strerror(errno));
            exit(1);
        }

        if ((write(logFile, "Socket created\n", sizeof("Socket created\n") - 1)) == -1) {
            printf("Error with write()\n%s\n", strerror(errno));
            exit(1);
        }

        if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
            printf("Error with bind()\n%s\n", strerror(errno));
            exit(1);
        }

        if ((write(logFile, "Address was associated with a socket\n", sizeof("Address was associated with a socket\n") - 1)) == -1) {
            printf("Error with write()\n%s\n", strerror(errno));
            exit(1);
        }

        if (listen(sockfd, 5) == -1) {
            printf("Error with listen()\n%s\n", strerror(errno));
            exit(1);
        }

        if ((write(logFile, "Accepting connections has been enabled\n", sizeof("Accepting connections has been enabled\n") - 1)) == -1) {
            printf("Error with write()\n%s\n", strerror(errno));
            exit(1);
        }

        while (1) {
            if ((newsockfd = accept(sockfd, NULL, NULL)) == -1) {
                printf("Error with accept()\n%s\n", strerror(errno));
                continue;
            }

            if (fork() == 0) {
                memNeeded = snprintf(NULL, 0, "Child process:\npid = %d;\ngid = %d;\nsid = %d.\n\n", getpid(), getgid(), getsid(getpid())) + sizeof('\0');
                str = malloc(memNeeded);
                snprintf(str, memNeeded, "Child process:\npid = %d;\ngid = %d;\nsid = %d.\n\n", getpid(), getgid(), getsid(getpid()));

                if ((write(logFile, str, strlen(str))) == -1) {
                    printf("Error with write()\n%s\n", strerror(errno));
                    exit(1);
                }

                if ((write(logFile, "A child process has been created to handle the connection\n", sizeof("A child process has been created to handle the connection\n") - 1)) == -1) {
                    printf("Error with write()\n%s\n", strerror(errno));
                    exit(1);
                }

                while (recv(newsockfd, c, SIZEBUF, 0) > 0) {
                    if (strcmp(c, "close") == 0) {
                        send(newsockfd, c, SIZEBUF, 0);
                        break;
                    }

                    time_t now;
                    time(&now);
                    struct tm* local = localtime(&now);
                    char buffer[SIZEBUF];
                    strcpy(buffer, c);
                    int hours = local->tm_hour;         
                    int minutes = local->tm_min;        
                    int seconds = local->tm_sec;        

                    snprintf(c, SIZEBUF, "%02d:%02d:%02d_%d_%s", hours, minutes, seconds, getpid(), buffer);

                    memNeeded = snprintf(NULL, 0, "recv = %s\n\n", c) + sizeof('\0');
                    str = malloc(memNeeded);
                    snprintf(str, memNeeded, "recv = %s\n\n", c);

                    if ((write(logFile, str, strlen(str))) == -1) {
                        printf("Error with write()\n%s\n", strerror(errno));
                        exit(1);
                    }

                    send(newsockfd, c, SIZEBUF, 0);
                }

                if ((write(logFile, "Child process was closed\n", sizeof("Child process was closed\n") - 1)) == -1) {
                    printf("Error with write()\n%s\n", strerror(errno));
                    exit(1);
                }

                close(newsockfd);
                exit(0);
            }

            close(newsockfd);
        }

    } else {
        if ((write(logFile, "Parent process created child process\n", sizeof("Parent process created child process\n") - 1)) == -1) {
            printf("Error with write()\n%s\n", strerror(errno));
            exit(1);
        }
    }

    exit(0);
}
