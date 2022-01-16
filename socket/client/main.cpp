#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>

void error_report(char *message){
    printf("%s", message);
    exit(1);
}

void *recvData(void *sock){
    char buffer[100000];
    int server = *(int *)sock;

    while(true){
        memset(&buffer, 0, sizeof(buffer));
        int recv_data_len = read(server, buffer, sizeof(buffer)-1);

        if(recv_data_len == 0 || recv_data_len == -1){
            error_report("[!] receive error\n");
            break;
        }
        if(strlen(buffer) != 0){
            buffer[recv_data_len] = '\0';
            printf("server: %s\n", buffer);
            fflush(stdout);
        }
    }

    close(server);
}

int main(int argc, char *argv[]){
    if (argc != 3){
        printf("[!] Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    char message[100000];
    int str_len;

    if(sock == -1)
        error_report("sock() error");
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        error_report("connect() error");
    
    pthread_t t;
    pthread_create(&t, NULL, recvData, (void *)&sock);
    pthread_detach(t);

    while(true){
        char message[100000];
        scanf("%s", &message);

        write(sock, message, sizeof(message));
    }
    return 0;
}