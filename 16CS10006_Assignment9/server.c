//I think non blocking call using non blocking flag is better as it provides greater freedom to manipulate and write code.. But it leads to busy waiting 
//hence if my use case is to write a code which takes less cpu time I wil use signals 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h> 

#include <fcntl.h>
#include <signal.h>

#define MAXBUF 100

int sockfd;
int sockfd2;
char buf[MAXBUF];

void io_handler(int signal1)
{
	printf("%s\n","AAya mein");
    int addr_len;	
    struct sockaddr_in their_addr, cliaddr;
    printf("Sockfd->%d\n",sockfd);
    sockfd2=sockfd;
    int numbytes=recvfrom(sockfd2, buf, MAXBUF, 0,(struct sockaddr *)&their_addr, &addr_len);
    if (numbytes== -1) {
            perror("recvfrom");
            exit(1);
    }
    sockfd2=sockfd;
	int sendbytes=sendto(sockfd2, buf, MAXBUF, 0,(struct sockaddr *)&their_addr, addr_len);
	buf[numbytes]='\0';
	printf("%s\n",buf);
	signal(SIGIO,io_handler);
	return;
}
int main(){
	// int sockfd;
	struct sockaddr_in servaddr,cliaddr;

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0){
		perror("socket conection failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr,0,sizeof(servaddr));
	memset(&cliaddr,0,sizeof(cliaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(8181);
	servaddr.sin_addr.s_addr=INADDR_ANY;

	if(bind(sockfd,(const struct sockaddr*)&servaddr,sizeof(servaddr))<0){
		perror("binding failed");
		exit(EXIT_FAILURE);
	}

	signal(SIGIO,io_handler);
	printf("\nServer Running....\n");

	if (fcntl(sockfd,F_SETOWN, getpid()) < 0){
		perror("fcntl F_SETOWN");
		exit(1);
	}

	if (fcntl(sockfd,F_SETFL,FASYNC) <0 ){
		perror("fcntl F_SETFL, FASYNC");
		exit(1);
	}
	while(1){
		;
	}
}