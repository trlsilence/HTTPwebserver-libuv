#include "uv.h"
#include "transport_proto.h"
#include <unistd.h> 
#include "timer.h"
#include <stdlib.h>
#include <vector>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <memory.h>
#include <stddef.h> 
#include <malloc.h> 
#include <ctype.h> 
#include <algorithm>
#include "membuf.h"
#include <iostream>
#include "ClientIDtohandler.h"
#include "IPMapClient.h"

using namespace std;

#define METHOD "GET"

#if defined(WIN32)
	#define snprintf _snprintf
#endif

vector <uv_stream_t*> client_temp;

struct casereturn
{
	int clienthandler;
	int ManagerID;
	string userid;
	string cdnid;
	string senddata;
};

uv_tcp_t   _server;
uv_tcp_t   _client;
uv_loop_t*  _loop = NULL;
uv_tcp_t* handle;
uv_loop_t* loop;
uint64_t repeat = 0;  

//int server_port = 80;  //server port
string fversion = "1";   //default protocal version
string default_StateID = "100";  //default protocal StateID
string default_rsptime = "5";  //default protocal ResponseTime
string cmdzero = "0";
string identity = "2";
int result1 = 1;

int calculate = 1; //统计数据发送失败的次数

const int case1 = 1;
const int case3 = 3;
const int case4 = 4;
const int case5 = 5;
const int case6 = 6;
const int case7 = 7;
const int case8 = 8;
const int case9 = 9;
const int case10 = 10;
const int case11 = 11;
const int case12 = 12;
const int case13 = 13;

void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
void tinyweb_on_connection(uv_stream_t* server, int status);
void on_uv_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
void on_read_http(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
void on_read_client(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf);
void echo_write(uv_write_t *req_write, int status);
void wating_to_find(char* pathinfo, char* useraddress);
void after_uv_close_client(uv_handle_t* client);
void write_uv_data(uv_stream_t* client, void* data, unsigned int len, int need_copy_data, int need_free_data);
void after_uv_write(uv_write_t* w, int status);
void tinyweb_close_client(uv_stream_t* client);

class CTcpServer
{
	public:
		static char* check_sockname(struct sockaddr* addr, char* compare_ip, int compare_port, char* context);
		static void start_listener(uv_loop_t* loop, const char* ip, int port);
		static int judge_number(char* str);
		static void ready_to_checkip(uv_stream_t* server, int status);
		static void client_write(uv_stream_t *client,const string &senddata,int cishu);
		static casereturn Start_to_SerializeorParseManager(const char* stemp, int client);//attention
		static casereturn Start_to_SerializeorParseCDN(const char* stemp, const string &client);//attention
		static string Serial_head_userurl(const string &userurl, const string &userid);
		static string construct_cmdfive(const string &domain, const string enip, int service, const string &userid, int livetime, int money, int size);
		static char* format_http_respone(char* status, char* content_type, const void* content, int content_length, char* content_location);
		static void tinyweb_on_request_get(uv_stream_t* client, char* pathinfo, char* query_stirng,const void* textmessage);
};
