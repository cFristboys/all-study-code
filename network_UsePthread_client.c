#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

void* fun_r(void *arg){
    int * sockfd = (int *)arg;
    char buf[1024];
    int ret;
    while(1){
        memset(buf,0,sizeof(buf));
        ret = recv(*sockfd,buf,sizeof(buf),0);
        if(ret<0){
            perror("recv");
            break ;
        }
        printf("server:%s\n",buf);
        if (ret == 0)
        {
            printf("server close\n");
            break;
        }
        
    }
    pthread_exit(0);
}

void* fun_w(void *arg){
    int *sockfd = (int *)arg;
    char buf[1024];
    while(1){
        memset(buf,0,sizeof(buf));
        fgets(buf,sizeof(buf),stdin);
        buf[strlen(buf)-1] = '\0';
        if(send(*sockfd,buf,sizeof(buf),0) < 0){
            perror("send:");
            break ;
        }
        if(strncmp(buf,"quit",4) == 0){
            break;
        }
    }
    pthread_exit(0);
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    server_addr.sin_addr.s_addr = inet_addr("192.168.112.123");

    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("connect");
        return -1;
    }
    pthread_t tid_r, tid_w;
    pthread_create(&tid_r, NULL, fun_r, (void *)&sockfd);
    pthread_create(&tid_w, NULL, fun_w, (void *)&sockfd);
    pthread_join(tid_r, NULL);
    pthread_join(tid_w, NULL);
    close(sockfd);
    return 0;

}
