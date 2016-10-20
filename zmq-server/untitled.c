#include <zmq.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <fstream> 
int g_is_exiting = false;

void write_to_file( char* buffer, long size, char *file_name );
int send();
void ParseArgs( int argc, char **argv);
//char * serverip="135.251.157.166";
char * serverip="tcp://*:8602";
std::string RunLength;
std::string goldfile;
std::string cfgfile;

/** * Read command line options */
void ParseArgs( int argc, char **argv)
{
   int   c;               // current option

   c = 0;
   while( argv[c]) c++;

   if( argc != c)
      printf("Error:%s argc %d disagrees with %d command line arguments"
             "--please lookup \"man -s2 exec\" (SPN 1/31/03)\n",
          __func__, argc, c);
  while( (c = getopt(argc, argv, "xfmtDIN:l:i:n:o:p:r:v:V:W:w:z:")) != EOF)
  {
    switch( c )
    {   
        case 'l':    RunLength = optarg;     break;
        case 'i':    cfgfile = optarg;       break;
        case 'm':    break;
        case 'V':    break;
        case 'v':    break;
        case 'I':    break;
        case 'o':    goldfile = optarg;      break;
        case 'r':    break;
        case 't':    break;
        case 'W':    break;
        case 'w':    break;  
        case '?':
        default:     printf("Error: not support this %s option\n", c);
           break;
    } // end switch (c)
  } // while ((c = getopt(argc, argv, "xfrml:i:o:v:")) != EOF)
}



int main(int argc, char *argv[])
{
    int res;

    printf("ltesim client starting... \n");
    ParseArgs(argc, argv); 

    send();

}

#include <zmq.h>
#include "stdio.h"


int send_file_by_zmq(void * pSock ,string file, char* buffer, long size )
{
    if(zmq_send(pSock, file.c_str(), file.size(), 0) < 0) {
        fprintf(stderr, "send file name message faild\n");
        return -1;
    }
    printf("send message : [%s] size=%d succeed\n", file.c_str(),file.size());

    if(zmq_send(pSock, buffer, size, 0) < 0)        {
        fprintf(stderr, "send file message faild\n");   
        return -1;
    }
    printf("send file : [%s] size=%d succeed\n", file.c_str(),size);

    return 0;

}
int send_command( void * pSock, string command_head, string command)
{

    string cmd =  command_head + " " +command;
    if(zmq_send(pSock, cmd.c_str(), cmd.size(), 0) < 0) {
        fprintf(stderr, "send commad %s message faild\n", cmd.c_str());
        return -1;
    }

    printf("send message : [%s],size [%d] succeed\n", cmd.c_str(),cmd.size());

}
int send_command( void * pSock, string cmd)
{
    if(zmq_send(pSock, cmd.c_str(), cmd.size(), 0) < 0) {
        fprintf(stderr, "send commad %s message faild\n", cmd.c_str());
        return -1;
    }

    printf("send message : [%s],size [%d] succeed\n", cmd.c_str(),cmd.size());

}

int send_file(void * pSock, string file)
{
    filebuf *pbuf;  
    ifstream filestr;  
    long size;  
    char * buffer; 

    filestr.open (file.c_str(), ios::binary);  
    pbuf=filestr.rdbuf();  

    size=pbuf->pubseekoff (0,ios::end,ios::in);  
    printf("file :%s,size is %ld \n",file.c_str(),size);
    pbuf->pubseekpos (0,ios::in);  

    buffer=new char[size];  

    pbuf->sgetn (buffer,size);  
    filestr.close();    


    send_file_by_zmq(pSock ,file,  buffer,  size );

}

int recev_file(void * pSock,char *mssg,size_t msize)
{
    zmq_msg_t msg;
    zmq_msg_init (&msg); 
    long msg_size=-1;

    char file_name[200] ;

    memset(file_name,0,200);
    strncpy (file_name , mssg, msize);
    printf("received message : %s\n", file_name);  

    //to receive file
    zmq_msg_init (&msg); 
    if(msg_size = zmq_msg_recv(&msg,pSock,0) < 0){
        printf("error = %s\n", zmq_strerror(errno));
        return -1;
    }

    size_t  file_size = zmq_msg_size(&msg);     
    char *file_mssg = (char *)zmq_msg_data(&msg);
    printf("received file size is : %d\n", file_size);
    msg_size = (long)file_size;
    write_to_file(mssg, msg_size,file_mssg ) ;

    zmq_msg_close (&msg);

    return 0;

}


