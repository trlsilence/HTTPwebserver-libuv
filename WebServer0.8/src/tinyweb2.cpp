#include "tinyweb2.h"
#include <iostream>


using namespace std;
using namespace transport_proto;

/**Get ip address**/
void CTcpServer::check_sockname(struct sockaddr* addr, char* compare_ip, int compare_port, char* context)  //used by HTTP and General Clients
{  
	struct sockaddr_in check_addr = *(struct sockaddr_in*) addr;  
	char check_ip[17];  
	struct sockaddr_in compare_addr;
	uv_ip4_addr(compare_ip, compare_port,&compare_addr);  
	//网络字节序转换成主机字符序  
	uv_ip4_name(&check_addr, (char*)check_ip, sizeof check_ip);  
	//或者像下面这样获得ip地址  
	//char* check_ip = inet_ntoa(check_addr.sin_addr);  
	//std::cout<<context<<","<<check_ip<<","<<ntohs(check_addr.sin_port)<<endl;  
	printf("%s: %s:%d\n", context, check_ip, ntohs(check_addr.sin_port));
}

void after_uv_close_client(uv_handle_t* client)  //used by HTTP
{
	membuf_uninit((membuf_t*)client->data); //see: tinyweb_on_connection()
	free(client->data); //membuf_t*: request buffer
	free(client);
}

void after_uv_write(uv_write_t* w, int status)   //used by HTTP
{
	if(w->data)
		free(w->data); //copyed data
	uv_close((uv_handle_t*)w->handle, after_uv_close_client);
	free(w);
}

//close the connect of a client
void tinyweb_close_client(uv_stream_t* client)   //used by HTTP
{
	uv_close((uv_handle_t*)client, after_uv_close_client);
}

//write data to client
//data: the data to be sent
//len:  the size of data, can be -1 if data is string
//need_copy_data: copy data or not
//need_free_data: free data or not, ignore this arg if need_copy_data is non-zero
//write_uv_data() will close client connection after writien success, because tinyweb use short connection.
void write_uv_data(uv_stream_t* client, void* data, unsigned int len, int need_copy_data, int need_free_data) //HTTP调用的发送数据的函数
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

void echo_write(uv_write_t *req_write, int status)  
{  
	//printf("调用echo_write\n"); 
	if (status < 0) 
	{  
		fprintf(stderr, "Write error %s\n", uv_err_name(status));  
	}  
	char *base = (char*) req_write->data; 
    free(req_write);  
	//free(base); 
}

void CTcpServer::client_write(uv_stream_t *client,const string &senddata)  //send data to clients!
{
	/*准备写入数据*/
	uv_write_t *req_write = (uv_write_t *) malloc(sizeof(uv_write_t)); 
	cout<<"server speak:";
	req_write->data = (void*)(senddata.data()); 
	/*buf 初始化*/
	uv_buf_t *buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	uv_buf_init((char*)(senddata.data()), senddata.length());
	buf->base = (char*)(senddata.data());  
	buf->len = senddata.length();
	req_write->data = (void*) buf->base; 

	int r2 = uv_write(req_write, client, buf, 1, echo_write); 
	if(r2==0)
	{
	    printf(" Server successfully senddata to client!\n");
	}
	/*写入数据结束*/
} 
/***used to serialize header and requested url to manager client***/
string CTcpServer::Serial_head_userurl(const string &userurl, const string &userid)   //userid type is wrong! correct type is string!
{
	Serialize_head obj_Serialheader;
	string str1=obj_Serialheader.Serialhead(obj_Serialheader, fversion, 0, 0, default_StateID, default_rsptime);
	Cbody_GetCDN getcdn;
	string str2 = getcdn.Serial_GetCDN(getcdn, userurl, userid);
	str1.append(str2);
	cout<<"Successfully Serialized!"<<endl;
	return str1;
}

