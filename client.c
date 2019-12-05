#include<stdio.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

#define PORT "8080"
#define HOSTNAME "127.0.0.1"
#define SA struct sockaddr

//client action
#define CREATEFILE 'c'
#define MODIFYFILE 'm'
#define DELETEFILE 'd'
#define BUFSIZE 256

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd;
    int len, n;
    
    struct sockaddr_in servaddr,cliaddr;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));
    
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=inet_addr(HOSTNAME);
    servaddr.sin_port=htons(atoi(PORT));
    
    inet_pton(AF_INET,PORT,&servaddr.sin_addr);
    connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
    
    printf("connection ready \n");

    char action[5];
    char filename[50];
    char buffer[1000];
    
    FILE *f;
    sleep(1);
    //action
    strcpy(action, "c");
    send(sockfd,action,3,0);
    bzero(buffer,1000);
    printf("action send %s\n", action);
    sleep(1);

    //filename
    strcpy(filename, "lunita.jpeg");
    send(sockfd,filename,strlen(filename),0);
    bzero(buffer,1000);
    printf("filename send \n");
    //file content
    sleep(1);
    
    f = fopen("luna.jpeg","r+");
    
    printf("Sending file as Byte Array\n");
    char send_buffer[BUFSIZE]; // no link between BUFSIZE and the file size
    n = fread(send_buffer, 1, sizeof(send_buffer), f);

    while(!feof(f)) {
        write(sockfd, send_buffer, n);
        n = fread(send_buffer, 1, sizeof(send_buffer), f);
        printf("%d", n);
        if (n < 0) error("Unable send content \n");
        //printf("%s", send_buffer);
        //printf("%d", n);
    }
    write(sockfd, send_buffer, n);
    if (n < 0) error("Unable send content \n");
    printf("file content send \n");

    printf("the file was sent successfully\n");

    fseek(f, 0, SEEK_END);

    int lenFile = ftell(f);
    fclose(f);

    printf("Total size of file.txt = %d bytes\n", lenFile);
}