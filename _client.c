#include<stdio.h>
#include<sys/types.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <linux/inotify.h>
#include <errno.h>

//directory
#include <dirent.h>
#include <limits.h>

#define PORT "8080"
#define HOSTNAME "127.0.0.1"
#define SA struct sockaddr

//client action
#define CREATEFILE "c"
#define MODIFYFILE "m"
#define DELETEFILE "d"
#define BUFSIZE 10

//inotify
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

char buf[PATH_MAX];

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void synchronizeAllData() {
  char *res = realpath("client", buf);
}

int main() {
    int sockfd;
    int len, n;
              

    char action[5];
    char filename[50];
    //get directory
    synchronizeAllData();
    printf("watching: %s", buf);
    
    while(1) {
      int length, i = 0;
      int fd;
      int wd;
      char bufferNotify[EVENT_BUF_LEN];
      //file
      char buffer[1000];
      char send_buffer[BUFSIZE];
      FILE *f;

      /*creating the INOTIFY instance*/
      fd = inotify_init();

      /*checking for error*/
      if ( fd < 0 ) {
        perror( "inotify_init" );
      }

      /*adding the “/tmp” directory into watch list. Here, the suggestion is to validate the existence of the directory before adding into monitoring list.*/
      wd = inotify_add_watch( fd, buf, IN_CREATE | IN_DELETE | IN_MODIFY);

      /*read to determine the event change happens on “/tmp” directory. Actually this read blocks until the change event occurs*/ 
      length = read( fd, bufferNotify, EVENT_BUF_LEN ); 

      /*checking for error*/
      if ( length < 0 ) {
        perror( "read" );
      }  

      /*actually read return the list of change events happens. Here, read the change event one by one and process it accordingly.*/
      while ( i < length ) {     
        struct inotify_event *event = ( struct inotify_event * ) &bufferNotify[ i ];     
        if ( event->len ) {
          if ( event->mask & IN_CREATE ) {
            if ( event->mask & IN_ISDIR ) {
              printf( "New directory %s created.\n", event->name );
            }
            else {
              printf( "New file %s created.\n", event->name );
              
              struct sockaddr_in servaddr,cliaddr;
              sockfd = socket(AF_INET,SOCK_STREAM,0);
              bzero(&servaddr,sizeof(servaddr));
              
              servaddr.sin_family=AF_INET;
              servaddr.sin_addr.s_addr=inet_addr(HOSTNAME);
              servaddr.sin_port=htons(atoi(PORT));

              inet_pton(AF_INET,PORT,&servaddr.sin_addr);
              connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
              
              printf("connection ready \n");
              sleep(1);
              //action
              strcpy(action, CREATEFILE);
              send(sockfd,action,3,0);
              bzero(buffer,1000);
              printf("action send %s\n", action);
              sleep(1);

              //filename
              strcpy(filename, event->name);
              send(sockfd,filename,strlen(filename),0);
              bzero(buffer,1000);
              printf("filename send %s\n", filename);
              //file content
              sleep(1);
              f = fopen(event->name,"r+");

              if (f != NULL) {
                printf("%s", send_buffer);
                printf("Sending file as Byte Array\n");
                
                n = fread(send_buffer, 1, sizeof(send_buffer), f);

                while(!feof(f)) {
                  printf("hay algo en el archivo D:");
                  write(sockfd, send_buffer, n);
                  n = fread(send_buffer, 1, sizeof(send_buffer), f);
                  printf("%d", n);
                  if (n < 0) error("Unable send content \n");
                }
                write(sockfd, send_buffer, n);
                if (n < 0) error("Unable send content \n");
                printf("file content send \n");

                printf("the file was sent successfully\n");

                fseek(f, 0, SEEK_END);

                int lenFile = ftell(f);
                fclose(f);

                printf("Total size of %s = %d bytes\n", filename, lenFile);

                sleep(1);
              } else {
                printf("sorry but file null");
              }
            }
          } else if (event->mask & IN_MODIFY ) {
            if ( event->mask & IN_ISDIR ) {
              printf( "Directory %s modified.\n", event->name );
            }
            else {
              printf( "File %s modified.\n", event->name );
              
              struct sockaddr_in servaddr,cliaddr;
              sockfd = socket(AF_INET,SOCK_STREAM,0);
              bzero(&servaddr,sizeof(servaddr));
              
              servaddr.sin_family=AF_INET;
              servaddr.sin_addr.s_addr=inet_addr(HOSTNAME);
              servaddr.sin_port=htons(atoi(PORT));

              inet_pton(AF_INET,PORT,&servaddr.sin_addr);
              connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
              
              printf("connection ready \n");

              sleep(1);
              //action
              strcpy(action, MODIFYFILE);
              send(sockfd,action,3,0);
              bzero(buffer,1000);
              printf("action send %s\n", action);
              sleep(1);

              //filename
              strcpy(filename, event->name);
              send(sockfd,filename,strlen(filename),0);
              bzero(buffer,1000);
              printf("filename send %s\n", filename);
              //file content
              sleep(1);
              
              f = fopen(filename,"r+");
              
              if (f != NULL) {
                printf("%s", send_buffer);
                printf("Sending file as Byte Array\n");
              } else {
                printf("Sorry but die");
              }
              printf("Sending file as Byte Array\n");
              // no link between BUFSIZE and the file size
              n = fread(send_buffer, 1, sizeof(send_buffer), f);

              while(!feof(f)) {
                  write(sockfd, send_buffer, n);
                  n = fread(send_buffer, 1, sizeof(send_buffer), f);
                  printf("%d", n);
                  if (n < 0) error("Unable send content \n");
              }
              write(sockfd, send_buffer, n);
              if (n < 0) error("Unable send content \n");
              printf("file content send \n");

              printf("the file was sent successfully\n");

              fseek(f, 0, SEEK_END);

              int lenFile = ftell(f);
              fclose(f);

              printf("Total size of %s = %d bytes\n", filename, lenFile);
              sleep(1);
            }
          } else if ( event->mask & IN_DELETE ) {
            if ( event->mask & IN_ISDIR ) {
              printf( "Directory %s deleted.\n", event->name );
            }
            else {
              printf( "File %s deleted.\n", event->name );
              
              struct sockaddr_in servaddr,cliaddr;
              sockfd = socket(AF_INET,SOCK_STREAM,0);
              bzero(&servaddr,sizeof(servaddr));
              
              servaddr.sin_family=AF_INET;
              servaddr.sin_addr.s_addr=inet_addr(HOSTNAME);
              servaddr.sin_port=htons(atoi(PORT));

              inet_pton(AF_INET,PORT,&servaddr.sin_addr);
              connect(sockfd,(SA*)&servaddr,sizeof(servaddr));
              
              printf("connection ready \n");

              sleep(1);
              //action
              strcpy(action, DELETEFILE);
              send(sockfd,action,3,0);
              bzero(buffer,1000);
              printf("action send %s\n", action);
              sleep(1);

              //filename
              strcpy(filename, event->name);
              send(sockfd,filename,strlen(filename),0);
              bzero(buffer,1000);
              printf("filename send \n");
              //no file content sent
            }
          }          
        }
        i += EVENT_SIZE + event->len;
      }
      /*removing the “/tmp” directory from the watch list.*/
      inotify_rm_watch( fd, wd );

      /*closing the INOTIFY instance*/
      close( fd );
      close(sockfd);
    }
}