//  Hello World server

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdlib.h> 
#include <time.h>
#include <sys/stat.h>

void init_daemon()  
{  
    int pid;  
    int i;  
      
    if(pid=fork())  
    exit(0);//是父进程，结束父进程  
    else if(pid< 0)  
    exit(1);//fork失败，退出  
    //是第一子进程，后台继续执行  
  
    setsid();//第一子进程成为新的会话组长和进程组长  
    //并与控制终端分离  
    if(pid=fork())  
    exit(0);//是第一子进程，结束第一子进程  
    else if(pid< 0)  
    exit(1);//fork失败，退出  
    //是第二子进程，继续  
    //第二子进程不再是会话组长  
  
    for(i=0;i< NOFILE;++i)//关闭打开的文件描述符  
    close(i);  
    //chdir("/tmp");//改变工作目录到/tmp  
    umask(0);//重设文件创建掩模  
      
    return;  
} 

struct send_info
{
char filename[255]; //发送者ID
char filecontent[1024000];
};

int write_file(char * dir,char * filename,char * filecontent){

    int out = -1;  

    //以只写方式创建文件，如果文件不存在就创建一个新的文件  
    //文件属主具有读和写的权限  
    char full_path[255];
    memset(full_path,0,255);
    strcat(full_path,dir);
    strcat(full_path,"//");
    strcat(full_path,filename);
    strcat(full_path,".log");

    out = open(full_path, O_WRONLY|O_CREAT, 0777);  

    write(out, filecontent, strlen(filecontent));//写一个字节的数据  
  
    //关闭文件描述符  
    close(out);  
    return 0; 
}

int main (void)
{
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:9999");
    assert (rc == 0);
    printf("listening in tcp://*:9999\n");
    while (1) {
        /* Create an empty ØMQ message */
        zmq_msg_t msg;
        int rc = zmq_msg_init (&msg);
        assert (rc == 0);
        /* Block until a message is available to be received from socket */
        rc = zmq_msg_recv (&msg, responder, 0);
        assert (rc != -1);

        int size = zmq_msg_size(&msg);
        printf("size = %d\n",size);

        char datetime[200];
        time_t now;
        struct tm *tm_now;

        time(&now);
        tm_now = localtime(&now);
        strftime(datetime,200,"%Y-%m-%d",tm_now);
        //printf("now datetime : %s\n",datetime);

        if(access(datetime,0)){
            printf("can't found mkdir %s\n",datetime);
            mkdir(datetime,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }else{
            printf("the datetime dir already exists\n");
        }
        struct send_info * data;
        data  = zmq_msg_data(&msg);
        printf("filename = %s\n",data->filename );
        //printf("content %s\n",data->filecontent );
        write_file(datetime,data->filename ,data->filecontent);


        /* Release message */ 
        zmq_msg_close (&msg);

        zmq_send (responder, "World", 5, 0);
    }
    return 0;
}
