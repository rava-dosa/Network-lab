#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h> 

#include "rsocket.h"

extern uint8_t send_count=0;
extern uint8_t recv_count=0;
extern int recv_from_count=0;
extern long unsigned int tid;
pthread_mutex_t lock;
pthread_mutex_t lock1;
int count_glob=0;
// pthread_mutex_t recv_count_mutex;

const struct sockaddr *globaddr;
struct sockaddr *globaddr1;
// struct sockaddr_in globsendaddr1;
socklen_t globaddrlen;

typedef struct {
	int id;
	int valid;
	int size;
	void *buf;
	time_t seconds;
	in_port_t port;
	struct in_addr addr; 
} dataframe;

// dataframe recv_data[101];
dataframe *recv_data;
dataframe *unacknowledged_message;
// dataframe unacknowledged_message[101];
// int received_message_id[101];
int *received_message_id;
// int *received_message_id=(int *)malloc(101*sizeof(int));
//100 wala check daalna hoga
int dropmessage(float f){
	float a=(float)rand() / (float)RAND_MAX ;
	if(a<f){
		return 1;
	}
	else{
		return 0;
	}

}

int send_helper(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen){
	int h=len;
	count_glob++;
	if(dropmessage(PROB)){
		h=sendto(sockfd,buf,len,flags,dest_addr,addrlen);
	}
	return h; 
}
void HandleACKMsgRecv(void *buf_recv){
	//yahan per mutex wutex dekhna padega
	int id=*((int *)buf_recv);
	// printf("received ack for ->%d\n",id);
	for(int i=0;i<send_count;i++){
		if(unacknowledged_message[i].valid==1&&unacknowledged_message[i].id==id){
			unacknowledged_message[i].valid=0;
			// printf("Set valid bit to %d\n",unacknowledged_message[i].valid);
			break;
		}
	}
}

void HandleAppMsgRecv(void *buf_recv,int n, int sockfd){
	uint8_t id;
	memcpy(&id,buf_recv,1);
	// printf("Appmsg->%d\n",id);
	int i;
	for(i=0;i<recv_count;i++){
		// printf("received->%d\n",received_message_id[i]);
		if(received_message_id[i]==id){
			// printf("return->%d\n",i);
			int h=send_helper(sockfd,&id,1,0,globaddr1,globaddrlen);
			// printf("Received duplicate with id->%d\n",id);
			// printf("Ack sent with id->%d,ret=>%d\n",id,h);
			return;
		}
	}
	pthread_mutex_lock(&lock);	
	received_message_id[recv_count]=id;
	recv_data[recv_count].valid=1;
	recv_data[recv_count].id=id;
	recv_data[recv_count].size=n-1;
	buf_recv++;
	void *buf2=malloc(n-1);
	memcpy(buf2,buf_recv,n-1);
	recv_data[i].buf=buf2; 	
	recv_count++;
	pthread_mutex_unlock(&lock); 
	// printf("Data added->%s\n",buf2);
	// srand(100);
	int h;
	int rand1=rand();
	// printf("rand->%d\n",rand1);
	h=send_helper(sockfd,&id,1,0,globaddr1,globaddrlen);
	// printf("Ack sent with id->%d,ret=>%d\n",id,h);
	//check the received meessage id if available means duplicate.
}

