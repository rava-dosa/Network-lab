#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h> 
#include <time.h>

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
void sendint(int sockfd, int status_code){
	int converted_number = htonl(status_code);
	write(sockfd, &converted_number, sizeof(converted_number));
}
char count(char *buf, int n, char prev){
	// int len=strlen(buf);
	int word=0;
	for(int i=0;i<n;i++){
		char c=buf[i];
		if(isalnum(c)&&!isalnum(prev)){
			// printf("inloop\n");
			word=word+1;
		}
		prev=c;
	}
	if(word>2){
		word=1;
	}
	else{
		word=0;
	}
	return word;
}
char ** createarray(int row,int column){
     char **array = malloc(row * sizeof(char *));
     int i;
     for(i=0; i != row; ++i) {
         array[i] = malloc(column * sizeof(char));
     }
     return array;
}
void slicing(char *buf, char **ret, int i1, int i2, int j){
	for (int i=i1;i<i2;i++){
		ret[j][i-i1]=buf[i];
	}
	ret[j][i2-i1]='\0';
}
void getdata(char *buf, char **command,int *port){
	slicing(buf,command,0,4,0);
	char **portstr=createarray(1,max_string);
	slicing(buf,portstr,5,strlen(buf),0);
	printf("%s,%s\n",command[0],portstr[0]);
	*port=atoi(portstr[0]);

}
int forkclient(int port, char *buf){
	int			sockfd ;
	struct sockaddr_in	serv_addr;

	int i;
	// char buf[100];

	/* Opening a socket is exactly similar to the server process */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}
	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
	    error("setsockopt(SO_REUSEADDR) failed");
	    printf("Setsockopt nahi hua\n");
	    exit(0);
	}
	serv_addr.sin_family	= AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port	= htons(port);
	signal(SIGPIPE,signal_callback_handler);
	/* With the information specified in serv_addr, the connect()
	   system call establishes a connection with the server process.
	*/
	if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}
	int fd = open(buf, O_RDONLY);
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
	   send(sockfd, msg, nx, 0);
	   // printf("%s\n",msg);
	 }	
	// int n1=send(sockfd, buf, strlen(buf)+1, 0);
	// printf("%d->%s\n",n1, buf);
	close(sockfd);
	return 1;
	// exit(1);
}
int forkclientput(int port, char *temp){
	int			sockfd ;
	struct sockaddr_in	serv_addr;

	int i;
	// char buf[100];
	/* Opening a socket is exactly similar to the server process */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}
	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
	    error("setsockopt(SO_REUSEADDR) failed");
	}
	serv_addr.sin_family	= AF_INET;
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	serv_addr.sin_port	= htons(port);
	signal(SIGPIPE,signal_callback_handler);
	/* With the information specified in serv_addr, the connect()
	   system call establishes a connection with the server process.
	*/
	if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}
	int i8=0;
	int n1,x,fd;
	char buf[101];
	while(1){
		n1=recv(sockfd, buf, 100, 0);
		int n2=(int)n1;
		if(n1>0 && i8==0){
			printf("%s\n",temp);
			fd = open(temp, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			x=write(fd, buf, n2);
		}
		else if(n1==0){
			printf("No more bytes to read\n");
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
	close(sockfd);
	return 1;
	// exit(1);
}

int space(char *buf,int n){
	int count=0;
	for(int i=0;i<n;i++){
		if(buf[i]==" "){
			count++;
		}
	}
	if(count >1){
		return 1;
	}
	else{
		return 0;
	}
}
int main(int argc, char** argv)
{
	int			sockfd, newsockfd ; /* Socket descriptors */
	int			clilen;
	struct sockaddr_in	cli_addr, serv_addr;

	int i;
	// char *buf=malloc(100 * sizeof(char ));
	char buf[100];
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Cannot create socket\n");
		exit(0);
	}
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		= htons(atoi(argv[1]));
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr)) < 0) {
		printf("Unable to bind local address\n");
		exit(0);
	}
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	while(1){
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
					&clilen) ;
		if (newsockfd < 0) {
			printf("Accept error\n");
			exit(0);
		}
		int j=0;
		int port;
		int pid;
		while (1) {
			for(int k=0;k<100;k++) buf[k]='\0';
			int n;
			n=recv(newsockfd, buf, 100, 0);
			printf("%s\n",buf);
			// printf("just below print\n");
			if(!strcmp(buf,"quit")){
				sendint(newsockfd,421);
				break;
			}
			if(count(buf,n," ")){
				sendint(newsockfd,501);
				continue;
			}
			if(j==0){
				char **command=createarray(1,max_string);
				getdata(buf,command,&port);
				if(strcmp(command[0],"port")){
					sendint(newsockfd,503);
					break;
				}
				else if(!(port>1024 && port < 65535)){
					sendint(newsockfd,550);
					break;			
				}
				sendint(newsockfd,200);
			}
			if(j>=1){
				char **command=createarray(2,max_string);
				char **command1=createarray(1,max_string);
				// getdata(buf,command,&port);
				slicing(buf,command,0,3,0);
				slicing(buf,command1,0,2,0);
				if(!strcmp(command[0],"get")){
					printf("aaya na tu\n");
					pid=fork();
					if(pid==0){
						int ret2;
						char **temp=createarray(1,max_string);
						slicing(buf,temp,4,strlen(buf),0);
						ret2=forkclient(port,temp[0]);
						exit(ret2);
					}
					else{
						int status;
						waitpid(pid,&status,0);
						if(status/256==1){
							sendint(newsockfd,250);
						}
						else{
							sendint(newsockfd,550);
						}
					}
				}
				else if(!strcmp(command[0],"put")){
					printf("aaya na tu\n");
					pid=fork();
					if(pid==0){
						int ret2;
						char **temp=createarray(1,max_string);
						slicing(buf,temp,4,strlen(buf),0);
						printf("In child temp0->%s\n",temp[0]);
						ret2=forkclientput(port,temp[0]);
						printf("in child, return status->%d\n",ret2);
						exit(ret2);
					}
					else {
						int status;
						waitpid(pid,&status,0);
						printf("status->%d\n",status);
						if(status/256==1){
							sendint(newsockfd,250);
						}
						else{
							sendint(newsockfd,550);
						}
					}
				}
				else if(!strcmp(command1[0],"cd")){
					// printf("aaya na tu\n");
					char **temp=createarray(1,max_string);
					slicing(buf,temp,3,strlen(buf),0);
					int ret1=chdir(temp[0]);
					if(ret1>=0){
						sendint(newsockfd,200);
					}
					else{
						sendint(newsockfd,501);
					}
					char s1[100];
					printf("%d,%s\n",ret1,getcwd(s1, 100));
				}
			}
		j++;
		if(j>100){
			break;
		}
		}
		printf("socket closed\n");
		close(newsockfd);
	}
}
// if (fork() == 0) {
// 	close(sockfd);
// 	for(i=0; i < 100; i++) buf[i] = '\0';
// 	recv(newsockfd, buf, 100, 0);
// 	printf("%s\n", buf);
// 	close(newsockfd);
// 	printf("Socket going to be closed\n");
// 	exit(0);
// }