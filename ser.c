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

void recv_file(int c,char *name)
{

	int fd = open(name,O_WRONLY|O_CREAT,0600);//先打开文件，再接收大小
	if(fd == -1)
	{
		send(c,"err",3,0);
		return;
	}
	send(c,"ok",2,0);
	char elem[64] = {0};
	if(recv(c,elem,63,0)<=0)
    {
	   return;
	}
	
   // if(strncmp(elem,"ok#",3)!=0)
   // {
     //  printf("error:%s\n",elem+3);
    //}
    int size = 0;
	//sscanf(elem+3,"%d",&size);
	//printf("str size=%s,size=%d\n",elem+3,size);
	sscanf(elem,"%d",&size);
	printf("str size=%s,size=%d\n",elem,size);
	
	char data_size[512] = {0};
    int num = 0;
    int curr_size = 0;
    while((num = recv(c,data_size,512,0))>0)
    {
		 write(fd,data_size,num);
		 curr_size+=num;
		 float f = curr_size*100.0/size;
         printf("up:%.2f%%\r",f);
	     fflush(stdout);
         if(curr_size==size)
	     {
             break;
	     }
	}
	printf("\nup finished!\n");
	close(fd);
	
}
int socket_create();
void* thread_fun(void * arg);
int main()
{
    int sockfd = socket_create();

    struct sockaddr_in caddr;
    while( 1 )
    {
        int len = sizeof(caddr);
        int c = accept(sockfd,(struct sockaddr*)&caddr,&len);
        if ( c < 0 )
        {
            continue;
        }

        printf("accept c = %d\n",c);

        pthread_t id;
        pthread_create(&id,NULL,thread_fun,(void*)c);
    }

}

void* thread_fun(void * arg)
{
    int c = (int)arg;
    while( 1 )
    {
        char buff[128] = {0};
        int n = recv(c,buff,127,0);//rm a.c 
        if ( n <= 0 )
        {
            break;
        }

        //strtok_t  -> rm, a.c -> myargv[],
        //1) get -> send_file()
        //2) put -> recv_file()
        //3) rm, cp , ls ->  fork
        // parent -> read(pipefd[0]);
        // child -> dup2(pipefd[1],1)
        //          dup2(pipefd[2],2)
        //exec(ls);
        //exit()
        char * myargv[10]={0};
        char *ptr=NULL;
        char *s=strtok_r(buff," ",&ptr);
        if(s==NULL)
        {
            //send(c,"ok#",3,0);//文件从ok#开始发送
            continue;
        }
        int i=1;
        myargv[0]=s;
        while((s=strtok_r(NULL," ",&ptr))!=NULL)
        {
            myargv[i++]=s;
        }
        if(strcmp(myargv[0],"get")==0)
        {
            int fd=open(myargv[1],O_RDONLY);
            if(fd==-1)
            {
                send(c,"err",3,0);
                continue;
            }
            int size=lseek(fd,0,SEEK_END);
            lseek(fd,0,SEEK_SET);
            char file_size[64]={"ok#"};
            sprintf(file_size+3,"%d",size);
            send(c,file_size,strlen(file_size),0);
            if(size==0)
            {
                continue;
            }
            char statu[32]={0};
            if(recv(c,statu,31,0)<=0)
            {
                break;
            }
            if(strcmp(statu,"ok")!=0)
            {
                continue;
            }
           // sendfile(c,fd,NULL,size);//零拷贝
            char data[512]={0};
            int num=0;
            while((num=read(fd,data,512))>0)
            {
                send(c,data,num,0);
            }
            
            close(fd);
        }
        else if(strcmp(myargv[0],"put")==0)
        {
            
			recv_file(c,myargv[1]);
			
		}
        else
        {
            int pipefd[2];
            pipe(pipefd);
            pid_t pid=fork();
            assert(pid!=-1);
            if(pid==0)
            {
                dup2(pipefd[1],1);
                dup2(pipefd[1],2);
                execvp(myargv[0],myargv);
                perror("execvp error!\n");
                exit(0);
            }
            close(pipefd[1]);
            wait(NULL);
            char readbuff[1024]={"ok#"};
            int pipe_num=read(pipefd[0],readbuff+3,1020);
            send(c,readbuff,pipe_num+3,0);
            close(pipefd[0]);
        }
        //printf("buff=%s\n",buff);
        //send(c,"ok",2,0);

    }

    close(c);
    printf("one client over\n");

}

int socket_create()
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if ( sockfd == -1 )
    {
        return -1;
    }

    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(SER_PORT);
    saddr.sin_addr.s_addr = inet_addr(SER_IP);

    int res = bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
    if ( res == -1 )
    {
        return -1;
    }

    listen(sockfd,5);

    return sockfd;
}

