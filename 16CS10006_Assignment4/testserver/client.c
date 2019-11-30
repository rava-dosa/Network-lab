
/*    THE CLIENT PROCESS */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include<unistd.h> 
#include <sys/stat.h>
#include <signal.h>

#define max_string 1024

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
int recvstatuscode(int client_socket){
	int received_int = 0;
	int return_status = read(client_socket, &received_int, sizeof(received_int));
	received_int=ntohl(received_int);
	if (return_status > 0) {
	   fprintf(stdout, "Received int = %d\n", received_int);
	}
	else {
	   // Handling erros here
	}
	return received_int;
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

void reverse(char s[])
{
 int i, j;
 char c;

 for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
     c = s[i];
     s[i] = s[j];
     s[j] = c;
 }
}

void itoa(int n, char s[])
{
 int i, sign;

 if ((sign = n) < 0)  /* record sign */
     n = -n;          /* make n positive */
 i = 0;
 do {       /* generate digits in reverse order */
     s[i++] = n % 10 + '0';   /* get next digit */
 } while ((n /= 10) > 0);     /* delete it */
 if (sign < 0)
     s[i++] = '-';
 s[i] = '\0';
 reverse(s);
}

char ** createarray(int row,int column){
     char **array = malloc(row * sizeof(char *));
     int i;
     for(i=0; i != row; ++i) {
         array[i] = malloc(column * sizeof(char));
     }
     return array;
}

char **readinput(char **args,int *i){
     char buffer[1];
     char prev=' ';
     int j=0;
     while(1){
        read(STDIN_FILENO, buffer, 1);
             if(buffer[0]=='\n'){
               break; 
             }
             else{
               // printf("3\n");
               args[0][*i]=buffer[0];
               *i=*i+1;
             }
     }
     args[0][*i]='\0';
     return args;
}
void slicing(char *buf, char **ret, int i1, int i2, int j){
	for (int i=i1;i<i2;i++){
		ret[j][i-i1]=buf[i];
	}
	ret[j][i2-i1]='\0';
}
void basictcpserver(int port, char*buf1){
	printf("Aaya mein chile mein\n");
	int			sockfd, newsockfd ; /* Socket descriptors */
	int			clilen;
	struct sockaddr_in	cli_addr, serv_addr;

	int i;
	// char *buf=malloc(100 * sizeof(char ));
	char **temp=createarray(1,max_string);
	slicing(buf1,temp,4,strlen(buf1)+1,0);
	char buf[100];
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Cannot create socket\n");
		exit(0);
	}
	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
	    error("setsockopt(SO_REUSEADDR) failed");
	}
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		= htons(port);
	printf("portno in ftp client->%d\n",port);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr)) < 0) {
		printf("Unable to bind local address\n");
		exit(0);
	}
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
				&clilen) ;
	if (newsockfd < 0) {
		printf("Accept error\n");
		exit(0);
	}
	close(sockfd);
	// while (1) {
	// 	int n1=recv(newsockfd, buf, 100, 0);
	// 	printf("%s\n",buf);
	// 	close(newsockfd);
	// 	break;
	// }
	int i8=0;
	int n1,x,fd;
	while(1){
		n1=recv(newsockfd, buf, 100, 0);
		int n2=(int)n1;
		if(n1>0 && i8==0){
			fd = open(temp[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
			x=write(fd, buf, n2);
		}
		else if(n1==0){
			printf("File not found\n");
			break;
		}
	    else if(n1>0 && i8>0){
			x=write(fd, buf, n2);
		}
		else {
			break;
		}
		i8++;
	}
	printf("End of child server\n");

}
void basictcpserverput(int port, char *buf){
	printf("Aaya mein chile mein\n");
	int			sockfd, newsockfd ; /* Socket descriptors */
	int			clilen;
	struct sockaddr_in	cli_addr, serv_addr;

	int i;
	// char *buf=malloc(100 * sizeof(char ));
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Cannot create socket\n");
		exit(0);
	}
	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
	    error("setsockopt(SO_REUSEADDR) failed");
	    printf("Setsockopt nahi hua\n");
	    exit(0);
	}
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		= htons(port);
	printf("portno in ftp client->%d\n",port);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr)) < 0) {
		printf("Unable to bind local address\n");
		exit(0);
	}
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
				&clilen) ;
	if (newsockfd < 0) {
		printf("Accept error\n");
		exit(0);
	}
	close(sockfd);
	int i8=0;
	int n1,x,fd;
	fd = open(buf, O_RDONLY);
	if(fd<0){
		return -1;
	}
	char msg[101];
	 while (1)
	 {
	   // process buffer
	   int nx=read(fd, msg, 100);
	   if (nx==0){
	   	break;
	   }
	   msg[nx]='\0';
	   send(newsockfd, msg, nx, 0);
	   // printf("%s\n",msg);
	 }
	close(newsockfd);
	printf("End of child server\n");
	return 1;
}

