#include <czmq.h>

int recev_command(void * pSock,char *mssg, size_t msize)
{

    printf("received command message : %s\n", mssg);

/* char *tmp; tmp = &mssg[3]; //right(tmp, mssg, 3); printf("received command_mssg : %s\n", mssg); if (mssg[1] == 'l') { RunLength = tmp; printf("-l : %s\n", RunLength.c_str()); } else if(mssg[1]== 'i'){ cfgfile = tmp; printf("-l : %s\n", cfgfile.c_str()); } else if(mssg[1]== 'o'){ goldfile = tmp; printf("-o : %s\n", goldfile.c_str()); } else{} */

}

void write_to_file( char* buffer, long size, char *file_name )
{


  printf("file :%s,size should be %ld \n",file_name,size);
 
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
