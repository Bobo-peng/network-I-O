#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include<errno.h>
#include<vector>
#include<string.h>

int main(int argc, char* argv[]){

int fd = socket(AF_INET, SOCK_STREAM, 0);
if(fd < 0){
	printf("create socket failed!, errno=%d\n",errno);
}
struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = htonl(INADDR_ANY);
addr.sin_port = htons(8080);

int ret;
ret = bind(fd, (struct sockaddr*) &addr, sizeof(addr));
if(ret == -1){
	printf("bind failed!, errno=%d",errno);
	close(fd);
}
ret = listen(fd, SOMAXCONN);
if(ret == -1){
	printf("listen failed!, errno=%d",errno);
}

struct sockaddr_in clientaddr;
socklen_t clientaddrlen = sizeof(clientaddr);
int maxfd = fd;
std::vector<int> afds;
//开始用select来管理多个连接
	while(true){
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET(fd, &fdset);
		for(int i = 0; i < afds.size(); ++i){
			if(afds[i] != -1)//INVALID_FD
			FD_SET(afds[i], &fdset);
		}
		timeval tm;
		tm.tv_sec = 1;
		tm.tv_usec = 0;
		ret = select(maxfd+1, &fdset, NULL, NULL, &tm);//监听可读事件
		if(ret < 0){
			printf("select error!\n");
			break;
		}
		else if(ret == 0){
			printf("timeout!\n");
			continue;//超时
		}
		else{
			if(FD_ISSET(fd, &fdset)){//fd上有事件
				int afd = accept(fd, (struct sockaddr*) &clientaddr, &clientaddrlen);
				printf("accept afd= %d\n",afd);
		        if(afd == -1){
					break;
			    }
				 printf("A client connected!,ip=%s,port=%d\n",inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
				 afds.push_back(afd);//将客户端连接的socket fd 加入到afds集合
				 maxfd = maxfd > afd ? maxfd : afd;
		   }
			else{

				for(int i = 0; i < afds.size(); ++i){
					if(afds[i] != -1 && FD_ISSET(afds[i], &fdset)){
						char buf[256] = {0};
						int len = recv(afds[i], buf, 256, 0);
					    if(len <= 0){
							printf("afd[%d] close or error\n",i);
							close(afds[i]);
						    afds[i] = -1;
						}
						printf("afd[%d] recv %d bytes:%s\n", i, len, buf);
					}
				}
			}
		
		
		
	}
	
}
	//关闭客户端所以连接
	for(int i = 0; i < afds.size(); ++i){
		if(afds[i] != -1)
		close(afds[i]);
		
	}
	close(fd);//关闭监听socket





}

