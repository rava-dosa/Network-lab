#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include "rsocket.h"

int main(){
	int sockfd;
	sockfd=r_socket(AF_INET,SOCK_MRP,0);
	// printf("%d\n",sockfd);
	struct sockaddr_in servaddr,cliaddr,servaddr1;
	memset(&servaddr,0,sizeof(servaddr));
	memset(&cliaddr,0,sizeof(cliaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(50013);
	servaddr.sin_addr.s_addr=INADDR_ANY;
	if(r_bind(sockfd,(const struct sockaddr*)&servaddr,sizeof(servaddr))<0){
		perror("binding failed");
		exit(EXIT_FAILURE);
	}
	char *buffer=(char *)malloc(200*sizeof(char));
	int len=sizeof(cliaddr);
	// int n=recvfrom(sockfd,(char *)buffer,101,0,(struct sockaddr*)&cliaddr,&len);
	servaddr1.sin_family=AF_INET;
	servaddr1.sin_port=htons(50012);
	servaddr1.sin_addr.s_addr=INADDR_ANY;
	int ty=0;	
	while(ty<10){
		printf("Enter one character:-");
		char buf[2];
		scanf("%s",buf);
		r_sendto(sockfd,(char*)buf,strlen(buf)+1,0,(const struct sockaddr*)&servaddr1,sizeof(servaddr1));
		ty++;
	}
	printf("out of loop\n");
	// n=r_recvfrom(sockfd,(char *)buffer,101,0,(struct sockaddr*)&cliaddr,&len);
	// n=recvfrom(sockfd,(char *)buffer,101,0,(struct sockaddr*)&cliaddr,&len);
	// printf("sizeof->%d\n",n);
	// printf("%s\n",buffer);	
	r_close(sockfd);
}