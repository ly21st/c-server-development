#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>
#include <sys/timeb.h>


#define MAX_BUFFER		128
#define MAX_EPOLLSIZE	(384*1024)
#define MAX_PORT		100

void LogError(const char* _FLE,const char* _FUN,unsigned int _LNE,const char *fmt, ...);
void LogInfo(const char* _FLE,const char* _FUN,unsigned int _LNE,const char *fmt, ...);

#define TIME_SUB_MS(tv1, tv2)  ((tv1.tv_sec - tv2.tv_sec) * 1000 + (tv1.tv_usec - tv2.tv_usec) / 1000)

int isContinue = 0;

static int ntySetNonblock(int fd) {
	int flags;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) return flags;
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0) return -1;
	return 0;
}

static int ntySetReUseAddr(int fd) {
	int reuse = 1;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));
}



int main(int argc, char **argv) {
	if (argc <= 2) {
		printf("Usage: %s ip port\n", argv[0]);
		exit(0);
	}

	const char *ip = argv[1];
	int port = atoi(argv[2]);
	int connections = 0;
	char buffer[128] = {0};
	int i = 0, index = 0;

	struct epoll_event events[MAX_EPOLLSIZE];
	
	int epoll_fd = epoll_create(MAX_EPOLLSIZE);
	
	strcpy(buffer, " Data From MulClient\n");
		
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);

	struct timeval tv_begin;
	gettimeofday(&tv_begin, NULL);

	while (1) {
		if (++index >= MAX_PORT) index = 0;
		
		struct epoll_event ev;
		int sockfd = 0;

		if (connections < 340000 && !isContinue) {
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd == -1) {
			    LogError(__FILE__, __func__, __LINE__, "socket error\n");
				goto err;
			}

			//ntySetReUseAddr(sockfd);
			addr.sin_port = htons(port+index);

			if (connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0) {
                LogError(__FILE__, __func__, __LINE__, "connect error\n");
				goto err;
			}
			ntySetNonblock(sockfd);
			ntySetReUseAddr(sockfd);

			snprintf(buffer, sizeof(buffer), "Hello Server: client --> %d\n", connections);
			send(sockfd, buffer, strlen(buffer), 0);

			ev.data.fd = sockfd;
			ev.events = EPOLLIN | EPOLLOUT;
			epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev);
		
			connections ++;
		}
		//connections ++;
		if (connections % 1000 == 999 || connections >= 340000) {
			struct timeval tv_cur;
			memcpy(&tv_cur, &tv_begin, sizeof(struct timeval));
			
			gettimeofday(&tv_begin, NULL);

			int time_used = TIME_SUB_MS(tv_begin, tv_cur);
			printf("connections: %d, sockfd:%d, time_used:%d\n", connections, sockfd, time_used);

			int nfds = epoll_wait(epoll_fd, events, connections, 100);
			for (i = 0;i < nfds;i ++) {
				int clientfd = events[i].data.fd;
				if (events[i].events & EPOLLOUT) {
					snprintf(buffer, sizeof(buffer), "data from %d\n", clientfd);
					if (sockfd > 0) send(sockfd, buffer, strlen(buffer), 0);
                //    send(clientfd, buffer, strlen(buffer), 0);
				} else if (events[i].events & EPOLLIN) {
					char rBuffer[MAX_BUFFER] = {0};
				//	if (sockfd <= 0) continue;
					ssize_t length = recv(sockfd, rBuffer, MAX_BUFFER, 0);
                //    ssize_t length = recv(clientfd, rBuffer, MAX_BUFFER, 0);
                    rBuffer[MAX_BUFFER-1] = 0;
					if (length > 0) {
						printf(" RecvBuffer:%s\n", rBuffer);

						if (!strcmp(rBuffer, "quit")) {
							isContinue = 0;
						}
						
					} else if (length == 0) {
						printf(" Disconnect clientfd:%d\n", clientfd);
                        LogError(__FILE__, __func__, __LINE__, "recv data len is 0\n");
						connections --;
						close(clientfd);
					} else {
						if (errno == EINTR) continue;
						if (errno == EAGAIN || errno == EWOULDBLOCK) continue;

						printf(" Error clientfd:%d, errno:%d\n", clientfd, errno);
                        LogError(__FILE__, __func__, __LINE__, "recv error\n");
						close(clientfd);
					}
				} else {
					printf(" clientfd:%d, errno:%d\n", clientfd, errno);
                    LogError(__FILE__, __func__, __LINE__, "unknow even\n");
					close(clientfd);
				}
			}
		}

		usleep(1 * 1000);
	}

	return 0;

err:
    LogError(__FILE__, __func__, __LINE__, "error:\n");
	return 0;
	
}

char* timeNowWithMs(char* now) {
	struct tm tme;
	struct timeb timebuffer;
	time_t t;

	if (now == NULL) {
		return (NULL);
	}
	t = time(0);
	ftime(&timebuffer);
	memcpy(&tme, localtime(&t), sizeof(struct tm));
	sprintf(now, "%04d/%02d/%02d %02d:%02d:%02d.%03hu",
			tme.tm_year + 1900, tme.tm_mon + 1, tme.tm_mday, tme.tm_hour,
			tme.tm_min, tme.tm_sec, timebuffer.millitm);
	return (now);
}

void LogError(const char* _FLE,const char* _FUN,unsigned int _LNE,const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	char now[32];
	static pid_t pid = 0;

	if (pid == 0) {
		pid = getpid();
	}

	fprintf(
			stdout,
			"%s - [Error] PSID:%06u FILE:%s FUNC:%s LINE:%u SYEN:%d SYER:%s [MSG]- ",
			timeNowWithMs(now), pid, _FLE, _FUN, _LNE,
			errno,
			strerror(errno));
	vfprintf(stdout,fmt,ap);
	va_end(ap);
}

void LogInfo(const char* _FLE,const char* _FUN,unsigned int _LNE,const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	char now[32];
	static pid_t pid = 0;

	if (pid == 0) {
		pid = getpid();
	}

	timeNowWithMs(now);
	fprintf(
			stdout,
			"%s - [Info] PSID:%06u FILE:%s FUNC:%s LINE:%u [MSG]- ",
			timeNowWithMs(now), pid, _FLE, _FUN, _LNE);
	vfprintf(stdout,fmt,ap);
	va_end(ap);
}
