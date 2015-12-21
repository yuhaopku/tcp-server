/* client.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define MAXLINE 80
#define SERV_PORT 8001

long long ustime(){
	struct timeval tv;
	long long ust;

	gettimeofday(&tv, NULL);
	ust = ((long long)tv.tv_sec)*1000000;
	ust += tv.tv_usec; 
	return ust;
}

int main(int argc, char *argv[])
{
	struct sockaddr_in servaddr;
	char buf[MAXLINE];
	int sockfd, n, s;
	int *intp = NULL;
	char *str = "&cuid=sadgrbgade&mcc=460&mnc=1&lac=34666&cid=45726";
	int cids[2] = {34666, 45726};
  
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "10.46.93.65", &servaddr.sin_addr);
	servaddr.sin_port = htons(SERV_PORT);
  
	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	write(sockfd, str, strlen(str));
	n = read(sockfd, buf, MAXLINE);
	printf("%d bytes from server:%s\n", n, buf);

	for(n=10; n>0; n--){
		cids[0] += n;
		cids[1] += n;
		write(sockfd, (char*)cids, 2*sizeof(int));

		memset(buf, 0, sizeof(char)*MAXLINE);
		s = read(sockfd, buf, MAXLINE);
		//intp = (int*)buf;
		//printf("%d bytes from server: %d, %d\n", s, *intp, *(intp+1) );
		printf("%d bytes from server: %s\n", s, buf);
		sleep(1);
	}
	
	close(sockfd);
	return 0;
}

