struct corotine{
	void *func(void *argc);
	void *argc;
	void *stack;
	size_t stack_size;
	struct corotine *current;
	//TATLQ_ENTRY(corotine) ready;  
	struct corotine *ready_next; 
	RB_ENTRY(corotine) sleep;
	RB_ENTRY(corotine) wait; 	
	struct RB {
		struct	RB *left;
		struct	RB *right;
		struct	RB *parent;
		char color;
	}sleep;
	
}
while (1) {
	int nready = epoll_wait(epfd, events, EVENT_SIZE, -1);
	for (i = 0;i < nready;i ++) {
		int sockfd = events[i].data.fd;
		if (sockfd == listenfd) {
			int connfd = accept(listenfd, xxx, xxxx);
			setnonblock(connfd);
			ev.events = EPOLLIN | EPOLLET;
			ev.data.fd = connfd;
			epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
		} else {
			handle(sockfd);
		}
	}
}

int handle(int sockfd) {
	recv(sockfd, rbuffer, length, 0);
	parser_proto(rbuffer, length);
	send(sockfd, sbuffer, length, 0);
}
int thread_cb(int sockfd) {
	// 此函数是在线程池创建的线程中运行。
	// 与 handle 不在一个线程上下文中运行
	recv(sockfd, rbuffer, length, 0);
	parser_proto(rbuffer, length);
	send(sockfd, sbuffer, length, 0);
}
int handle(int sockfd) {
	//此函数在主线程 main_thread 中运行
	//在此处之前，确保线程池已经启动。
	push_thread(sockfd, thread_cb); //将 sockfd 放到其他线程中运行。
}

while(1) {
	socklen_t len = sizeof(struct sockaddr_in);
	int clientfd = accept(sockfd, (struct sockaddr*)&remote, &len);
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, client_cb, &clientfd);
}

while (1) {
	//epfd  这个是全局的，通过这个可以查询fd状态
	int nready = epoll_wait(epfd, events, EVENT_SIZE, -1);
	for (i = 0;i < nready;i ++) {
		int sockfd = events[i].data.fd;
		if (sockfd == listenfd) {
			int connfd = accept(listenfd, xxx, xxxx);
			setnonblock(connfd);
			ev.events = EPOLLIN | EPOLLET;
			ev.data.fd = connfd;
			epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
		} else {
			epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
			recv(sockfd, buffer, length, 0);
			//parser_proto(buffer, length);
			send(sockfd, buffer, length, 0);
			epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, NULL);
		}
	}
}


void nty_schedule_run(void)
POSIX 异步封装 API：
int nty_socket(int domain, int type, int protocol)
int nty_accept(int fd, struct sockaddr *addr, socklen_t *len)
int nty_recv(int fd, void *buf, int length)
int nty_send(int fd, const void *buf, int length)
int nty_close(int fd)

int nty_coroutine_create(nty_coroutine **new_co, proc_coroutine func,void *arg) {
	assert(pthread_once(&sched_key_once,
	nty_coroutine_sched_key_creator) == 0);
	nty_schedule *sched = nty_coroutine_get_sched();
	if (sched == NULL) {
		nty_schedule_create(0);
		sched = nty_coroutine_get_sched();
	if (sched == NULL) {
		printf("Failed to create scheduler\n");
		return -1;
	}
}
nty_coroutine *co = calloc(1, sizeof(nty_coroutine));
	if (co == NULL) {
		printf("Failed to allocate memory for new coroutine\n");
		return -2;
	}
	//
	int ret = posix_memalign(&co->stack, getpagesize(),sched->stack_size);
	if (ret) {
		printf("Failed to allocate stack for new coroutine\n");
		free(co);
		return -3;
	}
	co->sched = sched;
	co->stack_size = sched->stack_size;
	co->status = BIT(NTY_COROUTINE_STATUS_NEW); //
	co->id = sched->spawned_coroutines ++;
	co->func = func;
	co->fd = -1;
	co->events = 0;
	co->arg = arg;
	co->birth = nty_coroutine_usec_now();
	*new_co = co;
	TAILQ_INSERT_TAIL(&co->sched->ready, co, ready_next);
	return 0;
}





































