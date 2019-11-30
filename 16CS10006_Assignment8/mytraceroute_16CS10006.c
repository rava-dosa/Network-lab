#include<stdio.h>
#include<stdlib.h>
#include <signal.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <linux/ip.h> 
#include <linux/udp.h> 
#include <linux/icmp.h>

#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <netdb.h>
#include <time.h>

#include <sys/cdefs.h>
#include <sys/types.h>

#define LISTEN_PORT 8082
#define DESTINATION_PORT 32165
#define PAYLOAD_SIZE  52

struct hostent* getIP(char domain_name[]){
	struct hostent *hp = gethostbyname(domain_name);
	if(hp==NULL){
		printf("This hostname does not exist\n");
		exit(0);
	}
	return hp;
}
unsigned short csum(unsigned short *buf, int nwords){       
        unsigned long sum;
        for(sum=0; nwords>0; nwords--)
            sum += *buf++;
        sum = (sum >> 16) + (sum &0xffff);
        sum += (sum >> 16);
        return (unsigned short)(~sum);
}
void get_payload(char payload[], int len){
	int i;
	for (i = 0; i < len; ++i)
	{
		payload[i] = 'B' + i;
	}
}

struct iphdr setipheader(struct sockaddr_in dest_addr, int TTL_value){
	struct iphdr header_ip;
	header_ip.ihl = 5;
	header_ip.version = 4;
	header_ip.tos = 2	;
	header_ip.tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + PAYLOAD_SIZE);
	header_ip.id = htons(DESTINATION_PORT);
	header_ip.frag_off = 0;
	header_ip.ttl = TTL_value;
	header_ip.protocol = 17;
	header_ip.check = 0;
	header_ip.saddr = 0;
	header_ip.daddr = dest_addr.sin_addr.s_addr;
	return header_ip;
}

struct udphdr setudpheader(){
	struct udphdr header_udp;
	header_udp.source = htons(LISTEN_PORT);
	header_udp.dest = htons(DESTINATION_PORT);
	header_udp.len = htons(sizeof(struct udphdr) + PAYLOAD_SIZE);
	header_udp.check = 0;
	return header_udp;	
}
void createpayload(char UDPMessage[],char payload[],struct udphdr header_udp,struct iphdr header_ip){
	int ipH = sizeof(struct iphdr);
	int udpH = sizeof(struct udphdr); 
	memcpy(UDPMessage, &header_ip, ipH);
	header_ip.check = csum((unsigned short *)UDPMessage, sizeof(struct iphdr));
	memcpy(UDPMessage+ipH, &header_udp, udpH);
	header_udp.check = csum((unsigned short *)UDPMessage, ipH+udpH );
	memcpy(UDPMessage+ipH+udpH, payload, PAYLOAD_SIZE);	
}
double gettimediff(struct timeval before_time,struct timeval after_time){
	double temp=1000.0*(after_time.tv_sec - before_time.tv_sec)+0.001*(after_time.tv_usec-before_time.tv_usec);
	return temp;
}

