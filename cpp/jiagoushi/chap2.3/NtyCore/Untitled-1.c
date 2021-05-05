





while(1){
    int nready = epoll_wait();
    for(i = 0;i <nready; i++){
        aync_read(event[i].data.fd, buffer, length);
        parse();
        aync_send();
    }
}

aync_read  //1. 判断fd有没有阻塞  2. 不能挂起
{
    epoll_ctrl(epfd, EPOLL_CTL_DEL,vent[i].data.fd,ev);
    int n = poll(fd);//又来了新的事件
    if(n == -1) swap();//切换
    recv();
    send();
    epoll_ctrl(epfd, EPOLL_CTL_ADD,vent[i].data.fd,ev);
}
swap()  ////触发下一次













