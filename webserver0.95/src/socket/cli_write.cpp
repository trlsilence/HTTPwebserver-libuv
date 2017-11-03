#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <stdlib.h>  
#include "uv.h"  
#include <cstring>  
  
uv_loop_t *loop;  
uv_tcp_t *client;  
  
void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);   
void do_read(); 
void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf) ;    
void do_write();  
void echo_write(uv_write_t *req, int status);   
void on_connection(uv_connect_t* req, int status);
  
/*alloc_buffer：这是为接收数据而分配buffer的函数*/  
void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	//printf("alloc buffer\n");  
	*buf = uv_buf_init((char*) malloc(suggested_size), suggested_size);  
}  
/*do_read：uv_read_start准备接收数据*/   
void do_read()  
{  	
       // printf("start reading the data\n");
	int r = uv_read_start((uv_stream_t*) client, alloc_buffer, echo_read);    
	if(r<0)  
	{  
		printf("Received data is null!");
		fprintf(stderr, "Read error %s\n", uv_err_name(r));  
		uv_close((uv_handle_t*)client, NULL);  
	}
	if(r==0)
	{
		//printf("successfully received the data!\n");
    		//do_write();
	}
}

/*echo_read：打印输出接收到的数据*/
void echo_read(uv_stream_t *client , ssize_t nread, const uv_buf_t* buf)  
{  
	//printf("调用echo_read\n"); 
	if (nread < 0) {  
		if (nread == UV_EOF)
		{  
			printf("server has closed");
		    fprintf(stderr, "UV_EOF....\n");  
		}else
		{  
		    fprintf(stderr, "Read error %s : %s\n", uv_err_name(nread), uv_strerror(nread));  
		}  
		uv_close((uv_handle_t*) client, NULL);  
		return;  
	}else if(nread>2){  
		printf("server speak:%s\n", buf->base);
		do_write();  
	}  
}
    
/*do_wrirte：向服务器发送数据*/  
void do_write()  
{  
	//printf("start to send the data\n"); 
	uv_write_t *req_write = (uv_write_t *) malloc(sizeof(uv_write_t));    
	char *line = (char*)malloc(1024*sizeof(char));  
	memset(line, 0, 1024);  // Memset 用来对一段内存空间全部设置为某个字符，一般用在对定义的字符串进行初始化为‘ ’或‘/0’；
	int len = 0;  
	printf("client speak:");
	while (gets(line))  
	{  
		for(; len<1024; len++)  
		{  
		  if(line[len] == 0)  
		      break;  
		}  
		break;  
	}  
	uv_buf_t *buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));  
	uv_buf_init(line, len+1);  
	buf->base = line;  
	buf->len = len+1;  
	req_write->data = (void*) buf->base;  
	int r = uv_write(req_write, (uv_stream_t*)client, buf, 1, echo_write); 
	if(r<0)  
	{  
		//printf("发送失败！");
		fprintf(stderr, "write error %s\n", uv_err_name(r));  
		uv_close((uv_handle_t*)client, NULL);  
	}else if(r==0){
		//printf("发送成功！");
		do_read();  //再次调用数据读取函数
	} 
}

  
/*写数据后释放资源*/  
void echo_write(uv_write_t *req, int status)  
{  
	//printf("调用echo_write\n"); 
	if (status < 0) 
	{  
		fprintf(stderr, "Write error %s\n", uv_err_name(status));  
	}  
	char *base = (char*) req->data;
	free(req);  
	free(base);   
}  
  
/*监听时的回调函数*/  
void on_connection(uv_connect_t* req, int status)
{
	printf("on_connection\n");   
	if (status < 0) 
	{  
		// error!  
		fprintf(stderr, "connect error %s : %s\n", uv_err_name(status), uv_strerror(status));  
		return;  
	}else if(status==0)
	{
		printf("successfully connected!\n");
	}  
	free(req);    
	do_write(); 
 }  
  
int main() 
{
	printf("the client is running...\n");  
	loop = uv_default_loop();  
	client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));  
	uv_tcp_init(loop, client);  
	struct sockaddr_in bind_addr;  
	uv_ip4_addr("127.0.0.1", 8081, &bind_addr);  //地址与socket绑定
	uv_connect_t *req = (uv_connect_t *)malloc(sizeof(uv_connect_t));  
	int r = uv_tcp_connect(req, client, (const sockaddr*)&bind_addr, on_connection);  
	if (r) 
	{  
		fprintf(stderr, "connect error %s\n", uv_err_name(r));   
		return 1;  
	}
	return uv_run(loop, UV_RUN_DEFAULT);    
} 