int recev_command(void * pSock,char *mssg, size_t msize)
{

    printf("received command message : %s\n", mssg);

/* char *tmp; tmp = &mssg[3]; //right(tmp, mssg, 3); printf("received command_mssg : %s\n", mssg); if (mssg[1] == 'l') { RunLength = tmp; printf("-l : %s\n", RunLength.c_str()); } else if(mssg[1]== 'i'){ cfgfile = tmp; printf("-l : %s\n", cfgfile.c_str()); } else if(mssg[1]== 'o'){ goldfile = tmp; printf("-o : %s\n", goldfile.c_str()); } else{} */

}

void write_to_file( char* buffer, long size, char *file_name )
{

  filebuf *pbuf;  
  fstream filestr;  
  filestr.open (file_name,  std::ifstream::out);  
  if (! filestr.is_open())
  {
      std::cout << "Error opening file %s" << file_name;
      return;
  }


  pbuf=filestr.rdbuf();  


  printf("file :%s,size should be %ld \n",file_name,size);
  pbuf->sputn (buffer,size);  
  //pbuf->sputn ("my12",5); 


  size=pbuf->pubseekoff (0,ios::end,ios::in);  
  printf("file :%s,size is %ld \n",file_name,size);
  pbuf->pubseekpos (0,ios::in);  
  filestr.close();  
}


int send()
{
    void * pCtx = NULL;
    void * pSock = NULL;
    const char * pAddr = "tcp://135.251.157.166:7766";
    char *file_name ;

    if((pCtx = zmq_ctx_new()) == NULL) {
        return 0;
    }
    if((pSock = zmq_socket(pCtx, ZMQ_DEALER)) == NULL)    {
        zmq_ctx_destroy(pCtx);
        return 0;
    }
    int iSndTimeout = 5000;// millsecond
    if(zmq_setsockopt(pSock, ZMQ_RCVTIMEO, &iSndTimeout, sizeof(iSndTimeout)) < 0)    {
        zmq_close(pSock);
        zmq_ctx_destroy(pCtx);
        return 0;
    }
    if(zmq_connect(pSock, pAddr) < 0)    {
        zmq_close(pSock);
        zmq_ctx_destroy(pCtx);
        return 0;
    }


    send_file(pSock ,cfgfile );
    send_file(pSock ,goldfile);   
    send_command(pSock,"-i my.cfg -l 165 -o my.gold");//cfgfile);


    while(1)
    {
        static int i = 0;
        char szMsg[1024] = {0};
        snprintf(szMsg, sizeof(szMsg), "hello world : %3d", i++);
        printf("Enter to send...\n");


        errno = 0;
        zmq_msg_t msg;
        zmq_msg_init (&msg); 
        long msg_size=-1;
        if(msg_size = zmq_msg_recv(&msg,pSock,0) < 0)           
        {
            printf("error = %s\n", zmq_strerror(errno));
            continue;
        }

        size_t  msize = zmq_msg_size(&msg);     
        char *mssg = (char *)zmq_msg_data(&msg);
        //if(msize < 200){
        // printf("received size=%d message : %s\n", msize,mssg );
        //}

        if(mssg[0] ==  '-')   {
            if(recev_command(pSock,mssg, msize)<0) continue;    
        }   

        else if( msize < 200 &&  msize > 2) {
            file_name = new char[msize];
            strncpy (file_name , mssg, msize);

            printf("wait to recevie file: %s\n",file_name );

        } 
        else{
            if(file_name != NULL){

                printf("received file : %s\n", file_name); 
                write_to_file(mssg, msize,file_name ) ;
            }
            file_name = NULL;
        }

        // Release message 
        zmq_msg_close (&msg);

        getchar();
    }

    return 0;
}

//Client 端代码：
#include <zmq.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#include <fstream> 
int g_is_exiting = false;

