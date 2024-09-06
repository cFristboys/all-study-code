#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <poll.h>



#include <string.h>
#include <strings.h>
#include <stdlib.h>
//network
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main()
{
    int sockfd = socket(AF_INEF,SOCK_STREAM,0);
    if(sockfd < 0)
    {
        perror("socket");
        return -1;
    }
    printf("socket success\n");
    struct sockaddr_in seraddr;
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(9990);
    seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(0 > bind(sockfd,(struct sockaddr*)&seraddr,sizeof(seraddr)))
    {
        perror("bind");
        return -1;
    }
    printf("bind success\n");
    if(0 > listen(sockfd,12))
    {
        perror("listen");
        return -1;
    }

    //poll
    int n = 2;
    struct pollfd fds[1024];
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN;

    struct timeval tv = {2};
    char buf[1024];
    int ret,i,ret1,j;

    while(1)
    {
        ret = poll(fds,n,0);
        if(0 > ret)
        {
            perror("select");
            return -1;
        }else if(ret = 0)
        {
            printf("timeout....");
            continue;
        }else
        {
            for(i = 0;i < n;i++)
            {
                if(fds[i].revents & POLLIN){
                    memset(buf,0,sizeof(buf));
                    if(fds[i].fd == 0)
                    {
                        read(0,buf,sizeof(buf));
                        printf("stdin:%s\n",buf);
                    }else if(sockfd == fds[i].fd)
                    {
                        int connfd = accept(sockfd,NULL,NULL);
                        if(0 > connfd)
                        {
                            perror("accept");
                            return -1;
                        }
                        printf("accept success\n");
                        
                        for(j = 0;j < n;j++)
                            if(-1 == fds[j].fd)
                                break;
                        if(j== 1024)
                        {
                            printf("too many client\n");
                            close(connfd);
                            continue;
                        }
                        fds[j].fd = connfd;
                        fds[j].events = POLLIN;
                        if(j == n)
                        {
                            n++;
                        }
                        
                    }else
                    {
                        ret1 = recv(fds[i].fd,buf,sizeof(buf),0);
                        if(ret1 < 0)
                        {
                            perror("recv");
                            return -1;
                        }else if(ret1 == 0)
                        {
                            printf("client close\n");
                            close(fds[i].fd);
                            fds[i].fd = -1;
                        }else
                        {
                            printf("recv : %s\n",buf);
                        }
                    }
                }
            }
        }
    }

    close(sockfd);

    return 0;

}
