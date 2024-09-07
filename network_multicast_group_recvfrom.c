#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>



#include <string.h>
#include <strings.h>
#include <stdlib.h>
//network
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

typedef struct Next{
    struct sockaddr_in client_addr;
    int sockfd;
}Data;


static Data data = {0};//这里只定义了一个存放客户段的结构体，但是可以定义多个，
                      //  实现多个客户端同时通信，理论上用链表形式就可以实现无上限的客户端通信.需要在每次收到消息时遍历一次链表，存在就是之前发送给信息的client，不存在就是新链接的客户端，将客户端加入链表
                      //这样太浪费资源，，
                      //另一个思路，这里的recvfrom作为一个中转站，接受消息，做转发操作，也就是服务端，sendto端，这样应该能实现1对1私聊
                      //但是还是需要做链表，或者数据库存储客户端地址，但是可以做成客户端申请登录，发送登录请求，判断收到内容是否是登录请求的内容
                       //如果是就存储新地址（加入链表，或者存入数据库），如果不是就做其他任务，比如转发消息（可以设定为客户端先选择发送消息的对象，（对象就需要遍历链表了，但是这样避免了每次都遍历链表操作）然后才是内容），回复数据
                        //客户端通过switch，发送不同申请，服务端通过Switch，响应不同操作
void *fun_recvfrom(void *arg)
{
    Data *data = (Data *)arg;
    char buf[1024];
    int len = sizeof(data->client_addr);
    while(1)
    {
        bzero(buf, sizeof(buf));

        int ret = recvfrom(data->sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&data->client_addr, &len);
        if(ret < 0)
        {
            perror("recvfrom");
            exit(1);
        }
        printf("recvfrom: %s\n", buf);
        if(strncmp(buf, "exit", 4) == 0)
        {
            printf("exit\n");
            pthread_exit(0);
        }
    }
}

void *fun_sendto(void *arg)
{
    Data* data = (Data *)arg;
    char buf[1024];
    while(1)
    {
        bzero(buf, sizeof(buf));
        fgets(buf, sizeof(buf), stdin);
        sendto(data->sockfd, buf, strlen(buf), 0, (struct sockaddr *)&data->client_addr, sizeof(data->client_addr));
        if(strncmp(buf, "exit", 4) == 0)
        {
            printf("exit\n");
            pthread_exit(0);
        }
    }

}

int main()
{
    data.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(data.sockfd < 0)
    {
        perror("socket");
        exit(1);
    }
  //  setsockopt(data.sockfd, SOL_SOCKET, SO_REUSEADDR, NULL, 0);这句代码参数错误，正确为89,90行，这句改掉，逻辑没有错误了

    struct ip_mreq mreq;
    bzero(&mreq, sizeof(mreq));
    mreq.imr_multiaddr.s_addr = inet_addr("224.0.0.4");
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    setsockopt(data.sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

    int on = 1;
    setsockopt(data.sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        struct sockaddr_in rcvaddr = {
        .sin_family         = AF_INET,
        .sin_port           = htons(8888),
        .sin_addr.s_addr    = htonl(INADDR_ANY),
    };
    if (0 > bind(data.sockfd, (struct sockaddr*)&rcvaddr, sizeof(rcvaddr))) {
        perror("bind");
        return -1;
    }
    printf("bind success!\n");

    pthread_t tid_recvfrom, tid_sendto;
    pthread_create(&tid_recvfrom, NULL, fun_recvfrom, (void *)&data);
    pthread_create(&tid_sendto, NULL, fun_sendto, (void *)&data);
    pthread_join(tid_sendto, NULL);
    pthread_join(tid_recvfrom, NULL);
    close(data.sockfd);
    return 0;

}
