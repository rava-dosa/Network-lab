/*
			NETWORK PROGRAMMING WITH SOCKETS

In this program we illustrate the use of Berkeley sockets for interprocess
communication across the network. We show the communication between a server
process and a client process.


*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
/* The following three files must be included for network programming */
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
			/* THE SERVER PROCESS */
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

main()
{
	int			sockfd, newsockfd ; /* Socket descriptors */
	int			clilen;
	struct sockaddr_in	cli_addr, serv_addr;

	int i;
	char buf[100];		/* We will use this buffer for communication */
	// char *buf;		/* We will use this buffer for communication */

	/* The following system call opens a socket. The first parameter
	   indicates the family of the protocol to be followed. For internet
	   protocols we use AF_INET. For TCP sockets the second parameter
	   is SOCK_STREAM. The third parameter is set to 0 for user
	   applications.
	*/
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}

	/* The structure "sockaddr_in" is defined in <netinet/in.h> for the
	   internet family of protocols. This has three main fields. The
 	   field "sin_family" specifies the family and is therefore AF_INET
	   for the internet family. The field "sin_addr" specifies the
	   internet address of the server. This field is set to INADDR_ANY
	   for machines having a single IP address. The field "sin_port"
	   specifies the port number of the server.
	*/
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		= htons(200002);

	/* With the information provided in serv_addr, we associate the server
	   with its port using the bind() system call. 
	*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr)) < 0) {
		perror("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, 5); /* This specifies that up to 5 concurrent client
			      requests will be queued up while the system is
			      executing the "accept" system call below.
			   */

	/* In this program we are illustrating an iterative server -- one
	   which handles client connections one by one.i.e., no concurrency.
	   The accept() system call returns a new socket descriptor
	   which is used for communication with the server. After the
	   communication is over, the process comes back to wait again on
	   the original socket descriptor.
	*/
	while (1) {

		/* The accept() system call accepts a client connection.
		   It blocks the server until a client request comes.

		   The accept() system call fills up the client's details
		   in a struct sockaddr which is passed as a parameter.
		   The length of the structure is noted in clilen. Note
		   that the new socket descriptor returned by the accept()
		   system call is stored in "newsockfd".
		*/
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
					&clilen) ;

		if (newsockfd < 0) {
			perror("Accept error\n");
			exit(0);
		}


		/* We initialize the buffer, copy the message to it,
			and send the message to the client. 
		*/
		
		// strcpy(buf,"I am god");
		// int n1=send(newsockfd, buf, strlen(buf) + 1, 0);
		int n1=recv(newsockfd, buf, 100, 0);
		buf[n1]='\0';
		printf("%d->%s\n",n1,buf);
		// FILE *fp=fopen(buf,"r");
		char msg[101];
		memset(msg,0,sizeof(msg));
		int fd = open(buf, O_RDONLY); 
		if(fd>0){
				char s123[1];
				memset(s123,0,sizeof(s123));
				s123[0]='L';
				send(newsockfd,s123,1,0);
				printf("Entered in fd>0\n");
				struct stat st;
				stat(buf, &st);
				long long int size = st.st_size;
				char snum[29];
				memset(snum,0,sizeof(snum));
				itoa(size, snum);
				send(newsockfd, snum, 29, 0);
				printf("%s\n",snum);
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
				 // msg[0]='\0';
				 // send(newsockfd, msg, strlen(msg)+ 1, 0);
				 close(newsockfd);
		}
		else {
			char snum[30];
			memset(snum,0,sizeof(snum));
			snum[0]='E';		
			send(newsockfd, snum, 30, 0);
			printf("%d\n", fd);
			close(newsockfd);	
		}
		/* We now receive a message from the client. For this example
  		   we make an assumption that the entire message sent from the
  		   client will come together. In general, this need not be true
		   for TCP sockets (unlike UDPi sockets), and this program may not
		   always work (for this example, the chance is very low as the 
		   message is very short. But in general, there has to be some
	   	   mechanism for the receiving side to know when the entire message
		  is received. Look up the return value of recv() to see how you
		  can do this.
		*/ 
		// n1=recv(newsockfd, buf, 100, 0);
		// printf("%d->%s\n",n1,buf);
		// printf("%s\n", buf);
		// close(newsockfd);
		// sleep(10);
	}
}
			


