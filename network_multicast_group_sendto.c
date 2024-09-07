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

typedef struct { // 向线程传递参数
    int sockfd;
    struct sockaddr_in addr;
}Data;

void* fun_sendto(void *arg)
{
    Data* data = (Data *)arg;
    char buf[1024];
    int len = sizeof(data->addr);
    while (1)
    {
        bzero(buf, sizeof(buf));
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf) - 1] = '\0';
        sendto(data->sockfd, buf, strlen(buf), 0, (struct sockaddr *)&(data->addr), len);
        if (strcmp(buf, "exit") == 0)
        {
            pthread_exit(NULL);
        }
    }
}

void* fun_recvfrom(void *arg)
{
    Data* data = (Data*)arg;
    char buf[1024];
    int len = sizeof(data->addr);
    while (1)
    {
        bzero(buf, sizeof(buf));
        recvfrom(data->sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&(data->addr), &len);
        if (strcmp(buf, "exit") == 0)
        {
            printf("exit\n");
            pthread_exit(NULL);
        }
        printf("recvfrom: %s\n", buf);
    }
}

int main(int argc, char *argv[])
{
    Data data;
    data.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (data.sockfd < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("224.0.0.4");
    data.addr = addr;

    pthread_t tid_sendto,tid_recvfrom;
    pthread_create(&tid_sendto, NULL, fun_sendto, (void *)&data);
    pthread_create(&tid_recvfrom, NULL, fun_recvfrom, (void *)&data);

    pthread_join(tid_sendto, NULL);
    pthread_join(tid_recvfrom, NULL);
    close(data.sockfd);
    return 0;
}
