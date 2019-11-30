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
char buf[MAXBUF];

int main(){
	struct sockaddr_in servaddr;

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0){
		perror("socket connection failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr,0,sizeof(servaddr));

	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=INADDR_ANY;
	servaddr.sin_port=htons(8181);
	while(1){
		scanf("%s",buf);
		printf("Sent->%s\n",buf);
		int len=sizeof(servaddr);
		int ret=sendto(sockfd, (char *)buf,100,0,(const struct sockaddr *)&servaddr,len);
		// printf("%s\n","sent");
		if(ret<0){
			perror("asdfg->");
			exit(0);
		}
		ret=recvfrom(sockfd, (char *)buf,100,0,(struct sockaddr *)&servaddr,&len);	
		if(ret<0){
			perror("asdfg->");
			exit(0);
		}			
		printf("Received->%s\n",buf);
	}
}