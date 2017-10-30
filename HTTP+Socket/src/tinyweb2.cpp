#include "tinyweb2.h"
#include <iostream>

using namespace std;

void CTcpServer::check_sockname(struct sockaddr* addr, char* compare_ip, int compare_port, char* context) 
{  
	struct sockaddr_in check_addr = *(struct sockaddr_in*) addr;  
	char check_ip[17];  
	int r;  
	struct sockaddr_in compare_addr;
	uv_ip4_addr(compare_ip, compare_port,&compare_addr);  
	//网络字节序转换成主机字符序  
	uv_ip4_name(&check_addr, (char*)check_ip, sizeof check_ip);  
	//或者像下面这样获得ip地址  
	//char* check_ip = inet_ntoa(check_addr.sin_addr);  
	//std::cout<<context<<","<<check_ip<<","<<ntohs(check_addr.sin_port)<<endl;  
	printf("%s: %s:%d\n", context, check_ip, ntohs(check_addr.sin_port));
}

void after_uv_close_client(uv_handle_t* client) 
{
	membuf_uninit((membuf_t*)client->data); //see: tinyweb_on_connection()
	free(client->data); //membuf_t*: request buffer
	free(client);
}

void after_uv_write(uv_write_t* w, int status) 
{
	if(w->data)
		free(w->data); //copyed data
	uv_close((uv_handle_t*)w->handle, after_uv_close_client);
	free(w);
}

//close the connect of a client
void tinyweb_close_client(uv_stream_t* client) 
{
	uv_close((uv_handle_t*)client, after_uv_close_client);
}

//write data to client
//data: the data to be sent
//len:  the size of data, can be -1 if data is string
//need_copy_data: copy data or not
//need_free_data: free data or not, ignore this arg if need_copy_data is non-zero
//write_uv_data() will close client connection after writien success, because tinyweb use short connection.
void write_uv_data(uv_stream_t* client, void* data, unsigned int len, int need_copy_data, int need_free_data) 
{
	uv_buf_t buf;
	uv_write_t* w;
	void* newdata  = (void*)data;

	if(data == NULL || len == 0) return;
	if(len ==(unsigned int)-1)
		len = strlen((char*)data);

	if(need_copy_data) {
		newdata = malloc(len);
		memcpy(newdata, data, len);
	}

	buf = uv_buf_init((char*)newdata, len);
	w = (uv_write_t*)malloc(sizeof(uv_write_t));
	w->data = (need_copy_data || need_free_data) ? newdata : NULL;
	uv_write(w, client, &buf, 1, after_uv_write); //free w and w->data in after_uv_write()
}

//status: "302 Found"
//content_type: "text/html"
//content: any utf-8 data, need html-encode if content_type is "text/html"
//content_length: can be -1 if content is string
//returns malloc()ed respone, need free() by caller
char* CTcpServer::format_http_respone(char* status, char* content_type, const void* content, int content_length, char* content_location) 
{
	printf("调用格式化的函数！\n");
	int totalsize;
	char* respone;
	if(content_length < 0)
		content_length = content ? strlen((char*)content) : 0;
	totalsize = strlen(status) + strlen(content_type)+ content_length + 128;
	respone = (char*) malloc(totalsize);
	sprintf(respone, "HTTP/1.1 %s\r\n"
					 "Content-Type:%s;charset=utf-8\r\n"
					 "Content-Length:%d\r\n"
					 "Location:%s\r\n\r\n",
					 status, content_type, content_length,content_location);
	if(content) {
		memcpy(respone + strlen(respone), content, content_length);
	}
	return respone;
}

//invoked by tinyweb when GET request comes in
//please invoke write_uv_data() once and only once on every request, to send respone to client and close the connection.
//if not handle this request (by invoking write_uv_data()), you can close connection using tinyweb_close_client(client).
//pathinfo: "/" or "/book/view/1"
//query_stirng: the string after '?' in url, such as "id=0&value=123", maybe NULL or ""
void CTcpServer::tinyweb_on_request_get(uv_stream_t* client, char* pathinfo, char* query_stirng) {
	printf("正在调用request get....\n");
	if(strcmp(pathinfo, "/") == 0) {
		char* respone = format_http_respone("200 OK", "text/html","Welcome to tinyweb", -1,query_stirng);
		write_uv_data(client, respone, -1, 0, 1);
	} else if(strcmp(pathinfo, "/404") == 0) {
		char* respone = format_http_respone("404 Not Found", "text/html","<h3>404 Page Not Found<h3>", -1,query_stirng);
		write_uv_data(client, respone, -1, 0, 1);
	} else {
		printf("准备重定向...");
		char* respone;
		//char content[1024];
		//snprintf(content, sizeof(content), "<p>pathinfo: %s</p><p>query stirng: %s</p>", pathinfo, query_stirng);
		//respone = format_http_respone("200 OK", "text/html", content, -1,query_stirng);
		respone = format_http_respone("302 Found", "text/html","<h3>已经重定向到CDN<h3>", -1,  query_stirng);
		write_uv_data(client, respone, -1, 0, 1);
	}
	printf("数据返回完成！\n");
}