void HandleReceive(int sockfd){
		void *buf_recv=malloc(103);
		int n=recvfrom(sockfd,buf_recv, 103,0,globaddr1,&globaddrlen);
		if(n<0){
			perror("Error is receiving:-");
		}
		if(n==1){
			//ack aur ye sabe ke liye
			HandleACKMsgRecv(buf_recv);
		}
		else if(n>1){
			HandleAppMsgRecv(buf_recv,n,sockfd);;
		}	
}
void *thread_x(void *x){
	int sockfd=*(int *)x;
	fd_set rfds;
	struct timeval tv;
	int retval;
	while(1){
		FD_ZERO(&rfds);
		FD_SET(sockfd, &rfds);	
		tv.tv_sec = 1;
		tv.tv_usec = 0;			
		retval = select(sockfd+1, &rfds, NULL, NULL, &tv);	
		if(retval==-1){
			perror("select:-");
		}
		else if(FD_ISSET(sockfd, &rfds)){
			HandleReceive(sockfd);
		}
		else{
			int ty=time(NULL);
			// pthread_mutex_lock(&lock1);
			for(int j=0;j<send_count;j++){
				if(unacknowledged_message[j].valid==1){
					if((ty-unacknowledged_message[j].seconds)>TIMEOUT){
						struct sockaddr_in sendaddr;
						sendaddr.sin_family=AF_INET;
						sendaddr.sin_addr=unacknowledged_message[j].addr;
						sendaddr.sin_port=unacknowledged_message[j].port;
						int rey=send_helper(sockfd,unacknowledged_message[j].buf,unacknowledged_message[j].size,0,(const struct sockaddr *)&sendaddr,sizeof(sendaddr));
						if(rey==-1){
							perror("Retransmit me load hai");
						}
					}
				}
			}
			// pthread_mutex_lock(&lock1);
		}
	}
}

int r_socket(int domain, int type, int protocol){
    if (pthread_mutex_init(&lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    }
    if (pthread_mutex_init(&lock1, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    }    
    received_message_id=(int *)malloc(101*sizeof(int));
    unacknowledged_message=(dataframe *)malloc(101*sizeof(dataframe));	
    recv_data=(dataframe *)malloc(101*sizeof(dataframe));	
	memset(received_message_id,-1,101*sizeof(int));
	int sockfd;
	if(type==SOCK_MRP){
		sockfd=socket(AF_INET, SOCK_DGRAM, 0);
		//yahan per thread bhi banega.
		pthread_create(&tid, NULL, thread_x, (void *)&sockfd);
		return sockfd;
	}
	else{
		return -1;
	}
}
int r_bind(int sockfd, struct sockaddr *addr,socklen_t addrlen){
	globaddr=(const)addr;
	globaddr1=addr;
	globaddrlen=addrlen;
	int ret=bind(sockfd,addr,addrlen);
	return ret;
}
int r_sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen){
	// memcpy(&globsendaddr1,dest_addr,addrlen);
	if(len>100){
		return -1;
	}
	else{
		void *buf2=malloc(len+1);
		memcpy(buf2,&send_count,1);
		buf2++;
		memcpy(buf2,buf,len);
		buf2--;
		int ret=send_helper(sockfd,buf2,len+1,flags,dest_addr,addrlen);
		if(ret>0){
			unacknowledged_message[send_count].buf=buf2;
			unacknowledged_message[send_count].id=send_count;
			unacknowledged_message[send_count].seconds=time(NULL);
			unacknowledged_message[send_count].size=len+1;
			unacknowledged_message[send_count].valid=1;
			unacknowledged_message[send_count].port=(*((struct sockaddr_in *)dest_addr)).sin_port;
			unacknowledged_message[send_count].addr=(*((struct sockaddr_in *)dest_addr)).sin_addr;
			pthread_mutex_lock(&lock1);
			send_count++;
			pthread_mutex_unlock(&lock1);
			return ret-1;
		}
		else{
			return ret;
		}
	}
}
int r_recvfrom(int sockfd, void *buf, size_t len, int flags,const struct sockaddr *src_addr, socklen_t *addrlen){
	int ret;
	while(1){
		pthread_mutex_lock(&lock);
		if(recv_from_count<recv_count){
			strcpy(buf,recv_data[recv_from_count].buf);
			ret = recv_data[recv_from_count].size;
			src_addr=globaddr;
			addrlen=&globaddrlen;
			recv_from_count++;
			pthread_mutex_unlock(&lock);
			break;
		}
		else{
			pthread_mutex_unlock(&lock);
			sleep(1);
		}
	}
	return ret;
}
int r_close(int sockfd){
	int x;
	while(1){
		for(x=0;x<send_count;x++){
			if(unacknowledged_message[x].valid==1){
				// printf("Phasa mein->%d\n",x);
				x=0;
			}
		}
		break;
	}
	free(received_message_id);
	free(unacknowledged_message);
	free(recv_data);
	pthread_cancel(tid);
	close(sockfd);
	printf("Total Send trial->%d\n",count_glob);
}
