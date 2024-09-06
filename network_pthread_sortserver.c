/*===============================================
 *   文件名称：server.c
 *   创 建 者：     
 *   创建日期：2024年07月17日
 *   描    述：
 ================================================*/
#include "head.h"
#include <signal.h>
#include <sys/wait.h>

//void handler(int sig)
//{
     
//}
///////
void *func(void *arg)//这个函数叫线程启动例程
{
    int connfd = *(int*)arg;
    char buf[1024];
    while (1) {
        memset(buf, 0, sizeof(buf));
        int ret = recv(connfd, buf, sizeof(buf), 0);
        if (ret < 0) {
            perror("recv");
            break;
        } else if (0 == ret) {
            printf("client close!\n");
            break;
        }else
            printf("Recv: %s\n",buf);
        if (strcmp(buf, "quit") == 0)
            break;
    }
    close(connfd);
    // pthread_exit(NULL);
    return NULL;//线程启动例程返回 ,main函数中使用了pthread_detach,所以这里不需要pthread_exit(NULL);return后自动回收线程
}

////
int main(int argc, char *argv[])
{
   // signal(SIGTERM, handler);

    //1.创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }
    printf("socket success!\n");

    //2.绑定本机地址和端口
    IPaddr_t srvaddr = {
        .sin_family         = AF_INET,  //IPv4
        .sin_port           = htons(7778), //port
        .sin_addr.s_addr    = htonl(INADDR_ANY), //0.0.0.0
        //.sin_addr.s_addr    = inet_addr("192.168.7.135")
    };
    if (0 > bind(sockfd, (SA*)&srvaddr, sizeof(srvaddr))) {
        perror("bind");
        return -1;
    }
    printf("bind success!\n");

    //3.设置监听套接字
    if (0 > listen(sockfd, 5)) {
        perror("listen");
        return -1;
    }
    printf("listen success\n");

    pthread_t thread;
    int ret;
    char buf[1024];
    while (1) {
        //4.接收客户端的连接，并生成通信套接字
        int connfd = accept(sockfd,NULL,NULL);
        if (connfd < 0) {
            perror("accept");
            return -1;
        }
        printf("accept success\n");

        ////
        int ret = pthread_create(&thread,NULL,func,(void *)&connfd);
        if(0 != ret)
        {
            printf("%s\n",strerror(ret));
            return -1;
        }
        pthread_detach(thread);
        ////
    }


//6.关闭套接字
close(sockfd);

return 0;
} 