int handlereceive(int * counter, int * outer,int *TTL_value,int *default_case_counter,int sockfd_icmp, struct timeval before_time, struct timeval after_time, char *dest_ip){
	struct sockaddr_in raddr;		
	socklen_t raddr_len = sizeof(raddr);
	char msg[2048];
	int msglen = recvfrom(sockfd_icmp, msg, 2048, 0, (struct sockaddr *)&raddr, &raddr_len);
	gettimeofday(&after_time, NULL);
	double timediff = gettimediff(before_time,after_time);
	struct iphdr header_ip1;
	header_ip1 = *((struct iphdr *) msg);
	if(header_ip1.protocol != 1){
		return 0;
	}
	struct icmphdr hdr_icmp;
	char *x;
	hdr_icmp = *((struct icmphdr *) (msg+sizeof(struct iphdr)));
	if(hdr_icmp.type==3){
		printf("Hop_Count(%d)  %s  %fms\n", *TTL_value, inet_ntoa(raddr.sin_addr), timediff);
		*counter = 0;
		if(strcmp(inet_ntoa(raddr.sin_addr), dest_ip)==0)							
			*outer = -2;
		return 1;
	}
	else if(hdr_icmp.type==11){
		printf("Hop_Count(%d)  %s  %fms\n", *TTL_value, inet_ntoa(raddr.sin_addr), timediff);
		*counter = 0;
		*outer = 4;
		*TTL_value=*TTL_value+1;
		return 1;
	}
	else{
		*default_case_counter = 1;
		return 2;
	}

}
int main(int argc, char const *argv[])
{
		int TTL_value=1;
		char *domain_name = (char*)malloc(strlen(argv[1])*sizeof(char));
		strcpy(domain_name, argv[1]);
		struct hostent *hp = getIP(domain_name);
		char *dest_ip = inet_ntoa( *( struct in_addr*)( hp -> h_addr_list[0]));
		printf("%s\n", dest_ip);
		int S1, serv_addr_len;
		struct sockaddr_in serv_addr;
		if(((S1 = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) ){
			perror("raw socket IP_HDRINCL");
		}
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(LISTEN_PORT);
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr_len = sizeof(serv_addr);
		if((bind(S1, (struct sockaddr*) &serv_addr, serv_addr_len) < 0)){
			perror("raw bind");
		}
		int enable = 1;
		if(setsockopt(S1, IPPROTO_IP, IP_HDRINCL, &enable, sizeof(enable)) < 0){
			perror("setsockopt() error");
			exit(0);
		}
		int sockfd_icmp;
		struct sockaddr_in dest_addr;
		if((sockfd_icmp = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0){
			perror("raw socket");
			exit(0);
		}	
		dest_addr.sin_family = AF_INET;	
		dest_addr.sin_port = htons(DESTINATION_PORT);
		dest_addr.sin_addr.s_addr = inet_addr(inet_ntoa( *( struct in_addr*)( hp -> h_addr_list[0])));
	int outer;
	for (outer = 3; outer > 0 && TTL_value <= 30; outer--){
			struct iphdr header_ip=setipheader(dest_addr,TTL_value);
			struct udphdr header_udp=setudpheader();
			char payload[PAYLOAD_SIZE];
			get_payload(payload, PAYLOAD_SIZE);
			char UDPMessage[2048];
			int ipH = sizeof(struct iphdr);
			int udpH = sizeof(struct udphdr); 			
			createpayload(UDPMessage,payload,header_udp,header_ip);
			struct timeval before_time, after_time;
			int sendlen = sendto(S1, UDPMessage, PAYLOAD_SIZE+ipH+udpH, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
			gettimeofday(&before_time, NULL);
			fd_set readfs;
			int counter = 1, default_case_counter = 0;
			long T = 1, uT = 0;
			struct timeval timeout, remTime;
			while(counter){
				FD_ZERO(&readfs);
				int nfds = sockfd_icmp +1;
				FD_SET(sockfd_icmp, &readfs);
				if(default_case_counter == 0){
					timeout.tv_sec=T;	
					timeout.tv_usec=uT;	
				}
				else{
					timeout = remTime;
				}
				select(nfds, &readfs, 0, 0, &timeout);
				remTime = timeout;
				if(FD_ISSET(sockfd_icmp, &readfs)){
					/*receive message*/		
					int ret=handlereceive(&counter,&outer,&TTL_value,&default_case_counter,sockfd_icmp,before_time,after_time,dest_ip);
					switch(ret){
						case 0:
							continue;
						case 1:
							break;
						case 2:;
					}
				}
				else{
					counter = 0;	
					if(outer==1){
						printf("Hop_Count(%d) * *\n", TTL_value);
						outer = 4;
						TTL_value++;
					}
				}
			}
	}
	close(S1);
	close(sockfd_icmp);
	return 0;
}