/*
Name: Xiaoyan Xu
Student  Number: 11229743
NSID: xix799
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h> 
#include <pthread.h> 

#define INPUTSIZE 20
#define  PORT 31234
#define  BACKLOG 10
#define  MAXDATASIZE 100
#define  HOSTNAME "127.0.0.1"
int flag = -1;
void getInput(char *buffer){
    char input[INPUTSIZE];
    printf("Enter the day: ");
    fgets(input, sizeof(input), stdin);
    strcpy(buffer,input);
    buffer[strcspn(buffer,"\n")]='\0';

}

int validateInput(char *buffer){
    int val = -1;
    if ((strcmp(buffer,"Monday") == 0)||
    (strcmp(buffer,"Wednesday") == 0)||
    (strcmp(buffer,"Thursday") == 0)||
    (strcmp(buffer,"Friday") == 0)||
    (strcmp(buffer,"Tuesday") == 0)||
    (strcmp(buffer,"Saturday") == 0)||
    (strcmp(buffer,"Sunday") == 0))
    {
        val = 1;
    }
    else if (strcmp(buffer,"quit") == 0){
        flag = 1;
        val = 1;
    }

    return val;
}

void *nodeSend(void* port_des){
    int  socketfd, num, len, port_;
    char  buf[MAXDATASIZE];
    char buffer[INPUTSIZE];
    struct hostent *host;
    struct sockaddr_in server;
    char msg[INPUTSIZE];

    while(1){

        memset(buffer,0,INPUTSIZE);
        memset(buf,0,MAXDATASIZE);
        getInput(buffer);
        if(validateInput(buffer)==-1)
        {
            printf("invalid day\n");
            break;
        }

        else{    
        strcpy(msg,buffer);

        host=gethostbyname(HOSTNAME);

        port_=*(int*)port_des;

        memset(&server, 0, sizeof(server));
        server.sin_family= AF_INET;
        server.sin_port = htons(port_);
        server.sin_addr =*((struct in_addr *)host->h_addr);
        if((socketfd=socket(AF_INET, SOCK_STREAM, 0))==-1){
            printf("socket()error\n");
            exit(1);
        }
        if(connect(socketfd,(struct sockaddr *)&server,sizeof(server))==-1){
        printf("connect()error\n");
        exit(1);
        }
        
        len = strlen(msg);
        
        send(socketfd, msg, len, 0);
        
        if(flag==1){        
            break;
        }
        if((num=recv(socketfd,buf,MAXDATASIZE,0)) == -1){
            printf("recv() error\n");
            exit(1);
        }
        buf[num]='\0';
        printf("%s",buf);
        close(socketfd);                  
        }
    
    }
    return 0; 
}

void *nodeRecive(void* arg){
 int  socketfd, connectfd, buf_len;
    struct  sockaddr_in server;
    struct  sockaddr_in client;
    socklen_t  addrlen;
    char  buf[MAXDATASIZE];


    int opt =1;
    addrlen =sizeof(client);

    memset(&server, 0, sizeof(server));
    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr.s_addr= htonl (INADDR_ANY);

    if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Creating  socket failed.");
        exit(1);
    }
  
    if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1){
        perror("setsockopt failed");
        exit(1);
    }
    if(bind(socketfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Bind failed.");
        exit(1);
    }   
    if(listen(socketfd,BACKLOG)== -1){ 
        perror("listen failed\n");
        exit(1);
    }

    while(1){

        if((connectfd = accept(socketfd,(struct sockaddr*)&client,&addrlen))==-1) {
            perror("accept()error\n");
            exit(1);
        }
        
       

        if((buf_len = recv(connectfd,buf,MAXDATASIZE,0)) == -1){
        printf("recv() error\n");
        exit(1);
        }
        if(strcmp(buf,"quit")==0){break;}
        printf("buffer from proxy is: %s\n",buf);
       
        close(connectfd);
    }
    return 0; 
}
 
int main(int argc, char *argv[]){
    pthread_t tid1; 
    pthread_t tid2; 
    int p;


    
    if(argc!=2){
        printf("Usage: ./clienttcp port\n");
        exit(1);
    }
    p = atoi(argv[1]);
    printf("hello\n");
    pthread_create(&tid1, NULL, nodeSend, (void *)&p);
    
    pthread_create(&tid2, NULL, nodeRecive, (void *)&p);
    pthread_join(tid1, NULL); 
    pthread_join(tid2, NULL); 
    return 0;
}

