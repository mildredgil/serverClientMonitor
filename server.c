#include<stdio.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <linux/inotify.h>

//Server 
#define PORT "8080"
#define SA struct sockaddr
#define LISTENQ 10

//client action
#define CREATEFILE "c"
#define MODIFYFILE "m"
#define DELETEFILE "d"
#define BUFSIZE 10
//notify 
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int fd,sockfd,listenfd,newsockfd, n;
    pid_t childpid;
    socklen_t client;
    struct sockaddr_in servaddr,cliaddr;
    listenfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof(servaddr));
    
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(atoi(PORT));
    bind(listenfd,(SA*)&servaddr,sizeof(servaddr));
    listen(listenfd,LISTENQ);
    
    do { 
        client=sizeof(cliaddr);
        newsockfd=accept(listenfd,(SA*)&cliaddr,&client);
        char buffer[1000];
        FILE *fp;
        
        if (newsockfd < 0) error("ERROR on accept");
        bzero(buffer,1000);
        //action
        n = recv(newsockfd , buffer , 1000 , 0);
        
        if (n < 0) error("ERROR reading from socket <action>");
        char action[4];
        strcpy(action, buffer);
        printf("the action is %s \n" , action);
        bzero(buffer,1000);
        //filename
        //n = read(newsockfd,buffer,1000);
        n = recv(newsockfd , buffer , 1000 , 0);
        if (n < 0) error("ERROR reading from socket <filename>");
        char filename[50];
        strcpy(filename, buffer);
        printf("the filename is %s \n" , filename);
        bzero(buffer,1000);

        if( strcmp(action, CREATEFILE) == 0 ) {
            //Creates an empty file for writing. 
            //If a file with the same name already exists, 
            //its content is erased and the file is considered as a new empty file.
            fp=fopen(filename,"w+");

            //file content
            //Read File Byte Array and Copy in file
            printf("Reading File Byte Array\n");
            char p_array[BUFSIZE];

            n = recv(newsockfd, p_array, BUFSIZE, 0);
            printf("%d /n", n);
            int first = 0;
            while (n >= 0 && first == 0) {
                fwrite(p_array, 1, n, fp);
                
                if( n == 0) {
                    first = -1;
                }

                n = read(newsockfd, p_array, BUFSIZE);
                //printf("%s", p_array);
                //printf("%d /n", n);
            }
            
            if (n < 0) error("ERROR reading from socket <file content>");
            fprintf(fp,"%s",buffer);
            bzero(buffer,1000);

            printf("the file was received successfully\n");
            printf("the new file created is %s\n" , filename);
            fseek(fp, 0, SEEK_END);
            int lenFile = ftell(fp);
            fclose(fp);

            printf("Total size of %s = %d bytes\n", filename, lenFile);
        } else if ( strcmp(action, MODIFYFILE) == 0 ) {
            //Creates an empty file for writing. 
            //If a file with the same name already exists, 
            //its content is erased and the file is considered as a new empty file.
            fp=fopen(filename,"w+");

            if(fp != NULL) {
                //file content
                //Read File Byte Array and Copy in file
                printf("Reading File Byte Array\n");
                char p_array[BUFSIZE];

                n = recv(newsockfd, p_array, BUFSIZE, 0);
                printf("%d /n", n);
                int first = 0;
                while (n >= 0 && first == 0) {
                    fwrite(p_array, 1, n, fp);
                    
                    if( n == 0) {
                        first = -1;
                    }

                    n = read(newsockfd, p_array, BUFSIZE);
                    printf("%d /n", n);
                }
                
                if (n < 0) error("ERROR reading from socket <file content>");
                fprintf(fp,"%s",buffer);
                bzero(buffer,1000);

                printf("the file was received successfully\n");
                printf("the new file created is %s\n" , filename);
                fseek(fp, 0, SEEK_END);
                int lenFile = ftell(fp);
                fclose(fp);

                printf("Total size of %s = %d bytes\n", filename, lenFile);
            }
        } else if ( strcmp(action, DELETEFILE) == 0 ) {
            recv(newsockfd , buffer , 1000 , 0);

            n = remove(filename);
            if (n < 0) printf("Unable to delete the file");
        }
        close(newsockfd);
        close(sockfd);
    } while(1);
}