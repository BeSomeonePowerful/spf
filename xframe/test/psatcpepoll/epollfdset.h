// the code of this file is copy from resip

#ifndef _FDSET_H
#define _FDSET_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include "comtypedef_vchar.h"
#include <sys/epoll.h>
#define MAX_SOCKET_EVENTS 1024
#define MAXSIZE     1024
class CFdSet
{
private:
    int _iepfd;
public:
   CFdSet()
   {
      _iepfd = epoll_create(MAX_SOCKET_EVENTS);
   }

   ~CFdSet(){
      close(_iepfd);
   }

   /*添加句柄
    * @param enableRead: 设置是否可读
    * @param enableWrite: 设置是否可写
    * @return  操作是否成功, true – 成功, false – 失败
    */
   bool addEvent(int fd, bool enableRead, bool enableWrite) {

       struct epoll_event ev;
       memset(&ev, 0, sizeof(ev));
       ev.data.fd = fd;
       // 设置要处理的事件类型
       ev.events = 0;

       if (enableRead) {
           ev.events |= EPOLLIN;
       }
       if (enableWrite) {
           ev.events |= EPOLLOUT;
       }
       bool rc = (epoll_ctl(_iepfd, EPOLL_CTL_ADD, fd, &ev) == 0); //成功返回0，失败返回-1
       return rc;
   }

   /*修改设置句柄
    * @param enableRead: 设置是否可读
    * @param enableWrite: 设置是否可写
    * @return  操作是否成功, true – 成功, false – 失败
    */
   bool setEvent(int fd, bool enableRead, bool enableWrite) {

       struct epoll_event ev;
       memset(&ev, 0, sizeof(ev));
       ev.data.fd = fd;
       // 设置要处理的事件类型
       ev.events = 0;

       if (enableRead) {
           ev.events |= EPOLLIN;
       }
       if (enableWrite) {
           ev.events |= EPOLLOUT;
       }
       bool rc = (epoll_ctl(_iepfd, EPOLL_CTL_MOD, fd, &ev) == 0);
       return rc;
   }

   /*
    *删除句柄
    * @param socket 被删除socket
    * @return  操作是否成功, true – 成功, false – 失败
    */
   bool removeEvent(int fd) {

       struct epoll_event ev;
       memset(&ev, 0, sizeof(ev));
       ev.data.fd = fd;
       // 设置要处理的事件类型
       ev.events = 0;
       bool rc = (epoll_ctl(_iepfd, EPOLL_CTL_DEL, fd, &ev) == 0);
       return rc;
   }

   /*
    *获取事件
    * @param socket 被删除socket
    * @return  操作是否成功, true – 成功, false – 失败
    */
   int getEvents(int timeout, int cnt) {

       struct epoll_event events[MAX_SOCKET_EVENTS];

       if (cnt > MAX_SOCKET_EVENTS) {
           cnt = MAX_SOCKET_EVENTS;
       }

       int res = epoll_wait(_iepfd, events, cnt , timeout);

       /*if (res > 0) {
          memset(ioevents, 0, sizeof(IOEvent) * res);
        }

       for (int i = 0; i < res; i++) {
           if (events[i].events & (EPOLLERR | EPOLLHUP)) {
               ioevents[i]._errorOccurred = true;
           }
           if ((events[i].events & EPOLLIN) != 0) {
               ioevents[i]._readOccurred = true;
           }
           if ((events[i].events & EPOLLOUT) != 0) {
               ioevents[i]._writeOccurred = true;
           }
       }*/
       return res;
   }

   void do_epoll(int listenfd)
   {
       struct epoll_event events[MAX_SOCKET_EVENTS];
       int ret;
       char buf[MAXSIZE];
       memset(buf,0,MAXSIZE);
       //创建一个描述符
       //添加监听描述符事件
       addEvent(listenfd,true,false);
       for ( ; ; )
       {
           //获取已经准备好的描述符事件
           ret = epoll_wait(_iepfd,events,MAX_SOCKET_EVENTS,-1);
           //printf("\n");
           handle_events(_iepfd,events,ret,listenfd,buf);
       }
        printf("????\n");
        close(_iepfd);
   }

   void
   handle_events(int _iepfd,struct epoll_event *events,int num,int listenfd,char *buf)
   {
       int i;
       int fd;
       //进行选好遍历
       for (i = 0;i < num;i++)
       {
           fd = events[i].data.fd;
           //根据描述符的类型和事件类型进行处理
           if ((fd == listenfd) &&(events[i].events & EPOLLIN))
               handle_accpet(_iepfd,listenfd);
           else if (events[i].events & EPOLLIN){
              //printf("read doing....\n");
               do_read(_iepfd,fd,buf);
            }
           else if (events[i].events & EPOLLOUT){
              //printf("write doing.....\n");
               do_write(_iepfd,fd,buf);
             }
       }
   }

   void handle_accpet(int _iepfd,int listenfd)
   {
       int clifd;
       struct sockaddr_in cliaddr;
       socklen_t  cliaddrlen = sizeof(cliaddr);
       clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
       if (clifd == -1)
           perror("accpet error:");
       else
       {
           printf("accept a new client: %s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
           //添加一个客户描述符和事件
           addEvent(clifd,true,false);
       }
   }

   void do_read(int _iepfd,int fd,char *buf)
   {
       int nread;
       nread = read(fd,buf,MAXSIZE);
       //printf("nread:%d\n", nread);
       if (nread == -1)
       {
           perror("read error:");
           close(fd);
           removeEvent(fd);
       }
       else if (nread == 0)
       {
           fprintf(stderr,"client close.\n");
           close(fd);
           removeEvent(fd);
       }
       else
       {
           printf("read message is : %s\n",buf);
          //printf("..........%s\n",buf);
           //修改描述符对应的事件，由读改为写
           if(setEvent(fd,false,true)) {
              //printf("readtowrite...\n");
           }
       }
   }

   void do_write(int _iepfd,int fd,char *buf)
   {
       int nwrite;
       nwrite = write(fd,buf,strlen(buf));
       if (nwrite == -1)
       {
           perror("write error:");
           close(fd);
           removeEvent(fd);
       }
       else
           if(setEvent(fd,true,false)) {
              //printf("writetoread.....\n");
           }
       memset(buf,0,MAXSIZE);
   }
};
#endif
