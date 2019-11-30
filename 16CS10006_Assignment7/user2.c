#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include "rsocket.h"

int main(){
	int sockfd;
	// struct sockaddr_in cliaddr;
	sockfd=r_socket(AF_INET,SOCK_MRP,0);
	// printf("%d\n",sockfd);
	struct sockaddr_in servaddr,cliaddr;
	memset(&servaddr,0,sizeof(servaddr));
	memset(&cliaddr,0,sizeof(cliaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(50012);
	servaddr.sin_addr.s_addr=INADDR_ANY;
	if(r_bind(sockfd,(const struct sockaddr*)&servaddr,sizeof(servaddr))<0){
		perror("binding failed");
		exit(EXIT_FAILURE);
	}
	char *buffer=(char *)malloc(200*sizeof(char));
	int len=sizeof(cliaddr);
	// int n=recvfrom(sockfd,(char *)buffer,101,0,(struct sockaddr*)&cliaddr,&len);
	int ty=0;
	while(1){
		char *buffer=(char *)malloc(200*sizeof(char));
		int n=r_recvfrom(sockfd,(char *)buffer,101,0,(struct sockaddr*)&cliaddr,&len);
		printf("Received ->%s\n",buffer);
		// ty++;
	}
	// printf("sizeof->%d\n",n);
	// n=r_recvfrom(sockfd,(char *)buffer,101,0,(struct sockaddr*)&cliaddr,&len);
	// n=recvfrom(sockfd,(char *)buffer,101,0,(struct sockaddr*)&cliaddr,&len);
	// printf("sizeof->%d\n",n);
	// printf("%s\n",buffer);	
	r_close(sockfd);
}