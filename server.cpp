#include "zhelpers.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#define NDEBUG
#define backend_host "tcp://localhost:5679"

char* process(char* Dpic);
int main() {
	void *context = zmq_ctx_new();
	void *worker = zmq_socket(context, ZMQ_REQ);
	char *identity = "WORKER1";
    zmq_setsockopt (worker, ZMQ_IDENTITY, identity, strlen(identity));
	zmq_connect(worker, backend_host);
	s_send(worker, "READY");
	printf("ready\n");
	while(1){
		char *identity = s_recv(worker);
		char *empty = s_recv(worker);
		assert(*empty == 0);
		free(empty);
		
		//char *Dpic = s_recv(worker);
		char *Dpic;
    	int size = zmq_recv (worker, Dpic, 10000000, 0);
        Dpic[size] = '\0';
		char *Recog_Result = process(Dpic);
		s_sendmore(worker, identity);
		s_sendmore(worker, "");
		//s_send(worker, Recog_Result);
		zmq_send(worker, Recog_Result, size, 0);
		free(Dpic);
		free(identity);
		free(Recog_Result);
		printf("ready\n");
	}

	zmq_close(worker);
	zmq_ctx_destroy(context);
}


char* process(char* Dpic){
	return Dpic;	
}