void write_to_file( char* buffer, long size, char *file_name );
int recev(); 
void ParseArgs( int argc, char **argv);
//char * serverip="135.251.157.166";
char * serverip="tcp://*:8602";
std::string RunLength;
std::string goldfile="xxx";
std::string cfgfile;

/** * Read command line options */
void ParseArgs( int argc, char **argv)
{
   int   c;               // current option

   c = 0;
   while( argv[c]) c++;

   if( argc != c)
      printf("Error:%s argc %d disagrees with %d command line arguments"
             "--please lookup \"man -s2 exec\" (SPN 1/31/03)\n",
          __func__, argc, c);
  while( (c = getopt(argc, argv, "xfmtDIN:l:i:n:o:p:r:v:V:W:w:z:")) != EOF)
  {
    switch( c )
    {   
        case 'l':    RunLength = optarg;     break;
        case 'i':    cfgfile = optarg;       break;
        case 'm':    break;
        case 'V':    break;
        case 'v':    break;
        case 'I':    break;
        case 'o':    goldfile = optarg;      break;
        case 'r':    break;
        case 't':    break;
        case 'W':    break;
        case 'w':    break;  
        case '?':
        default:     printf("Error: not support this %s option\n", c);
           break;
    } // end switch (c)
  } // while ((c = getopt(argc, argv, "xfrml:i:o:v:")) != EOF)
}



int main(int argc, char *argv[])
{
    int res;

    printf("ltesim client starting... \n");
    ParseArgs(argc, argv); 

    recev(); 
   // void *m_zmqctx = zmq_ctx_new();
   // zmq_ctx_set(m_zmqctx, ZMQ_MAX_SOCKETS, 128);

   // printf("ltesim client starting zmq... \n");
   // BaseZmqClient * zmq_clinet = new BaseZmqClient();
   // zmq_clinet->SetContext(m_zmqctx);
   // zmq_clinet->Start(serverip, ZMQ_ROUTER);

   // while(1){};

   // 
   // zmq_term(m_zmqctx);
   // zmq_ctx_destroy(m_zmqctx);


}

#include <zmq.h>
#include "stdio.h"

int send_file_by_zmq(void * pSock ,string file, char* buffer, long size )
{
    if(zmq_send(pSock, file.c_str(), file.size(), 0) < 0) {
        fprintf(stderr, "send file name message faild\n");
        return -1;
    }
    printf("send message : [%s] size=%d succeed\n", file.c_str(),file.size());

    if(zmq_send(pSock, buffer, size, 0) < 0)        {
        fprintf(stderr, "send file message faild\n");   
        return -1;
    }
    printf("send file : [%s] size=%d succeed\n", file.c_str(),size);

    return 0;

}
int send_command( void * pSock, string command_head, string command)
{

    string cmd =  command_head + " " +command;
    if(zmq_send(pSock, cmd.c_str(), cmd.size(), 0) < 0) {
        fprintf(stderr, "send commad %s message faild\n", cmd.c_str());
        return -1;
    }

    printf("send message : [%s],size [%d] succeed\n", cmd.c_str(),cmd.size());

}
int send_command( void * pSock, string cmd)
{
    if(zmq_send(pSock, cmd.c_str(), cmd.size(), 0) < 0) {
        fprintf(stderr, "send commad %s message faild\n", cmd.c_str());
        return -1;
    }

    printf("send message : [%s],size [%d] succeed\n", cmd.c_str(),cmd.size());

}

int send_file(void * pSock, string file)
{
    filebuf *pbuf;  
    ifstream filestr;  
    long size;  
    char * buffer; 

    filestr.open (file.c_str(), ios::binary);  
    pbuf=filestr.rdbuf();  

    size=pbuf->pubseekoff (0,ios::end,ios::in);  
    printf("file :%s,size is %ld \n",file.c_str(),size);
    pbuf->pubseekpos (0,ios::in);  

    buffer=new char[size];  

    pbuf->sgetn (buffer,size);  
    filestr.close();    


    send_file_by_zmq(pSock ,file,  buffer,  size );

}



