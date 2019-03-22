#include "zhelpers.h"
#define NDEBUG
#define DEQUEUE(q) memmove(&(q)[0], &(q)[1], sizeof(q) - sizeof(q[0]))
#define frontend_host "tcp://*:5678"
#define backend_host "tcp://*:5679"

int main(void){
    void *context = zmq_ctx_new();
    void *frontend = zmq_socket(context, ZMQ_ROUTER);
    void *backend = zmq_socket(context, ZMQ_ROUTER);
    int available_workers = 0;
    char *worker_queue[10];

    zmq_bind(frontend, frontend_host);
    zmq_bind(backend, backend_host);
    printf("start\n");
    while(1){
        zmq_pollitem_t items[] = {
            {backend, 0, ZMQ_POLLIN, 0},
            {frontend, 0, ZMQ_POLLIN, 0}
        };
        int rc = zmq_poll(items, available_workers ? 2:1, -1);
        if (rc == -1)
            break;
        

        if (items[0].revents & ZMQ_POLLIN){
            char *worker_id = s_recv(backend);
            worker_queue[available_workers++] = worker_id;
            printf("Worker %s ready\n", worker_queue[0]);

            char* empty = s_recv(backend);
            assert(empty[0] == 0);
            free(empty);

            char* client_id = s_recv(backend);
            
            if (strcmp(client_id, "READY") != 0){
                empty = s_recv(backend);
                assert(empty[0] == 0);
                free(empty);
                char *reply = s_recv(backend);
                s_sendmore(frontend, client_id);
                s_sendmore(frontend, "");
                s_send(frontend, reply);
                free(reply);
            }
            free(client_id);
        }
        
        if (items[1].revents & ZMQ_POLLIN){
            char *client_id = s_recv(frontend);
            char *empty = s_recv(frontend);
            assert(empty[0] == 0);
            free(empty);
            //char *request = s_recv(frontend);
            char *request;
            int size = zmq_recv (frontend, request, 256, 0);
            printf("received request from %s\n", client_id);
            s_sendmore(backend, worker_queue[0]);
            s_sendmore(backend, "");
            s_sendmore(backend, client_id);
            s_sendmore(backend, "");
            s_send(backend, request);
            //zmq_send (backend, request, size, 0);
            printf("sent request to %s\n", worker_queue[0]);
            free(client_id);
            free(request);
            free(worker_queue[0]);
            DEQUEUE(worker_queue);
            available_workers--;
        }
    }    
    zmq_close(frontend);
    zmq_close(backend);
    zmq_ctx_destroy(context);
}