void CTcpServer::alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	*buf = uv_buf_init((char*)malloc(suggested_size), suggested_size);
}

void on_uv_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) 
{
	if(nread > 0) {
		//printf("client speak:%s\n",buf->base);
		char *pending_find_str = "HTTP";
       	if(strstr(buf->base,pending_find_str)!=NULL)
		{
			printf("****This is a HTTP user****");
		}
		char* crln2;
		membuf_t* membuf = (membuf_t*) client->data;   //see tinyweb_on_connection()
		assert(membuf);
		membuf_append_data(membuf, buf->base, nread);
		if((crln2 = strstr((char*)membuf->data, "\r\n\r\n")) != NULL) {
			char* request_header = membuf->data;
			*crln2 = '\0';
			printf("\n----Tinyweb client request: ----\n");
			//printf("\n----Tinyweb client request: ----\n%s\n", request_header);
			//handle GET request
			if(request_header[0]=='G' && request_header[1]=='E' && request_header[2]=='T') {
				
				char *query_stirng, *end, *temp;
				char* pathinfo = request_header + 3;
                                //自己添加的
				printf("****\n");
				temp = strtok(pathinfo, "\r\n"); 
				printf("****\n");
				temp=temp+1;
				temp=strtok(temp," ");//user URL
				printf("%s\n",temp);//输出分解的字符串
				printf("****\n");
				//自己添加结束
				while(isspace(*pathinfo)) pathinfo++;
				end = strchr(pathinfo, ' ');   //find the position '' first comes up
				printf("URL: %s",end);  
				if(end) *end = '\0';
				query_stirng = strchr(pathinfo, '?'); //split pathinfo and query_stirng using '?'
				if(query_stirng) {
					*query_stirng = '\0';
					query_stirng++;
				}
				query_stirng="https://www.baidu.com";
				CTcpServer::tinyweb_on_request_get(client, pathinfo, query_stirng);
				//write_uv_data -> after_uv_write -> uv_close, will close the client, so additional processing is not needed.
			} 
			else 
			{
				tinyweb_close_client(client);
			}
		}
	} else if(nread == -1) {
		tinyweb_close_client(client);
	}

	if(buf->base)
		free(buf->base);
}

void tinyweb_on_connection(uv_stream_t* server, int status)
{
	printf("enter on_new_connection...\n");
	assert(server == (uv_stream_t*)&_server);
	if(status == 0) 
	{
		struct sockaddr sockname, peername;  
		int namelen,r;  
		uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
		client->data = malloc(sizeof(membuf_t));
		membuf_init((membuf_t*)client->data, 128);
		uv_tcp_init(_loop, client);
		uv_accept((uv_stream_t*)&_server, (uv_stream_t*)client);
		uv_read_start((uv_stream_t*)client,CTcpServer::alloc_buffer, on_uv_read);
		//write_uv_data((uv_stream_t*)client, http_respone, -1, 0);
		//close client after uv_write, and free it in after_uv_close()
		namelen = sizeof sockname;  
		r = uv_tcp_getsockname(client, &sockname, &namelen);  
		CTcpServer::check_sockname(&sockname, "0.0.0.0", server_port, "server socket");  

		//有连接，可以获得目标的ip和端口  
		namelen = sizeof peername;  
		r = uv_tcp_getpeername(client, &peername, &namelen);  
		CTcpServer::check_sockname(&peername, "127.0.0.1", -1, "accepted socket peer");  

	} 

}

//start web server, linstening ip:port
//ip can be NULL or "", which means "0.0.0.0"
void CTcpServer::start_listener(uv_loop_t* loop, const char* ip, int port) {
	struct sockaddr sockname, peername;  
	int namelen;  
	_loop = loop;
	uv_tcp_init(_loop, &_server);
	struct sockaddr_in addr;
	uv_ip4_addr("0.0.0.0", port, &addr);    //(ip && ip[0]) ? ip :
	uv_tcp_bind(&_server, (const struct sockaddr*) &addr, 0);
	int r = uv_listen((uv_stream_t*)&_server, 8, tinyweb_on_connection);
	if (r)
	{  
	    	fprintf(stderr, "Listen error %s\n", uv_err_name(r));  
	}
	else
	{
		printf("listening......\n");
	}
	//sockname，获得监听自己的ip和端口  
	memset(&sockname, -1, sizeof sockname);  
	namelen = sizeof sockname;  
	r = uv_tcp_getsockname(&_server, &sockname, &namelen);  
	CTcpServer::check_sockname(&sockname, "0.0.0.0", 8081, "server socket");  
	//没有连接时，peername是无意义的  
	namelen = sizeof peername;  
	r = uv_tcp_getpeername(&_server, &peername, &namelen);  
	if (r == -1) {  
		printf("socket is not connected.\n");  
	}    
}

int main()
{
	CTcpServer::start_listener(uv_default_loop(), "0.0.0.0", 8081);
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}