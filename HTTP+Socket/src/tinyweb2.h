#include "uv.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <memory.h>
#include <stddef.h> 
#include <malloc.h> 
#include <ctype.h> 
#include "membuf.h"


#if defined(WIN32)
	#define snprintf _snprintf
#endif

uv_tcp_t   _server;
uv_tcp_t   _client;
uv_loop_t*  _loop = NULL;
uv_tcp_t* handle;
uv_loop_t* loop;
int server_port = 8081;

void tinyweb_on_connection(uv_stream_t* server, int status);
void on_uv_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf);
void after_uv_close_client(uv_handle_t* client);
void write_uv_data(uv_stream_t* client, void* data, unsigned int len, int need_copy_data, int need_free_data);
void after_uv_write(uv_write_t* w, int status);
void tinyweb_close_client(uv_stream_t* client);

class CTcpServer
{
	public:
		static void check_sockname(struct sockaddr* addr, char* compare_ip, int compare_port, char* context);
		static void start_listener(uv_loop_t* loop, const char* ip, int port);
		static void ready_to_checkip(uv_stream_t* server, int status);
		static void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
		static char* format_http_respone(char* status, char* content_type, const void* content, int content_length, char* content_location) ;
		static void tinyweb_on_request_get(uv_stream_t* client, char* pathinfo, char* query_stirng) ;
};