void on_read_http(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
	char* crln2;
		membuf_t* membuf = (membuf_t*) client->data;   //see tinyweb_on_connection()
		assert(membuf);
		membuf_append_data(membuf, buf->base, nread);
		if((crln2 = strstr((char*)membuf->data, "\r\n\r\n")) != NULL) 
		{
			char* request_header = membuf->data;
			*crln2 = '\0';
			printf("\n----Tinyweb client request: ----\n");
			//printf("\n----Tinyweb client request: ----\n%s\n", request_header);
			//handle GET request
			if(request_header[0]=='G' && request_header[1]=='E' && request_header[2]=='T') 
			{
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
				UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
				int clienthandler=1;  //需要指定的Manager的ID
				uv_stream_t *sendclient;
				sendclient=usertohandler->GetUserHandler(clienthandler);
				string str=temp;
				string pending_send_str;
				string userid = "202.202.43.125";
				pending_send_str = CTcpServer::Serial_head_userurl(str, userid);  // 1 is 
				CTcpServer::client_write(sendclient, pending_send_str);
				//自己添加结束
				while(isspace(*pathinfo)) pathinfo++;
				end = strchr(pathinfo, ' ');   //find the position '' first comes up
				printf("URL: %s",end);  
				if(end) *end = '\0';
				query_stirng = strchr(pathinfo, '?'); //split pathinfo and query_stirng using '?'
				if(query_stirng) 
				{
					*query_stirng = '\0';
					query_stirng++;
				}
				//这里添加从CDN处获取到的用于重定向的链接（需要添加函数），对应于命令字11

				query_stirng="https://www.baidu.com";
				CTcpServer::tinyweb_on_request_get(client, pathinfo, query_stirng);
				//write_uv_data -> after_uv_write -> uv_close, will close the client, so additional processing is not needed.
			} 
			else 
			{
				tinyweb_close_client(client);
			}
		}
}

void on_read_client(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf) 
{
        //printf("数据读取！\n");  
	if (nread < 0) 
	{  
	    if (nread == UV_EOF)
	    {
	    	cout<<client<<"client has closed!"<<endl;
			fprintf(stderr, "UV_EOF....\n");  
	    }else
	    {  
			fprintf(stderr, "Read error %s : %s\n", uv_err_name(nread), uv_strerror(nread));  
	    }  
	    uv_close((uv_handle_t*) client, NULL);  
	    return;  
	}
	if(nread>0)
	{
		const char *pending_find_str = "Hello";
		if(strstr(buf->base,pending_find_str)!=NULL||strstr(buf->base,"hello")!=NULL)
		{
			cout<<"client speak(未序列化):"<<buf->base<<endl;
			cout<<"这是后端的心跳包！"<<endl;
		}
		else
		{
			printf("现在处理的客户端句柄是：%d \n",client);
			/*反序列化收到的数据*/
			struct casereturn caseresult;
			UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
			struct StateandID state_id;
			cout<<"client speak(未序列化):"<<buf->base<<endl;
			state_id = usertohandler->FindMapValue(client);
			if (state_id.result==1)
			{	
				cout<<"客户端ID存在，无需再添加了!"<<endl;;
			}
			else
			{
				state_id.client_id = usertohandler->UserIDtoHandlerManager::AddIDtoHandler(client);
				cout<<"No finding client id!"<<endl;
			}
			caseresult.clienthandler=1;
			//printf("nread:%d\n",nread);  //打印出收到的字符数
			//scaseresult=CTcpServer::Start_to_Parse(buf->base, int client);
			uv_stream_t *sendclient;
			sendclient=usertohandler->GetUserHandler(caseresult.clienthandler);
			//string senddata="Hi~~~~~~";
		   	//CTcpServer::client_write(sendclient, senddata);
			/*
			printf("查找后的客户端句柄是：%d,准备发送数据 \n",sendclient);
			//实际读取到了内容，准备发送数据 
		   	CTcpServer::client_write(sendclient, caseresult.senddata);	*/
		}
	}	
	
} 

void on_uv_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) 
{
	//judge the received data is correct or wrong?
	if (nread < 0) 
	{  
	    if (nread == UV_EOF)
	    {
	    	cout<<client<<"client has closed!"<<endl;
			fprintf(stderr, "UV_EOF....\n");  
	    }else
	    {  
			fprintf(stderr, "Read error %s : %s\n", uv_err_name(nread), uv_strerror(nread));  
	    }  
	    uv_close((uv_handle_t*) client, NULL);  
	    return;  
	}

	// received the correct data
	if(nread > 0) 
	{
		//printf("client speak:%s\n",buf->base);
		char *pending_find_str = "HTTP";
       	if(strstr(buf->base,pending_find_str)!=NULL)
		{
			printf("****This is a HTTP user****");
			on_read_http(client, nread, buf);
		}
		else
		{
			printf("****This is Manager or CDN node*****\n");
			on_read_client(client, nread, buf);
		}

		
	} else if(nread == -1) {
		tinyweb_close_client(client);
	}

	if(buf->base)
		free(buf->base);
}

