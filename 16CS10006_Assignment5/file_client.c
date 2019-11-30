
/*    THE CLIENT PROCESS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h> 
#include <time.h>
void signal_callback_handler(int signum){

        printf("Connection closed-> %d\n",signum);
        exit(0);
}

void progressbar(int byte,long long int size){
   int x=((float)byte/(int)size)*100;
   // printf("%d,%d,%d\n",byte,(int)size,x);
   char dot[101];
   memset(dot,'.',x/3);
   dot[x/3]='\0';
   printf("\rIn progress [%d][%s]", x,dot);
   usleep(10);
   fflush(stdout);
}

char count(char *buf, int *word, int n, char prev){
	// int len=strlen(buf);
	for(int i=0;i<n;i++){
		char c=buf[i];
		if(isalnum(c)&&!isalnum(prev)){
			// printf("inloop\n");
			*word=*word+1;
		}
		prev=c;
	}
	return prev;
}
void slicing(char *buf, char *ret, int i1, int i2){
	for (int i=i1;i<i2;i++){
		ret[i-i1]=buf[i];
	}
	ret[i2-i1]='\0';
}
main()
{
	int			sockfd ;
	struct sockaddr_in	serv_addr;

	int i;
	char buf[100];

	/* Opening a socket is exactly similar to the server process */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}

	serv_addr.sin_family	= AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port	= htons(200002);
	signal(SIGPIPE,signal_callback_handler);
	/* With the information specified in serv_addr, the connect()
	   system call establishes a connection with the server process.
	*/
	if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}
	/* After connection, the client can send or receive messages.
	   However, please note that recv() will block when the
	   server is not sending and vice versa. Similarly send() will
	   block when the server is not receiving and vice versa. For
	   non-blocking modes, refer to the online man pages.
	*/
	printf("Enter file name-> ");
	scanf("%s",buf);
	printf("%s\n",buf);
	printf("hit\n");
	// for(i=0; i < 100; i++) buf[i] = '\0';
	int n1=send(sockfd, buf, strlen(buf)+1, 0);
	// printf("%d->%s\n",n1, buf);
	int byte=0;
	int word=0;
	int x=0;
	int i1=0;
	int fd;
	char prev;
	int size=0;
	int leftsize=0;
	while(1){
		memset(buf,0,sizeof(buf));
		i1=i1+1;
		// sleep(1);
		int n6=0;
		int n5;
		if(i1==1){
			n5=recv(sockfd, buf+n6, 30, MSG_WAITALL);
		}
		if(buf[1]=='0' && i1==1 && buf[0]=='L'){
			printf("File found but empty\n");
			close(sockfd);
			exit(0);
		}
		if(buf[1]!=0 && i1==1 && buf[0]=='L'){
			char *bufnew=malloc(29 * sizeof(char));;
			slicing(buf,bufnew,1,30);
			size=atoi(bufnew);
			printf("%d\n",size);
			leftsize=size;
			prev='-';
		}
		if(buf[0]=='E' && i1==1){
			printf("File not found\n");
			close(sockfd);
			exit(0);
		}
		if(leftsize>100){
			n1=recv(sockfd, buf, 100, MSG_WAITALL);
		}
		else{
			n1=recv(sockfd, buf, leftsize, MSG_WAITALL);	
		}
		// printf("afterifelse->%d\n", n1);
		int n2=(int)n1;
		prev=count(buf,&word,n2,prev);
		if(n1>0 && i1==1){
			fd = open("foo11", O_WRONLY | O_CREAT | O_TRUNC, 0644);
			x=write(fd, buf, n2);
		}
		else if(n1==0 && i1==1){
			printf("File not found\n");
			exit(0);
		}
	    else if(n1>0 && i1>1){
			x=write(fd, buf, n2);
		}
		else {
			break;
		}
		byte=byte+n2;
		leftsize=size-byte;
		// printf("%d,%d",byte,size);
		// progressbar(byte,size);

	}
	close(fd);
	printf("\nno of byte->%d,%d\n",byte,word);
	strcpy(buf,"Message from client");
	n1=send(sockfd, buf, strlen(buf) + 1, 0);
	n1=send(sockfd, buf, strlen(buf) + 1, 0);
	n1=send(sockfd, buf, strlen(buf) + 1, 0);
	// printf("%d->%s\n",n1, buf);
	// strcpy(buf,"Message from client");
	// n1=send(sockfd, buf, strlen(buf) + 1, 0);
	// 	strcpy(buf,"Message from client");
	// n1=send(sockfd, buf, strlen(buf) + 1, 0);	strcpy(buf,"Message from client");
	// n1=send(sockfd, buf, strlen(buf) + 1, 0);
	if (n1==-1){
		perror("connection closed->");
	}
	// printf("%d->%s\n",n1, buf);
	close(sockfd);
}