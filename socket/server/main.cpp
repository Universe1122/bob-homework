#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>

using namespace std;

bool echo = false;

void error_report(char *message){
    printf("%s", message);
}

void *recvData(void *client_sock){
    printf("[*] Connected\n");
    char buffer[100000];
    int client = *(int *)client_sock;

    while(true){
        memset(&buffer, 0, sizeof(buffer));

        int recv_data_len = read(client, buffer, sizeof(buffer)-1);
        
        if(recv_data_len == 0 || recv_data_len == -1){
            error_report("[!] receive error\n");
            break;
        }
        if(strlen(buffer) != 0){
            buffer[recv_data_len] = '\0';
            printf("client: %s\n", buffer);
            fflush(stdout);
        }
        if(echo){
            write(client, buffer, sizeof(buffer));
        }
    }

    printf("[*] Disconnected\n");
    close(client);
}

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("[!] Usage: %s <port> -e -b\n", argv[0]);
        exit(1);
    }

    if(argc == 3){
        if(strcmp(argv[2], "-e") == 0)
            echo = true;
    }

    int server_sock = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    if(server_sock == -1){
        error_report("[!] socket error\n");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        error_report("[!] bind error\n");
        exit(1);
    }
    
    if (listen(server_sock, 5) == -1){
        error_report("[!] listen error\n");
        exit(1);
    }
    
    while(true){
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
        if(client_sock == -1){
            error_report("[!] accpet() error");
            break;
        }
        
        pthread_t t;
        pthread_create(&t, NULL, recvData, (void *)&client_sock);
        pthread_detach(t);
    }
    close(server_sock);
    return 0;
    // https://jaimemin.tistory.com/7
}