casereturn Deail_CaseOne(const string &stemp, int client)  //处理命令字1
{
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp; //用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)

	Cbody_GetCDNRsp  obj_getcdnrsp;  //

	bodytemp = obj_getcdnrsp.Parse_GetCDNRsp(obj_getcdnrsp, stemp);
	headstruct.cmdtype = 2;
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, 7, headstruct.cmdtype, default_StateID, default_rsptime);
	Cbody_AssignCDN obj_assigncdn;
	temp_body = obj_assigncdn.Serial_AssignCDN(obj_assigncdn, METHOD, bodytemp.url, client);  //client用于标识最初的请求是由谁发出的,写入到body中
	temp_head.append(temp_body);
	casedeail.clienthandler = bodytemp.cdnid;
	casedeail.senddata = temp_head;
	return casedeail;
}

casereturn Deail_CaseThree(const string &stemp, int client)  //处理命令字1
{
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)

	Cbody_AssignCDNRsp obj_assigncdnrsp;
	bodytemp = obj_assigncdnrsp.Parse_AssignCDNRsp(obj_assigncdnrsp,stemp);
	headstruct.cmdtype = 4;
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, 7, headstruct.cmdtype, default_StateID, default_rsptime);
	Cbody_AssignCDNRep obj_assigncdnrep;
	//userid is CDNurl ; url is the url requested by the user;
	temp_body = obj_assigncdnrep.Serial_AssignCDNRep(obj_assigncdnrep, client, bodytemp.url);
	temp_head.append(temp_body);
    casedeail.clienthandler=bodytemp.managerid;
	casedeail.senddata=temp_head;
	return casedeail; 
}

casereturn Deail_CaseSeven(const string &stemp, int client)  //cmdtype 7
{	
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)

	Cbody_DelFile obj_delfile;
	//bodytemp = obj_delfile.parse_DelFile(obj_delfile,stemp);
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, 7, headstruct.cmdtype, default_StateID, default_rsptime);
	bodytemp = obj_delfile.Parse_DelFile(obj_delfile, stemp);
	temp_body = obj_delfile.Serial_DelFile(obj_delfile, client, bodytemp.url);
	temp_head.append(temp_body);
	casedeail.clienthandler=bodytemp.cdnid;
	casedeail.senddata=temp_head;
	return casedeail;

}

casereturn Deail_CaseEight(const string &stemp, int client)
{
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)

	Cbody_AddFile obj_addfile;
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, 7, headstruct.cmdtype, default_StateID, default_rsptime);
	bodytemp = obj_addfile.Parse_AddFile(obj_addfile, stemp);
	temp_body = obj_addfile.Serial_AddFile(obj_addfile, client, bodytemp.url);
	temp_head.append(temp_body); 
	casedeail.clienthandler=bodytemp.cdnid;
	casedeail.senddata=temp_head;
	return casedeail;
}

casereturn Deail_CaseNine(const string &stemp, int client)
{
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)

	struct StateInfo statedetail;
	Cbody_StateInfo obj_stateinfo;
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, 7, headstruct.cmdtype, default_StateID, default_rsptime);
	statedetail = obj_stateinfo.Parse_StateInfo(obj_stateinfo, stemp);
	temp_body = obj_stateinfo.Serial_StateInfo(obj_stateinfo, statedetail.m_ullMaxBand, statedetail.m_ullUseBand,
	statedetail.m_ullDCDNBand, statedetail.m_ullMaxSpace, statedetail.m_ullUseSpace, statedetail.m_uiHealthDegree, 
	statedetail.m_iLastKeepAliveTime,statedetail.managerid, statedetail.cdnid);  // ot use managerid , to find the true handler
	temp_head.append(temp_body);
	casedeail.clienthandler=statedetail.managerid;
	casedeail.senddata=temp_head;
	return casedeail;
}

casereturn Deail_CaseTen(const string &stemp, int client)
{
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)

	Cbody_FileInfo obj_fileinfo;
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, 7, headstruct.cmdtype, default_StateID, default_rsptime);
	bodytemp = obj_fileinfo.Parse_FileInfo(obj_fileinfo, stemp);
	temp_body= obj_fileinfo.Serial_FileInfo(obj_fileinfo, bodytemp.managerid, bodytemp.url, bodytemp.cdnid);
	temp_head.append(temp_body);
	casedeail.clienthandler=bodytemp.cdnid;
	casedeail.senddata=temp_head;
	return casedeail;
}

