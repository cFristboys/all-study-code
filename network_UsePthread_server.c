#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void* fun_w(void * arg){
    int *connfd = (int *)arg;
    char buf[1024];
    while(1){
        memset(buf,0,sizeof(buf));
        fgets(buf,sizeof(buf),stdin);
        buf[strlen(buf)-1] = '\0';
        if(send(*connfd,buf,sizeof(buf),0) < 0){
            perror("send:");
            break;
        }
        if(strncmp(buf,"quit",4) == 0){
            break;
        }
    }
    pthread_exit(0);
}

void* fun_r(void * arg){
    int *connfd = (int *)arg;
    char buf[1024];
    int ret;
    while(1){
        memset(buf,0,sizeof(buf));
        if((ret = recv(*connfd,buf,sizeof(buf),0)) < 0){
            perror("send:");
            break;
        }
        printf("client say:%s\n",buf);
        if(ret == 0){
            printf("client close\n");
            break;
        }
    }
    pthread_exit(0);
}

int main()
{//创建
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        perror("socket:");
        return -1;
    }
    printf("socket ok\n");
    //绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = inet_addr("192.168.112.123");
    if(bind(sockfd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
    {
        perror("bind:");
        return -1;
    }
    printf("bind ok\n");
    //监听
    if(listen(sockfd,12) < 0)
    {
        perror("listen");
        return -1;
    }
    printf("listen ok\n");
    //连接
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int connfd = accept(sockfd,(struct sockaddr*)&client_addr,&len);
    if (connfd < 0)
    {
        perror("accept:");
        return -1;
    }
    printf("accept ok\n");
    printf("client ip:%s\n",inet_ntoa(client_addr.sin_addr));  
    //读写
    pthread_t tid_w,tid_r;
    pthread_create(&tid_w,NULL,fun_w,(void *)&connfd);
    pthread_create(&tid_r,NULL,fun_r,(void *)&connfd);

    pthread_join(tid_w,NULL);
    pthread_join(tid_r,NULL);

    close(connfd);
    //关闭
    return 0;
}
