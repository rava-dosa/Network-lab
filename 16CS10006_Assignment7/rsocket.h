#ifndef TRY_H
#define TRY_H

#define SOCK_MRP 4
#define TIMEOUT 2
#define PROB 0.5
uint8_t send_count;
uint8_t recv_count;
long unsigned int tid;
int recv_from_count;

int r_socket(int domain, int type, int protocol);
int r_bind(int sockfd, struct sockaddr *addr,socklen_t addrlen);
int r_sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
int r_recvfrom(int sockfd, void *buf, size_t len, int flags,const struct sockaddr *src_addr, socklen_t *addrlen);
int r_close(int fd);

#endif