casereturn Deail_CaseEleven(const string &stemp, int client)   //提取URL和UserID,返回给用户进行重定向
{
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)

	cout<<"needs to send respose to the user!"<<endl;
    Cbody_GetCDNRsp  obj_getcdnrsp1;  //
	bodytemp = obj_getcdnrsp1.Parse_GetCDNRsp(obj_getcdnrsp1,stemp);
	//bodytemp can be used to send to users!
	casedeail.senddata = bodytemp.url;
	casedeail.userid = bodytemp.userid;  //这里是有问题的，应该改为string型
	return casedeail;
}

casereturn Deail_CaseTwelve(const string &stemp, int client) //前端需要反序列化，提取URL和UserID,返回给用户进行重定向
{
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	Cbody_CDNFindSourceResult findresult;

	cout<<"needs to send respose to the user!"<<endl;
	bodytemp=findresult.Parse_CDNFindSourceResult(findresult, stemp);
	//bodytemp can be used to send to users!
	casedeail.senddata = bodytemp.url;
	casedeail.userid = bodytemp.userid; 
	return casedeail;
}

casereturn Deail_CaseThirting(const string &stemp, int client) //此时。stemp作为首部的数据，直接发送
{
    string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
    int managerid = 1; //
	cout<<"needs to send managerid to CDN!"<<endl;
    Cbody_ReturnManagerIDtoCDN report_ip; //
	//需要添加查找ManagerID的函数,这里需要添加查找ManagerID的函数，暂时写为1
	//bodytemp can be used to send to users!
	temp_body = report_ip.Serial_ReturnManagerIDtoCDN(report_ip, managerid, client);
	temp_body.append(stemp);
	casedeail.senddata = stemp; 
	casedeail.clienthandler = client;
	return casedeail;
}

casereturn CTcpServer::Start_to_SerializeorParse(const string &stemp, int client)
{
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;   
	struct common bodytemp;
	headstruct = obj_Serialize.Parsehead(obj_Serialize, stemp);
	
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;  //暂时存储body序列化后的结果

	switch (headstruct.cmdtype)
	{
		//后端返回获取CDN的结果，后端没有信息
		case 1: 
		{
			casedeail = Deail_CaseOne(stemp, client);
			break;
		}

		//CDN拉源响应	     
		case 3: 
		{
			casedeail = Deail_CaseThree(stemp, client);
			break;     
		} 
			
		//case 5: Cbody_AddMirror obj_addmirror;
		//        bodytemp = obj_addmirror.
		 //       break;    //添加镜像
		//添加镜像响应
		case 6: 
		{
			if(headstruct.stateid==100)
			{ 
				cout<<"Successfully Add Mirror! "<<endl;
			}
			break;    
		} 
		//CDN文件添加	
		case 7:  
		{
			casedeail = Deail_CaseSeven(stemp, client);
			break;    
		}
		//CDN文件删除	
		case 8:  
		{
			casedeail = Deail_CaseEight(stemp, client);
			break;    
		}
		//CDN状态信息上报	
		case 9: 
		{
			casedeail = Deail_CaseNine(stemp, client);
			break;    
		} 
		//CDN文件信息上报	
		case 10: 
		{
			casedeail = Deail_CaseTen(stemp, client);
		    break;    
		}
		//后端返回获取CDN的结果，后端存有CDN的信息	
		case 11: 
		{
			casedeail = Deail_CaseEleven(stemp, client);
		    break;  
		} 
		//只需要解析头部
		case 12:
		{
			cout<<"CDN successfully scratches contents!";
			//后端发送重定向URL到前端,前端需要反序列化，提取URL和UserID
			casedeail = Deail_CaseTwelve(stemp, client);
			break;
		} 
		case 13: 
		{
			cout<<"This is a leep-Allive package sent by CDN!";
			casedeail = Deail_CaseThirting(stemp, client);
			break;
		}

		default: cout<<"the cmdtype is deyond the accepted field!"<<endl;break;
	}
	return casedeail; 
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
		r = uv_read_start((uv_stream_t*)client,CTcpServer::alloc_buffer, on_uv_read);
		if(r<0)  
		{  
			fprintf(stderr, "Read error %s\n", uv_err_name(r));  
			uv_close((uv_handle_t*)client, NULL);  
		} 
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
