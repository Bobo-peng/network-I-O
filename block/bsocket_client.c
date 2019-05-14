#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include <stdlib.h>
#include<errno.h>
#include<string.h>

int main(int argc, char* argv[]){
	int *p = NULL;
	*p = 0;

if(argc != 3){
	printf("please input IP port\n");
}
char* ip = argv[1];
unsigned int port = atoi(argv[2]);
int fd = socket(AF_INET, SOCK_STREAM, 0);
if(fd < 0){
	printf("create socket failed!, errno=%d\n",errno);
}
struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = inet_addr(ip);
addr.sin_port = htons(port);

int ret;
ret = connect(fd, (struct sockaddr*) &addr, sizeof(addr));
if(ret == -1){
	printf("connect failed!, errno=%d",errno);
}
else{
	char *buf = "hello, I am a client!";
	ret = send(fd, buf, strlen(buf), 0);
	if(ret > 0){
		printf("send %d byte:%s\n",ret,buf);
	}
	else if(ret == -1){
		printf("send error!,errno=%d\n",errno);
	}
	else{
		printf("server disconnect!\n");
	}
}
	
}
