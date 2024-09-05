#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>



#include <string.h>
#include <strings.h>
#include <stdlib.h>
//network
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main()
{   
    //创建
    int sockfd  = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }
    //绑定
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    server_addr.sin_addr.s_addr = inet_addr("192.168.242.128");
    socklen_t len = sizeof(server_addr);
    if (bind(sockfd,(struct sockaddr*)&server_addr,len) < 0)
    {
        perror("bind");
        exit(1);
    }
    //监听
    if (listen(sockfd,10) < 0)
    {
        perror("listen");
        exit(1);
    }   
    printf("listen\n");
    //select 检测
    fd_set fds,rfds;
    FD_ZERO(&fds);
    FD_SET(sockfd,&fds);
    int maxfd = sockfd;

    char buf[1024];
    int ret,re,i;
    while (1)
    {
        rfds = fds;
        ret = select(maxfd+1,&rfds,NULL,NULL,NULL);//最后一个参数位NULL表示阻塞,只有事件发生才会往下执行,
        //也就是返回值大于0,出错返回-1,(超时返回0),但是阻塞状态不可能返回0
        if (ret < 0)
        {
            perror("select");
            exit(1);
        }
        // if(ret == 0)
        // {
        //     continue;
        // }
        
        //发生事件
        for(i = 0;i <= maxfd;i++)
        {
            if(FD_ISSET(i,&rfds))
            {
                if (i == sockfd)
                {
                   int newfd = accept(sockfd,NULL,NULL);
                        if (newfd < 0)
                        {
                            perror("accept");
                            exit(1);
                        }
                    FD_SET(newfd,&fds);
                    maxfd = maxfd > newfd ? maxfd : newfd;
                    printf("newfd:%d\n",newfd);
                }else{
                    re = recv(i,buf,sizeof(buf),0);
                    if (re < 0)
                    {
                        perror("read");
                        exit(1);
                    }
                    if (re == 0)
                    {
                        printf("client close\n");
                        close(i);
                        FD_CLR(i,&fds);
                    }else{
                        printf("client say:%s\n",buf);
                    }
                }     
            }
        }
    }
    close(sockfd);
    return 0;
}
