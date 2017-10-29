#include <iostream> 
#include "tcpserver.h"
#include "ClientIDtohandler.h"

using namespace std;
//CTcpServer tcp_server;

void CTcpServer::alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) 
{ 
	//分配接收缓存内存和设置建议大小，小于等于实际大小
	*buf = uv_buf_init((char*) malloc(suggested_size), suggested_size);
}

void CTcpServer::start_read()
{
	int r1 = uv_read_start((uv_stream_t*) handle, alloc_buffer, echo_read);       
	if(r1<0)  
	{  
		fprintf(stderr, "Read error %s\n", uv_err_name(r1));  
		uv_close((uv_handle_t*)handle, NULL);  
	} 
}

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf) {
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
	const char *pending_find_str = "Hello";
	if(strstr(buf->base,pending_find_str)!=NULL)
	{
		cout<<"client speak(未序列化):"<<buf->base<<endl;
		cout<<"这是心跳包！"<<endl;
	}
	else
	{
		//printf("现在处理的客户端句柄是：%d ",client);
		/*反序列化收到的数据*/
		struct casereturn caseresult;
		UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
		struct StateandID state_id;
		state_id = usertohandler->FindMapValue(client);
		if (state_id.result=1)
		{	
			cout<<"客户端ID存在，无需再添加了!"<<endl;;
		}
		else
		{
			state_id.client_id = usertohandler->UserIDtoHandlerManager::AddIDtoHandler(client);
		}
		caseresult=CTcpServer::Start_to_SerializeorParse(buf->base, state_id.client_id);
	    	//printf("nread:%d\n",nread);  //打印出收到的字符数
		cout<<"client speak(未序列化):"<<buf->base<<endl;
		uv_stream_t *sendclient;
		sendclient=usertohandler->GetUserHandler(caseresult.clienthandler);
	    	printf("查找后的客户端句柄是：%d,准备发送数据 \n",sendclient);
		//实际读取到了内容，准备发送数据 
	   	if(nread>0)
		{
	       		CTcpServer::client_write(sendclient, caseresult.senddata);
		}
	}
} 

casereturn CTcpServer::Start_to_SerializeorParse(const string &stemp, int client)
{
	struct casereturn casedeail;
	Serialize_head obj_Serialize;
	struct heads headstruct;
	headstruct = obj_Serialize.Parsehead(obj_Serialize, stemp);
	struct common bodytemp;
	string temp_head;
	string temp_body;
	switch (headstruct.cmdtype)
	{
		//case 0: cout<<"user needs to send request!"<<endl   ;break;
		//后端返回获取CDN的结果，后端没有信息
		case 1: 
		{
			Cbody_GetCDNRsp  obj_getcdnrsp;  //
			bodytemp = obj_getcdnrsp.Parse_GetCDNRsp(obj_getcdnrsp, stemp);
			headstruct.cmdtype = 2;
			temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, 7, headstruct.cmdtype, default_StateID, default_rsptime);
			Cbody_AssignCDN obj_assigncdn;
			temp_body = obj_assigncdn.Serial_AssignCDN(obj_assigncdn, METHOD, bodytemp.url, client); 
			temp_head.append(temp_body);
			casedeail.clienthandler = bodytemp.cdnid;
			casedeail.senddata = temp_head;
			break;
		}

		//CDN拉源响应	     
		case 3: 
		{
			Cbody_AssignCDNRsp obj_assigncdnrsp;
			bodytemp = obj_assigncdnrsp.Parse_AssignCDNRsp(obj_assigncdnrsp,stemp);
			headstruct.cmdtype = 4;
			temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, 7, headstruct.cmdtype, default_StateID, default_rsptime);
			Cbody_AssignCDNRep obj_assigncdnrep;
			temp_body = obj_assigncdnrep.Serial_AssignCDNRep(obj_assigncdnrep, bodytemp.url);
			temp_head.append(temp_body);
            casedeail.clienthandler=bodytemp.cdnid;
			casedeail.senddata=temp_head; 
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
			Cbody_DelFile obj_delfile;
			//bodytemp = obj_delfile.parse_DelFile(obj_delfile,stemp);
			temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, 7, headstruct.cmdtype, default_StateID, default_rsptime);
			bodytemp = obj_delfile.Parse_DelFile(obj_delfile, stemp);
			temp_body = obj_delfile.Serial_DelFile(obj_delfile, client, bodytemp.url);
			temp_head.append(temp_body);
			casedeail.clienthandler=bodytemp.cdnid;
			casedeail.senddata=temp_head;
			break;    
		}
		//CDN文件删除	
		case 8:  
		{
			Cbody_AddFile obj_addfile;
			temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, 7, headstruct.cmdtype, default_StateID, default_rsptime);
			bodytemp = obj_addfile.Parse_AddFile(obj_addfile, stemp);
			temp_body = obj_addfile.Serial_AddFile(obj_addfile, client, bodytemp.url);
			temp_head.append(temp_body); 
			casedeail.clienthandler=bodytemp.cdnid;
			casedeail.senddata=temp_head;
			break;    
		}
		//CDN状态信息上报	
		case 9: 
		{
			struct StateInfo statedetail;
			Cbody_StateInfo obj_stateinfo;
			temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, 7, headstruct.cmdtype, default_StateID, default_rsptime);
			statedetail = obj_stateinfo.Parse_StateInfo(obj_stateinfo, stemp);
			temp_body = obj_stateinfo.Serial_StateInfo(obj_stateinfo, statedetail.m_ullMaxBand, statedetail.m_ullUseBand,
			statedetail.m_ullDCDNBand, statedetail.m_ullMaxSpace, statedetail.m_ullUseSpace, statedetail.m_uiHealthDegree, statedetail.m_iLastKeepAliveTime,client);
			temp_head.append(temp_body);
			casedeail.clienthandler=statedetail.managerid;
			casedeail.senddata=temp_head;
			break;    
		} 
		//CDN文件信息上报	
		case 10: 
		{
			Cbody_FileInfo obj_fileinfo;
			temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, 7, headstruct.cmdtype, default_StateID, default_rsptime);
			bodytemp = obj_fileinfo.Parse_FileInfo(obj_fileinfo, stemp);
			temp_body= obj_fileinfo.Serial_FileInfo(obj_fileinfo, client, bodytemp.url);
			temp_head.append(temp_body);
			casedeail.clienthandler=bodytemp.cdnid;
			casedeail.senddata=temp_head;
		    break;    
		}
		//后端返回获取CDN的结果，后端存有CDN的信息	
		case 11: 
		{
			cout<<"needs to send respose to the user!"<<endl;
            Cbody_GetCDNRsp  obj_getcdnrsp1;  //
			bodytemp = obj_getcdnrsp1.Parse_GetCDNRsp(obj_getcdnrsp1, stemp);
			//bodytemp can be used to send to users!
		    break;  
		} 
		//只需要解析头部
		case 12: cout<<"CDN successfully scratches contents!";break;

		default: cout<<"the cmdtype is deyond the accepted field!"<<endl;break;
	}
	return casedeail; 
}

