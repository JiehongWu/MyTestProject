#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>

#define  SER_PORT   6000
#define  SER_IP     "127.0.0.1"
void recv_file(int sockfd,char* name)
{
    char buff[64]={0};
    if(recv(sockfd,buff,63,0)<=0)
    {
        return;
    }
    if(strncmp(buff,"ok#",3)!=0)
    {
        printf("Error:%s\n",buff+3);
		return;//打印完需要退出
    }
    int size=0;
    sscanf(buff+3,"%d",&size);
    printf("str size:=%s,size=%d\n",buff+3,size);
    int fd=open(name,O_WRONLY|O_CREAT,0600);
    if(fd==-1)
    {
        send(sockfd,"err",3,0);
        return;
    }
    send(sockfd,"ok",2,0);
    char data_size[512]={0};
    int num=0;
    int curr_size=0;
    while((num=recv(sockfd,data_size,511,0))>0)
    {
        write(fd,data_size,num);
        curr_size+=num;
        float f=curr_size*100.0/size;
        printf("dowm:%.2f%%\r",f);
        fflush(stdout);
        if(curr_size == size)
        {
            break;
        }
    }
    printf("\ndown finished!\n");
    close(fd);
}
int main()
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    assert ( sockfd != -1 );

    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SER_PORT);
    saddr.sin_addr.s_addr = inet_addr(SER_IP);

    int res = connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
    assert( res != -1 );

    while( 1 )
    {
        char buff[128] = {0};
        printf("Connect success>> ");
        fflush(stdout);
        
        fgets(buff,128,stdin);// ls , rm a.c,  cp  a.txt  b.txt ,get a.c  , put name
        buff[strlen(buff)-1]=0;
        char cmd[128]={0};
        strcpy(cmd,buff);
        char *s=strtok(cmd," ");
        if(s==NULL)
        {
            continue;
        }
        if ( strcmp(s,"exit") == 0 )
        {
            break;
        }
        else if(strcmp(s,"get")==0)//下载
        {
            s=strtok(NULL," ");
            if(s==NULL)
            {
                continue;
            } 
            send(sockfd,buff,strlen(buff),0);
            recv_file(sockfd,s);
        }
        else if(strcmp(s,"put")==0)//上传
        {
			send(sockfd,buff,strlen(buff),0);//没有给服务器发送 put XX
			s = strtok(NULL," ");
			if(s == NULL)
			{
				continue;
			}
			int fd = open(s,O_RDONLY);
			if(fd == -1)
			{
                send(sockfd,"err",3,0);
				continue;
			}
			
			int size = lseek(fd,0,SEEK_END);
			lseek(fd,0,SEEK_SET);
            char filesize[64] = {0};
			sprintf(filesize,"%d",size);
			send(sockfd,filesize,strlen(filesize),0);
			if(size == 0)
			{
               continue;
			}
			char statu[32]={0};//服务器打开文件成功后send的 ok 没有接收
            if(recv(sockfd,statu,31,0) <= 0)
            {
                continue;
            }
            if(strcmp(statu,"ok")!=0)
            {
                continue;
            }
			////////////////////////////////////////////
			
		    char elem[512] = {0};
			int number = 0;
			while((number = read(fd,elem,512))>0)
			{
                send(sockfd,elem,number,0);
			}
			close(fd);
		}
        else  //其他命令
        {
            send(sockfd,buff,strlen(buff),0);
			recv(sockfd,buff,127,0);
			printf("%s\n",buff+3);
       
        }
    }
    close(sockfd);
}

