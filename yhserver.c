/**
* 长连接主服务流程。
* 创建消息循环，注册accept、read、write事件
* 
* yuhao@baidu.com 
* 2014.1.26
*/

#include <errno.h>
#include <ctype.h>
#include <stdio.h>

#include "anet.h"
#include "ae.h"
#include "yhclient.h"
#include "yhlog.h"

#define NOTUSED(V) ((void) V)
#define MAX_CLIENTS 10000
#define BINDADDR_MAX 16
#define BUF_SIZE 1024
#define PORT 8001

char buffer[BUF_SIZE];

//清除资源
void freeResource(aeEventLoop *el, int fd, void* c){
	yhDebug("free resource...\n");
	aeDeleteFileEvent(el, fd, AE_READABLE);
	aeDeleteFileEvent(el, fd, AE_WRITABLE);
	close(fd);
	if(c != NULL) freeClient(c);
}

//写函数句柄
void writeTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask){
	yhDebug("into writeTcpHandler\n");
	int writen;
	NOTUSED(mask);
	yhclient* c = (yhclient*) privdata;

	writen = write(fd, c->data, strlen(c->data));

	if(writen == -1){
		if(errno == EAGAIN){
			writen = 0;
		} else {
			yhDebug("write err: %s\n", strerror(errno));
		}
	} else {
		yhDebug("sent : %d bytes\n", writen);
	}

	//freeResource(el, fd, NULL);
	aeDeleteFileEvent(el, fd, AE_WRITABLE);
}

//读函数句柄
void readTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask){
	yhDebug("into readTcpHandler\n");
	int readlen, res;
	yhclient* c = (yhclient*)privdata;
	NOTUSED(mask);

	readlen = read(fd, buffer, BUF_SIZE);
	if(readlen == -1){
		if(errno == EAGAIN){
			readlen = 0;
		} else {
			yhDebug("close conn: %s\n", strerror(errno));
			freeResource(el, fd, c);
		}
	} else if(readlen == 0){
		yhDebug("Client closed connection\n");
		freeResource(el, fd, c);
	}

	processBuf(buffer, c);

	res = aeCreateFileEvent(el, fd, AE_WRITABLE, writeTcpHandler, c);
	if(res != -1){
		yhDebug("add writable file event to fd %d\n", fd);
	}
}

//accept函数句柄
void acceptTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask){
	int cport, cfd, res;
	char cip[IP_STR_LEN];
	NOTUSED(el);
	NOTUSED(mask);
	NOTUSED(privdata);

	cfd = anetTcpAccept(buffer, fd, cip, sizeof(cip), &cport);
	if (cfd == -1){
		yhDebug("accept err\n");
		freeResource(el, cfd, NULL);
		return ;
	}
	yhDebug("accepted %d\n", cfd);

	yhclient* yhc = createClient(cfd);
	if(yhc == NULL){
		yhDebug("create yhclient err\n");
		free(cfd);
		return ;
	}
	yhc->cport = cport;
	memcpy(yhc->cip, cip, sizeof(cip));

	res = aeCreateFileEvent(el, cfd, AE_READABLE, readTcpHandler, yhc);
	if(res == -1){
		freeResource(el, cfd, yhc);
	} else {
		yhDebug("add read file event to fd: %d\n", cfd);
	}

	return ;
}

//例行任务
int cronJob(aeEventLoop *el, long long id, void *clientData){
	//do cron jobs here

	if(clientData) yhDebug("%d event: %s\n", clientData);
	return 5000;	//5000ms后继续
	//return -1;	//no more
}

int main(){
	aeEventLoop *el;
	int listenfd, port=PORT, res, connfd;
	char * cronTips = "yhserver is running ...";

	el = aeCreateEventLoop(MAX_CLIENTS);
	yhDebug("event loop created...\n");
	
	listenfd = anetTcpServer(buffer, port, NULL);

	res = aeCreateFileEvent(el, listenfd, AE_READABLE, acceptTcpHandler,NULL);
	if(res == AE_ERR){
		freeResource(el, listenfd, NULL);
		yhDebug("AE_ERR: create file event error\n");
		exit(1);
	} else {
		yhDebug("file event added:%d\n", listenfd);
	}

	if(aeCreateTimeEvent(el, 1000, cronJob, cronTips, NULL) == AE_ERR) {
		yhDebug("create time event error:\n");
		exit(2);
	}

	aeMain(el);
	aeDeleteEventLoop(el);

	return 0;
}

