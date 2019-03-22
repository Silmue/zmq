#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "zhelpers.h"
#define frontend_host "tcp://localhost:5678"

void sendPic(char* pic_path, char* frontend);
int main(void)
{
    printf("start");
    sendPic("./test.png", frontend_host);
    return 0;
}

char * ReadFile(char *pathpic, int &pic_len){
    //将图片读取出来
    FILE *fp = fopen(pathpic, "rb");     //打开文件
    fseek(fp, 0, SEEK_END);  //一直寻找到文件尾部
    pic_len = ftell(fp);  //得到图片的长度
    rewind(fp);  //rewind将文件指针指向开头
    char *pic_buf = new char[pic_len + 1];  //开辟一个空间在堆上
    memset(pic_buf, 0, pic_len + 1);  //清空文件指针
    //读取文件内容
    fread(pic_buf,sizeof(char),pic_len,fp);
    pic_buf[pic_len] = '\0';
    fclose(fp);
    return pic_buf;
}

// char * ReadFile(char *pathpic, int &pic_len){
//     vector<uchar> pic_encode;
//     cv::Mat pic = cv::imread(pathpic);
//     pic_encode = cv::imencode(".png", pic, pic_encode);
//     pic_len = pic_encode.size();
//     return pic_encode.data();
// }


void savePic(char* pathpic, char* pic, int size){
    FILE *fp = fopen(pathpic, "wb");
    fwrite(pic, sizeof(char), size, fp);
    fclose(fp);
}


void sendPic(char* pic_path, char* frontend) {
	int Dpic_len;
	char *Dpic_data = NULL;
	void *context = zmq_ctx_new();
	void *client = zmq_socket(context, ZMQ_REQ);

	Dpic_data = ReadFile(pic_path, Dpic_len);
	printf("length=%d\n", Dpic_len);
    
	char *identity = "CLIENT1";
    zmq_setsockopt (client, ZMQ_IDENTITY, identity, strlen(identity));	
    zmq_connect(client, frontend);
    zmq_send (client, Dpic_data, Dpic_len, 0);
    //s_send(client, Dpic_data);
    printf("sent, waiting for feedback\n");
	//char *reply = s_recv(client);
    char *reply;
    int size = zmq_recv(client, reply, 10000000, 0);
    reply[size] = '\0';
    printf("received\n");
    //printf(reply);
    savePic("./recv1.png", reply, size);
	free(reply);
	zmq_close(client);
	zmq_ctx_destroy(context);
}
