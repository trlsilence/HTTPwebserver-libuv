#include <iostream>  
#include <string.h>
#include <stdlib.h>  
#include <stddef.h>  
#include <malloc.h>  
#include "uv.h" 
//#include "../transport_proto/transport_proto.pb.h"
#include "../transport_proto/transport_proto.h" 

#define METHOD "GET"

struct casereturn
{
	int clienthandler;
	string senddata;
};

uv_loop_t* loop;  //event loop
int server_port = 7000;  //server port
uv_tcp_t tcpServer;    //tcpserver handler
uv_tcp_t* handle;   //event handler
float fversion = 1.0;   //default protocal version
int default_StateID = 100;  //default protocal StateID
int default_rsptime = 5;  //default protocal ResponseTime

using namespace std;


//
void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf);
void echo_write(uv_write_t *req_write, int status);
//一个新连接到达时的回调函数  
void on_connection(uv_stream_t* server, int status);
class CTcpServer
{
	public:
		static void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
		static void start_read();
		static void client_write(uv_stream_t *client,const string &senddata); //attention
		static int tcp_listener();
		//开启一个tcp监听  
		//static void GetCDNResponse(casereturn case,erialize_head obj, heads header, common comm, const string &headtemp,const string &bodytemp);
		//校验两个地址是否相同  
		static void check_sockname(struct sockaddr* addr, const char* compare_ip, int compare_port, const char* context);
		static casereturn Start_to_SerializeorParse(const string &stemp, int client);//attention
};