int main(int argc, char** argv)
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
	serv_addr.sin_port	= htons(atoi(argv[1]));
	if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}
	//yahan per port daalna hai.Phir isko ek normal variable me save kardenge.
	int port;
	int pid;
	int status;
	int flag=0;
	while(1){
		printf("> ");
		fflush(stdout);
		char **args=createarray(1,max_string);
		int i=0;
		args=readinput(args,&i);
		printf("%s\n",args[0]);
		char **temp=createarray(1,max_string);
		char **temp1=createarray(1,max_string);
		char **temp2=createarray(1,max_string);
		slicing(args[0],temp,0,4,0);
		slicing(args[0],temp1,0,3,0);
		slicing(args[0],temp2,0,2,0);
		if(!strcmp("port",temp[0])){
			slicing(args[0],temp,5,strlen(args[0]),0);
			port=atoi(temp[0]);
			send(sockfd, args[0], strlen(args[0]) + 1, 0);
			flag=1;
		}
		else if(!strcmp("get",temp1[0])){
			printf("Child forked with pid->%d\n",pid);
			pid=fork();
			if(pid==0){
				printf("Aaye mein\n");
				basictcpserver(port,args[0]);
				exit(0);
				printf("hi there\n");
			}
			else{
				send(sockfd, args[0], strlen(args[0]) + 1, 0);
				if(flag==0){
					kill(pid,SIGKILL);
					exit(0);
				}
				waitpid(pid,&status,0);
				printf("Wait kr rha hoon\n");
			}
			// recv(sockfd,buf,100,0);
			// printf("%s\n",buf);
			// kill(pid, SIGKILL);
		}
		else if(!strcmp("put",temp1[0])){
			printf("Child forked with pid->%d\n",pid);
			pid=fork();
			if(pid==0){
				printf("Aaye mein\n");
				char **temp1=createarray(1,max_string);
				slicing(args[0],temp1,4,strlen(args[0]),0);
				basictcpserverput(port,temp1[0]);
				exit(0);
				printf("hi there\n");
			}
			else{
				send(sockfd, args[0], strlen(args[0]) + 1, 0);
				if(flag==0){
					kill(pid,SIGKILL);
					exit(0);
				}
				waitpid(pid,&status,0);
				printf("Wait khatm hua hai\n");
			}
		}
		else if(!strcmp("quit",temp[0])){
			send(sockfd, args[0], strlen(args[0]) + 1, 0);
			close(sockfd);
			exit(0);
		}
		else if(!strcmp("cd",temp2[0])){
			send(sockfd, args[0], strlen(args[0]) + 1, 0);
		}
		int status_code=recvstatuscode(sockfd);
		if(status_code/100==5&&status_code!=501){
			printf("%s\n", "Socket closed from server due to wrong connection");
			exit(0);
		}
		// printf("%s\n",buf);
	}
}