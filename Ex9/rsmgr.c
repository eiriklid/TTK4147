#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <pthread.h>

#include "fifo.h"

dispatch_t              *dpp;
resmgr_attr_t           resmgr_attr;
dispatch_context_t      *ctp;
resmgr_connect_funcs_t  connect_funcs;
resmgr_io_funcs_t       io_funcs;
iofunc_attr_t           io_attr;

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb);

int io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);

char* global_buf = "Hello World!\n";
pthread_mutex_t buf_mutex;

int counter = 0;
fifo_t queue;
void error(char *s)
{
	perror(s);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	printf("Start resource manager\n");
	pthread_mutex_init(&buf_mutex, NULL);
	// create dispatch.
	if (!(dpp = dispatch_create()))
		error("Create dispatch");

	init_fifo(&queue);
	// initialize resource manager attributes.
	memset(&resmgr_attr, 0, sizeof(resmgr_attr));
	resmgr_attr.nparts_max = 1;
	resmgr_attr.msg_max_size = 2048;

	// set standard connect and io functions.
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);
	iofunc_attr_init(&io_attr, S_IFNAM | 0666, 0, 0);

	// override functions
	io_funcs.read = io_read;
	io_funcs.write = io_write;

	// establish resource manager
	if (resmgr_attach(dpp, &resmgr_attr, "/dev/myresource", _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &io_attr) < 0)
		error("Resmanager attach");
	ctp = dispatch_context_alloc(dpp);

	// wait forever, handling messages.
	while(1)
	{
		if (!(ctp = dispatch_block(ctp)))
			error("Dispatch block");
		dispatch_handler(ctp);
	}

	exit(EXIT_SUCCESS);
}

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb){
	 //printf("io_read\n");
	 char *buf[255];
	 int status;
	 int nonblock;
	 pthread_mutex_lock(&queue.resource_mutex);
	 if ((status = iofunc_read_verify (ctp, msg, ocb,&nonblock)) != EOK){
	     return (status);
	 }

	 if(!fifo_status(&queue)){
		 if (nonblock){

			 return (EAGAIN);
		 }
		 else{
			 printf("Blocked %i\n",ctp->rcvid);
			 fifo_add_blocked_id(&queue,ctp->rcvid);
			 return(_RESMGR_NOREPLY);
		 }
	 }


	 fifo_print(&queue);
	 fifo_rem_string(&queue, buf);
	 //fifo_print(&queue);
	 pthread_mutex_unlock(&queue.resource_mutex);


	if(!ocb->offset)
	{
		// set data to return
		SETIOV(ctp->iov, buf, strlen(buf));
		_IO_SET_READ_NBYTES(ctp, strlen(buf));

		// increase the offset (new reads will not get the same data)
		ocb->offset = 1;

		// return
		return (_RESMGR_NPARTS(1));
	}
	else
	{
		// set to return no data
		_IO_SET_READ_NBYTES(ctp, 0);

		// return
		return (_RESMGR_NPARTS(0));
	}
}

int io_write (resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb){
    int status;
    char buf[255];

    if((status = iofunc_write_verify(ctp, msg, ocb, NULL)) != EOK)
        return (status);

    if((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
        return(ENOSYS);

    /* set up the number of bytes (returned by client's write()) */

    _IO_SET_WRITE_NBYTES (ctp, msg->i.nbytes);

   //buf = (char *) malloc(msg->i.nbytes + 1);
    if (buf == NULL)
        return(ENOMEM);

    /*
     *  Reread the data from the sender's message buffer.
     *  We're not assuming that all of the data fit into the
     *  resource manager library's receive buffer.
     */

    resmgr_msgread(ctp, buf, msg->i.nbytes, sizeof(msg->i));
    buf [msg->i.nbytes] = '\0'; /* just in case the text is not NULL terminated */
    printf ("Received %d bytes = '%s'\n", msg -> i.nbytes, buf);

    pthread_mutex_lock(&queue.resource_mutex);
    if ((queue.blocked_id[queue.blockedHead]!=0)){
    	printf("BUF: %s\n", buf);
    	int status = MsgReply(fifo_rem_blocked_id(&queue), 0, buf, sizeof(buf));
    	printf("STATUS: %d\n", status);
   	}else{
   		fifo_add_string(&queue, buf);
   		fifo_print(&queue);
   	}
    pthread_mutex_unlock(&queue.resource_mutex);



    //free(buf);

    if (msg->i.nbytes > 0)
        ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

    return (_RESMGR_NPARTS (0));
}