void CTcpServer::client_write(uv_stream_t *client,const string &senddata)
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
	    //printf("send success!");
	}
    	CTcpServer::start_read();
	/*写入数据结束*/
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
	free(base); 
    CTcpServer::start_read();  
}
  
int CTcpServer::tcp_listener() 
{  
	struct sockaddr sockname, peername;  
	int namelen;  
	int r;  
	struct sockaddr_in addr;
	uv_ip4_addr("0.0.0.0", server_port,&addr);  
	uv_tcp_init(loop, &tcpServer);  
	uv_tcp_bind(&tcpServer,(const sockaddr*)&addr,0);  
	uv_listen((uv_stream_t*) &tcpServer, 128, on_connection);  

	//sockname，获得监听自己的ip和端口  
	memset(&sockname, -1, sizeof sockname);  
	namelen = sizeof sockname;  
	r = uv_tcp_getsockname(&tcpServer, &sockname, &namelen);  
	CTcpServer::check_sockname(&sockname, "0.0.0.0", server_port, "server socket");  
	//没有连接时，peername是无意义的  
	namelen = sizeof peername;  
	r = uv_tcp_getpeername(&tcpServer, &peername, &namelen);  
	if (r == -1) {  
		cout<<"socket is not connected."<<endl;  
	}  
	return 0;  
}  
  
void on_connection(uv_stream_t* server, int status) 
{  
	struct sockaddr sockname, peername;  
	int namelen;  
	int r;  
	if (status != 0) 
	{  
		cout<<"Connect error %s\n"<<endl;  
	}  
	handle = (uv_tcp_t*)malloc(sizeof(*handle));  
	r = uv_tcp_init(loop, handle);  
	/* associate server with stream */  
	handle->data = server;  
	uv_accept(server, (uv_stream_t*)handle);  
	memset(&sockname, -1, sizeof sockname);  
	namelen = sizeof sockname;  
	r = uv_tcp_getsockname(handle, &sockname, &namelen);  
	CTcpServer::check_sockname(&sockname, "0.0.0.0", server_port, "server socket");  

	//有连接，可以获得目标的ip和端口  
	namelen = sizeof peername;  
	r = uv_tcp_getpeername(handle, &peername, &namelen);  
	CTcpServer::check_sockname(&peername, "127.0.0.1", -1, "accepted socket peer");  
        
	//这儿开启一个流读取数据
    	CTcpServer::start_read();
}  
  
void CTcpServer::check_sockname(struct sockaddr* addr, const char* compare_ip, int compare_port, const char* context) 
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
	cout<<context<<","<<check_ip<<","<<ntohs(check_addr.sin_port)<<endl;  
}
  
int main() 
{  
	//CTcpServer tcpserver;
	loop = uv_default_loop();  
	CTcpServer::tcp_listener();  
	return uv_run(loop, UV_RUN_DEFAULT);  
}