int recev_file(void * pSock,char *mssg,size_t msize)
{
    zmq_msg_t msg;
    zmq_msg_init (&msg); 
    long msg_size=-1;

    char file_name[200] ;

    memset(file_name,0,200);
    strncpy (file_name , mssg, msize);
    printf("received message : %s\n", file_name);  

    //to receive file
    zmq_msg_init (&msg); 
    if(msg_size = zmq_msg_recv(&msg,pSock,0) < 0){
        printf("error = %s\n", zmq_strerror(errno));
        return -1;
    }

    size_t  file_size = zmq_msg_size(&msg);     
    char *file_mssg = (char *)zmq_msg_data(&msg);
    printf("received file size is : %d\n", file_size);
    msg_size = (long)file_size;
    write_to_file(mssg, msg_size,file_mssg ) ;

    zmq_msg_close (&msg);

    return 0;

}


int recev_command(void * pSock,char *mssg, size_t msize)
{

    printf("received command message : %s\n", mssg);

/* char *tmp; tmp = &mssg[3]; //right(tmp, mssg, 3); printf("received command_mssg : %s\n", mssg); if (mssg[1] == 'l') { RunLength = tmp; printf("-l : %s\n", RunLength.c_str()); } else if(mssg[1]== 'i'){ cfgfile = tmp; printf("-l : %s\n", cfgfile.c_str()); } else if(mssg[1]== 'o'){ goldfile = tmp; printf("-o : %s\n", goldfile.c_str()); } else{} */

}

void write_to_file( char* buffer, long size, char *file_name )
{

  filebuf *pbuf;  
  fstream filestr;  
  filestr.open (file_name,  std::ifstream::out);  
  if (! filestr.is_open())
  {
      std::cout << "Error opening file %s" << file_name;
      return;
  }


  pbuf=filestr.rdbuf();  


  printf("file :%s,size should be %ld \n",file_name,size);
  pbuf->sputn (buffer,size);  
  //pbuf->sputn ("my12",5); 


  size=pbuf->pubseekoff (0,ios::end,ios::in);  
  printf("file :%s,size is %ld \n",file_name,size);
  pbuf->pubseekpos (0,ios::in);  
  filestr.close();  
}

int recev() 
{
    void * pCtx = NULL;
    void * pSock = NULL;
    const char * pAddr = "tcp://*:7766";

    if((pCtx = zmq_ctx_new()) == NULL)
    {
        return 0;
    }
    if((pSock = zmq_socket(pCtx, ZMQ_DEALER)) == NULL)
    {
        zmq_ctx_destroy(pCtx);
        return 0;
    }
    int iRcvTimeout = 5000;// millsecond
    if(zmq_setsockopt(pSock, ZMQ_RCVTIMEO, &iRcvTimeout, sizeof(iRcvTimeout)) < 0)
    {
        zmq_close(pSock);
        zmq_ctx_destroy(pCtx);
        return 0;
    }
    if(zmq_bind(pSock, pAddr) < 0)
    {
        zmq_close(pSock);
        zmq_ctx_destroy(pCtx);
        return 0;
    }
    printf("bind at : %s\n", pAddr);

    char *file_name ;
    while(1)
    {
        errno = 0;
        zmq_msg_t msg;
        zmq_msg_init (&msg); 
        long msg_size=-1;
        if(msg_size = zmq_msg_recv(&msg,pSock,0) < 0)           
        {
            printf("error = %s\n", zmq_strerror(errno));
            continue;
        }

        size_t  msize = zmq_msg_size(&msg);     
        char *mssg = (char *)zmq_msg_data(&msg);
        //if(msize < 200){
        // printf("received size=%d message : %s\n", msize,mssg );
        //}

        if(mssg[0] ==  '-')   {
            if(recev_command(pSock,mssg, msize)<0) continue;    
        }   

        else if( msize < 200 &&  msize > 2) {
            file_name = new char[msize];
            strncpy (file_name , mssg, msize);

            printf("wait to recevie file: %s\n",file_name );

        } 
        else{
            if(file_name != NULL){

                printf("received file : %s\n", file_name); 
                write_to_file(mssg, msize,file_name ) ;

                send_file(pSock ,file_name );
            }
            file_name = NULL;
        }

        // Release message 
        zmq_msg_close (&msg);

    }

    return 0;
}