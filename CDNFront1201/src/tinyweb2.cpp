#include "CaseHandler.h" //处理命令字对应的事件
#include <sstream>   //实现string向int转换引入的库
#include <string>    //与字符串相关的需要引入上述库
#include <string.h>  //同上，这是C语言中的头文件
#include <iostream>  //C++中的标准的输入输出文件库
#include <fstream> 

using namespace std;  //使用C++的标准库的名字空间
using namespace transport_proto;  //使用协议对应的名字空间

/**Get ip address, used by HTTP and General Clients**/
char* CTcpServer::check_sockname(struct sockaddr* addr, char* compare_ip, int compare_port, char* context)  
{  
	struct sockaddr_in check_addr = *(struct sockaddr_in*) addr;  
	char check_ip[17];  //存放IP 
	struct sockaddr_in compare_addr;
	uv_ip4_addr(compare_ip, compare_port,&compare_addr);  
	//网络字节序转换成主机字符序  
	uv_ip4_name(&check_addr, (char*)check_ip, sizeof check_ip);  
	//或者像下面这样获得ip地址  
	char* check_ip1 = inet_ntoa(check_addr.sin_addr);  
	//std::cout<<context<<","<<check_ip<<","<<ntohs(check_addr.sin_port)<<endl;  
	printf("%s: %s:%d\n", context, check_ip, ntohs(check_addr.sin_port));   //context: server socket
	return check_ip1;
}

void on_uv_close(uv_handle_t* handle)
{
    if (handle != NULL)
    {
        delete handle;
    }
}

/*响应HTTP后释放连接数据*/
void after_uv_close_client(uv_handle_t* client)  //used by HTTP
{
	membuf_uninit((membuf_t*)client->data); //see: tinyweb_on_connection()
	free(client->data); //membuf_t*: request buffer
	free(client);
}
/*响应HTTP后关闭与用户的连接*/
void after_uv_write(uv_write_t* w, int status)   //used by HTTP
{
	//if(w->data)
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

	if(data == NULL || len == 0) 
		return;     //待发送的数据为空则终止
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
	sprintf(respone, "HTTP/1.1 %s\r\n"  //sprintf实现字符串拼接，头部所有的信息组装成一个response
					 "Content-Type:%s;charset=utf-8\r\n"
					 "Content-Length:%d\r\n"
					 "Location:%s\r\n\r\n",
					 status, content_type, content_length,content_location);
	if(content) 
	{
		memcpy(respone + strlen(respone), content, content_length);
	}
	return respone;
}

//invoked by tinyweb when GET request comes in
//please invoke write_uv_data() once and only once on every request, to send respone to client and close the connection.
//if not handle this request (by invoking write_uv_data()), you can close connection using tinyweb_close_client(client).
//pathinfo: "/" or "/book/view/1"
//query_stirng: the string after '?' in url, such as "id=0&value=123", maybe NULL or ""
void CTcpServer::tinyweb_on_request_get(uv_stream_t* client, char* pathinfo, char* query_stirng, const void* textmessage) 
{
	cout<<"/************************************/"<<endl;
	printf("正在调用request get....\n");
	if(strcmp(pathinfo, "/") == 0) 
	{
		char* respone = format_http_respone("200 OK", "text/html","Welcome to tinyweb", -1,query_stirng);
		write_uv_data(client, respone, -1, 0, 1);
	} 
	else if(strstr(pathinfo, "/enterprize") != NULL)  //企业用户HTTP请求的回复
	{
		char* message = (char*)textmessage;
		printf("返回给企业用户的提示：%s\n",message);
		char* tips = "任务发布信息已经添加成功！推流的CDN地址为：";
		char* sendtip = (char*)malloc(512*sizeof(char));
		sprintf(sendtip,"%s%s",tips,message);
		char* respone = format_http_respone("200 OK", "text/html",sendtip, -1,query_stirng);
		write_uv_data(client, respone, -1, 0, 1);
	}
	else if(strstr(pathinfo, "/sendwrong") != NULL)  //企业用户HTTP请求的回复
	{
		char* message = (char*)textmessage;
		printf("调用添加镜像异常反馈程序！");
		char* tips = "<h2>没有在线的Manager，无法为您提供服务！请稍后再试！<h2>";
		char* sendtip = (char*)malloc(512*sizeof(char));
		char* respone = format_http_respone("200 OK", "text/html",tips, -1,query_stirng);
		write_uv_data(client, respone, -1, 0, 1);
	}
	else if(strcmp(pathinfo, "/404") == 0) 
	{
		char* respone = format_http_respone("404 Not Found", "text/html","<h3>404 Page Not Found<h3>", -1,query_stirng);
		write_uv_data(client, respone, -1, 0, 1);
	} 
	else 
	{
		printf("准备重定向...");
		char* respone;
		//char content[1024];
		//snprintf(content, sizeof(content), "<p>pathinfo: %s</p><p>query stirng: %s</p>", pathinfo, query_stirng);
		//respone = format_http_respone("200 OK", "text/html", content, -1,query_stirng);
		respone = format_http_respone("302 Found", "text/html","<h3>已经重定向到CDN<h3>", -1,  query_stirng);
		write_uv_data(client, respone, -1, 0, 1);
	}
	printf("数据返回完成！\n");
	cout<<"/************************************/"<<endl;
}

/**分配缓存数据的buffer（发送或接收）**/
void alloc_buffer(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	buf->base = (char*)malloc(suggested_size);
	buf->len = suggested_size;
}

/*判断发送后的status状态，status==1说明发送成功共，不管成功与否都要释放数据*/
void echo_write(uv_write_t *req_write, int status)  
{  
	//printf("调用echo_write\n"); 
	if (status < 0) 
	{  
		fprintf(stderr, "Write error %s\n", uv_err_name(status));  
	}  
	char *base = (char*) req_write->data; 
	//free(req_write->data);
    free(req_write);  
}

/*向Manager后端和CDN节点发送数据的函数*/
void CTcpServer::client_write(uv_stream_t *client,const string &senddata,int cishu)  //send data to clients!
{
	/*准备写入数据*/
	cout<<"--------------正在调用数据发送的程序！------------"<<endl;
	uv_write_t *req_write = (uv_write_t *) malloc(sizeof(uv_write_t));   //uv_write_t *req_write，一次数据发送时的标识，详见libuv API
	cout<<"server speak:";    //准备打印出待发送的数据
	req_write->data = (void*)(senddata.data()); 
	/*buf 初始化*/
	uv_buf_t *buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
	char* temp1 = (char*)malloc(512*sizeof(char));  //定义临时存储的字符数组，并动态分配存储空间
	//下面两行共同完成string向char*的转换
	sprintf(temp1,"%s", senddata.c_str()); 
	temp1[senddata.length()]='\0';
	buf->base = temp1;  //填充uv_buf_t *buf的base数据部分
	buf->len = senddata.length();  //填充长度
    //new adding end
	int r2 = uv_write(req_write, client, buf, 1, echo_write);  //调用发送数据的函数，r2是发送状态，0表示成功
	if(r2==0)
	{
	    printf(" Server successfully senddata to client!\n");
	    printf("************************************************************\n");
	}
	else
	{
		cout<<"！！！！！！！！！！数据发送失败！！！！！！！！！！！"<<endl;
		++cishu;
		int shumu = cishu;
		if(shumu <= 2)   //失败次数累计达到3次
		{
			cout<<"---------开始启动定时器，5S后重发！-----------"<<endl;
			sleep(5);
			CTcpServer::client_write(client, senddata, cishu);
		}
		else
		{
			cout<<"连接失败的次数太多，关闭与客户端的连接"<<endl;
			struct StateandID state_id;
            UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
            //查询后删除Map中的记录

			state_id = usertohandler->FindManagerMapValue(client);
			usertohandler->UserIDtoHandlerManager::DeleteManagerHandler(state_id.client_id);

			state_id = usertohandler->FindCDNMapValue(client);
			usertohandler->UserIDtoHandlerManager::DeleteCDNHandler(state_id.cdnid);
            //查询后删除Manager Map中的记录
			//uv_close((uv_handle_t*)client, after_uv_close_client);
		}
	} 
	/*写入数据结束*/
} 

/***used to serialize header and requested url to manager client***/
/*收到并解析用户请求后调用该函数，使用自定义协议封装首部及用户请求的URL*/
string CTcpServer::Serial_head_userurl(const string &userurl, const string &userid)   
{
	Serialize_head obj_Serialheader;
	string str1=obj_Serialheader.Serialhead(obj_Serialheader, fversion, identity, cmdzero, default_StateID, default_rsptime);
	Cbody_GetCDN getcdn;
	string str2 = getcdn.Serial_GetCDN(getcdn, userurl, userid);
	str1.append(str2);
	cout<<"序列化后的结果:"<<endl;
	cout<<str1<<endl;
	/*struct heads headprase;
	headprase = obj_Serialheader.Parsehead(obj_Serialheader,str1);
	cout<<"version:"<<headprase.version<<endl;
	cout<<"usertype:"<<headprase.usertype<<endl;
	cout<<"cmdtype:"<<headprase.cmdtype<<endl;
	cout<<"Successfully Serialized!"<<endl;*/
	return str1;
}
/*收到并解析企业用户请求后调用该函数，构造命令字5*/
string CTcpServer::construct_cmdfive(const string &domain, const string enip, int service, const string &userid, int livetime, int money, int size)   
{
	Serialize_head obj_Serialheader;
	string str1=obj_Serialheader.Serialhead(obj_Serialheader, fversion, identity, "5", default_StateID, default_rsptime);
	Cbody_AddMirror addmirror;
	string str2 = addmirror.Serial_AddMirror(addmirror, domain, enip, service, userid, livetime, money, size);
	str1.append(str2);
	cout<<"序列化后的结果:"<<endl;
	cout<<str1<<endl;
	return str1;
}

/*查询MAP中是否有与用户请求相关的URL，里面涉及到两个MAP的查询*/
void wating_to_find(char* pathinfo, char* userAddress)
{
	printf("*******用户IP1*****：%s\n",userAddress);
	printf("pathinfo:%s\n",pathinfo);
	char* address;
	UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();  //单例模式
	//Map遍历开始
	cout<<"\n**********开始遍历Map,查找Map对应的URL*************\n"<<endl;
	map<char*, char*> aaa = usertohandler->m_mapIPtoURL;
	map<char*, char*>::iterator it; 
	for(it = aaa.begin(); it != aaa.end(); it++)
	{
		if(strcmp(it->first,userAddress)==0)  //使用字符串比较函数判断是否匹配
		{
			address = it->second;
			cout<<"URL: "<<it->second<<endl;
			printf("URL对应的IP记录:%s\n",userAddress);
		}
		else
		{
			cout<<"暂时查询不到URL ！"<<endl;
		}  		  	
	}
	cout<<"\n*****************遍历Map结束****************\n"<<endl;
	cout<<"\n**********开始查找用户IP对应的句柄*************\n"<<endl;
	struct FindIPHandler findiphandler;  //临时定义一个通信句柄
	uv_stream_t* bingbing;
	IPMapManager *iphandler = IPMapManager::Instance();  //单例模式
	/*map<char*, uv_stream_t*> bbb = iphandler->m_mapIPtoHandler;
	map<char*, uv_stream_t*>::iterator it11; 
	for(it11 = bbb.begin(); it11 != bbb.end(); it11++)
	{
		printf("遍历后的用户IP:%s\n", it11->first);
		printf("遍历后的用户句柄：%d\n", it11->second);		  	
	}*/
	findiphandler = iphandler-> GetUserhandlerValue(userAddress);  //,获取用户IP对应的通信句柄，GetUserhandlerValue函数写在ClientIDtohandler.h文件中
	printf("HTTP用户句柄：%d\n",findiphandler.user);
	if(findiphandler.result==1)
	{
		char* query_stirng = address ;//findurl.findurl; //返回的重定向的URL
		printf("准备将url返回至用户:%s\n",query_stirng);				
		CTcpServer::tinyweb_on_request_get(findiphandler.user, pathinfo, query_stirng,"useless");  //开始调用格式化函数向用户返回重定向
		usertohandler-> DeleteURLtoMap(userAddress); //响应完毕后就删除Map中关于该用户请求URL的记录	
	}
	else
	{
		cout<<"找不到HTTP用户句柄！"<<endl;
	}
}

int CTcpServer::judge_number(char* str)  
{  
	char ch='@',*p;
	int pos,count1 = 0;
	if(p=strchr(str,ch))//查找输入的字符
	{
		pos = p-str;		
		printf("\'%c\' first appeared at subscript %d\n",ch,p-str);//换算成下标输出
			
	}
	for(int i=0;i<strlen(str);i++)  
	{
		if(str[i]=='@')
			count1+=1;
	} 
	return count1;  
}


/*读取用户的HTTP请求*/
void on_read_http(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf1)  
{
	printf("\n------------------HTTP请求头 --------------------\n");
	printf("%s\n", buf1->base);  //打印HTTP请求头
	printf("\n------------------------------------------------\n");
    client_temp.push_back(client);  //将用户的HTTP 句柄插入到容器当中
    cout<<"容器的大小为："<<client_temp.size()<<endl;
    for(int i=1; i<=client_temp.size();i++)
    {
    	printf("容器中的记录:%d\n",client_temp[i-1]);
    }
	/*开始调用函数获取用户IP*/
	struct sockaddr sockname, peername;  
	int namelen,r;
	namelen = sizeof sockname;  
	//有连接，可以获得目标的ip和端口 
	char* userAddress; 
	r = uv_tcp_getpeername((uv_tcp_t*)client, &peername, &namelen);  
	userAddress = CTcpServer::check_sockname(&peername, "127.0.0.1", -1, "HTTP用户IP:");    //获取到了用户IP
	/*获取用户IP结束*/

	IPMapManager *iphandler = IPMapManager::Instance();   //单例模式
	struct FindIPHandler findiphandler;
	printf("HTTP用户IP:%s\n", userAddress);
	//findiphandler = iphandler->GetUserhandlerValue(userAddress);
	iphandler->AddIPtoMap(client,userAddress);  //将用户对应的IP和句柄插入到Map
	/*test start*/
	/*test end*/
	char* crln2;
	membuf_t* membuf = (membuf_t*) client->data;   //see tinyweb_on_connection()
	assert(membuf);
	membuf_append_data(membuf, buf1->base, nread);  //将buf1->base赋值给membuf

	if((crln2 = strstr((char*)membuf->data, "\r\n\r\n")) != NULL)   //strstr检查membuf->data中是否有"\r\n\r\n"
	{
		char* request_header = membuf->data;  
		*crln2 = '\0';
		printf("\n-------------Tinyweb client request: -------------\n");
		//printf("\n----Tinyweb client request: ----\n%s\n", request_header);
		//handle GET request
		if(request_header[0]=='G' && request_header[1]=='E' && request_header[2]=='T') 
		{
			char *query_stirng, *end, *temp,*t_temp,*host, *ziduan;
			char* pathinfo = request_header + 3; //去掉用户请求方式GET及其后的空格
			/*************分割用户请求头部获得host**************/
			t_temp = strtok(buf1->base, "\r\n");  //使用"\r\n"分割首部
			int numbber =1;
			while(t_temp)  //循环分割Http头部，获得host字段
		    {
		        t_temp=strtok(NULL,"\r\n");
		        ++numbber;
		        if(numbber==2)
		        {
		        	host = t_temp;  //获得host字段
		        }
		    }
		    host = host+6;
		    printf("用户请求的host:%s\n",host);
		    /********************获得host结束**************/

			temp = strtok(pathinfo, "\r\n"); 
			//printf("%s\n",temp);//输出分解的字符串s
			temp=temp+1;
			temp=strtok(temp," ");//使用空格分割，去掉第一行中的HTTP 1.1 标识
			printf("用户请求的内容路径：%s\n",temp);//输出分割后的字符串 

			/*下面if中的部分是为了判断windows版的CDN，现在不用了*/
			char* judgeuser = "enterprize";
			if(strstr(temp,"(null)")!=NULL || strstr(temp,"favicon.ico")!=NULL)
			{
					cout<<"请求里不可以包含null！"<<endl;
			}
			else
			{
				if(strstr(temp,judgeuser)!=NULL)
				{
					printf("***************这是一个企业客户*****************\n");
					while(isspace(*pathinfo)) 
					{
						printf("打印pathinfo：%s\n",pathinfo);
						pathinfo++;
					}
					end = strchr(pathinfo, ' ');   //find the position '' first comes up 
					if(end) *end = '\0';
					query_stirng = strchr(pathinfo, '?'); //split pathinfo and query_stirng using '?'
					if(query_stirng) 
					{
						*query_stirng = '\0';
						 query_stirng++;
					}
					pathinfo = pathinfo+1;
					printf("打印pathinfo：%s\n",pathinfo);
					printf("打印query_string：%s\n",query_stirng);
					//开始构造命令字5
					string pending_send_str; //定义字符串，存放序列化后即将发送给manager后端的数据
					UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();   //单例模式
					//int clienthandler=1;  //需要指定的Manager的ID，这里手动设置为1,因为Manager后端最先连接，第一个插入到Map
					struct FindClient clientfind;  //定义一个句柄，用于存放在Map中查询后的结果
					clientfind = usertohandler->LookUpManagerMap();  //通过指定key查询句柄	
					printf("构造命令字5时查找Manager的句柄：%d\n", clientfind.sendclient);
					if(clientfind.jieguo == 0 || clientfind.sendclient==0)
					{
						cout<<"没有找到manager后端的ID，无法完成发送！"<<end;	
						sleep(2);
						tinyweb_close_client(client);
						//CTcpServer::tinyweb_on_request_get(client_temp[0], "/sendwrong", "send back!", "const void* textmessage");
					}
					else
					{
						string userid = userAddress;   //需要添加用户的真实IP,形参传递过来的
						/*************分割用户请求头部获得各个字段**************/
						int checkresult = CTcpServer::judge_number(pathinfo);
						if(checkresult == 6)
						{
							ziduan = strtok(pathinfo, "@");  //使用"\r\n"分割首部
							printf("打印字段：%s\n",pathinfo);
							int numbber =1;
							string domain, enip, service, livetime, money, size;
							while(ziduan)  //循环分割Http头部，获得host字段
						    {
						        ziduan=strtok(NULL,"@");
						        ++numbber;
						        if(numbber == 2)
						        {
						        	domain = ziduan;
						        	printf("企业用户的域名：%s\n", domain.c_str());
						        }
						        else if(numbber == 3)
						        {
						        	enip = ziduan;
						        	printf("企业用户的IP：%s\n", enip.c_str());
						        }
						        else if(numbber == 4)
						        {
						        	service = ziduan;
						        	printf("企业用户的服务类行：%s\n", service.c_str());
						        }
						         else if(numbber == 5)
						        {
						        	livetime = ziduan;
						        }
						         else if(numbber == 6)
						        {
						        	money = ziduan;
						        }
						         else if(numbber == 7)
						        {
						        	size = ziduan;
						        }
						        else
						        {
						        	break;
						        }
						    }
				    		/********************获得各个字段结束**************/
				    		/*****************将部分字段由string转换为int***************/
				    		int servicetype,livetimer,moneys,sizes;
							stringstream ss,ss1,ss2,ss3;
							ss<<service; ss>>servicetype;
							ss1<<livetime; ss1>>livetimer;
							ss2<<money; ss2>>moneys;
							ss3<<size; ss3>>sizes;
							printf("企业用户的直播时长：%d\n", livetimer);
							printf("企业用户支付的费用：%d\n", moneys);
							printf("企业用户直播预估产生的大小：%d\n", sizes);
							printf("userid:%s\n", userid.c_str());
							pending_send_str = CTcpServer::construct_cmdfive(domain, enip, servicetype, userid, livetimer, moneys, sizes); //该函数用于创建命令字，如果是直播，需要重写函数，创建命令字5
							CTcpServer::client_write(clientfind.sendclient, pending_send_str,0);//update      //////////////////////////调用写函数将用户请求发送给后端
						//CTcpServer::tinyweb_on_request_get(client, pathinfo, query_stirng,"useless");  //开始调用格式化函数向用户返回重定向
						}
						else
						{
							cout<<"添加镜像的URL不正确，请仔细检查@符号！"<<endl;
							//sleep(5);
							uv_close((uv_handle_t*)client , on_uv_close);
						}

					}
				}
				else  //正常用户的请求处理
				{
					char* tempone = (char*)malloc(256*sizeof(char));
					sprintf(tempone,"%s%s",host,temp);  //拼接host和用户请求路径
					string str = tempone;  //字符串转换，下面即将用于序列化，以便传给manager后端
					printf("组装后准备发往后端的URL:%s\n",str.c_str());  //打印查看结构是否正确
					string pending_send_str; //定义字符串，存放序列化后即将发送给manager后端的数据

					UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();   //单例模式
					//int clienthandler=1;  //需要指定的Manager的ID，这里手动设置为1,因为Manager后端最先连接，第一个插入到Map
					struct FindClient clientfind;  //定义一个句柄，用于存放在Map中查询后的结果
					clientfind = usertohandler->LookUpManagerMap();   //查询句柄		

					string userid = userAddress;   //需要添加用户的真实IP,形参传递过来的
					pending_send_str = CTcpServer::Serial_head_userurl(str, userid);  // 该函数用于创建命令字，如果是直播，需要重写函数，创建命令字214

					printf("\n--------------------------------\n");
					printf("\n*******************准备将用户请求发送至后端*****************\n");
					while(isspace(*pathinfo)) pathinfo++;
					end = strchr(pathinfo, ' ');   //find the position '' first comes up
					if(end) *end = '\0';
					query_stirng = strchr(pathinfo, '?'); //split pathinfo and query_stirng using '?'
					if(query_stirng) 
					{
						*query_stirng = '\0';
						 query_stirng++;
					}
					//printf("pathinfo:%s\n",pathinfo);
					if(clientfind.result == 0)
					{
						cout<<"没有找到manager后端的ID，无法完成发送！"<<end;
						printf("Vector中存放的句柄是：%d", client_temp[0]);
						sleep(5);
						tinyweb_close_client(client);
						//uv_close((uv_handle_t*) client_temp[0], NULL);
						if(client_temp.size()>=1)
						{
							client_temp.pop_back();
						}
						cout<<"删除后的容器的大小为："<<client_temp.size()<<endl;	
					}
					else
					{
						CTcpServer::client_write(clientfind.sendclient, pending_send_str,0);//update      //////////////////////////调用写函数将用户请求发送给后端
					}
				}
			}
		} 
		else 
		{
			tinyweb_close_client(client);  //关闭连接
		}
	} 
}

/*读取CDN及Manager后端发送来的数据*/
void on_read_client(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf) 
{ 
	if (nread < 0) //读取错误  
	{
		static int count = 1;  
	    if (nread == UV_EOF)
	    {
	    	cout<<client<<"client has closed!"<<endl;
			fprintf(stderr, "UV_EOF....\n");
			struct StateandID state_id;
			UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
            //查询后删除Map中的记录
			state_id = usertohandler->FindManagerMapValue(client);
			usertohandler->UserIDtoHandlerManager::DeleteManagerHandler(state_id.client_id);

			state_id = usertohandler->FindCDNMapValue(client);
			usertohandler->UserIDtoHandlerManager::DeleteCDNHandler(state_id.cdnid);   
	    }
	    else
	    {  
			++count;
			fprintf(stderr, "Read error %s : %s\n", uv_err_name(nread), uv_strerror(nread)); 
			if(count==4)
			{
				cout<<"失败次数太多，断开与客户的连接！"<<endl;
				uv_close((uv_handle_t*) client, NULL);
			} 
	    }    
	    return;  
	}
	if(nread>0)  //正常读取
	{
		/************反序列化首部，获取命令字及用户类型!***********/
		Serialize_head obj_Serialize;  // head object
		struct heads headstruct;   
		headstruct = obj_Serialize.Parsehead(obj_Serialize, buf->base);
		int type;
		stringstream ss,ss00;
		string cmd = headstruct.cmdtype;
		ss<<cmd;
		ss>>type;
		int ustype;
		string user1 = headstruct.usertype;
		ss00<<user1;
		ss00>>ustype;
		cout<<"命令字:"<<type<<endl;
		cout<<"用户编号类型:"<<ustype<<endl;
		/*********************反序列化首部结束******************/

		/*******************遍历Manager Map句柄得到Handler**************/
		const char *find_str = "aaabbb";
		if(strstr(buf->base,find_str)!=NULL)
		{
			UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
			struct FindClient findmman;
			findmman = usertohandler->UserIDtoHandlerManager::LookUpManagerMap();
		}
		/****************遍历Manager Map句柄到Handler结束**************/
		const char *pending_find_str = "Hello";  //为了判别后端的心跳包
		if(strstr(buf->base,pending_find_str)!=NULL||strstr(buf->base,"hello")!=NULL)  //判断是否为后端的心跳包
		{
			cout<<"Manager发送的数据:"<<buf->base<<endl;
			cout<<"这是后端的心跳包！"<<endl;
			printf("manager的句柄是：%d \n",client);
			//句柄插入map 
			UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
			if (type == 0 && ustype == 0)
			{
				cout<<"该用户输入了错误的指令！！！"<<endl;
			}

			int lookupmanager = 0;
			lookupmanager = usertohandler->UserIDtoHandlerManager::FindManagerHandler(client);  //遍历map，查找记录
			printf("lookupmanager的结果是：%d \n",lookupmanager);
			if(lookupmanager == 1)   //id已经存在了，无需重复插入
			{
				cout<<"～～～～～～～～～Manager的ID已经存在!～～～～～～～～"<<endl;
			}
			else
			{
				usertohandler->UserIDtoHandlerManager::AddManagerIDtoHandler(client);
				//membuf_uninit((membuf_t*)client->data);
			}	
		}
		else  
		{
			if(ustype == 10001)
			{
				cout<<"～～～～～这是来自Manager的消息！～～～～～～"<<endl;
			}
			if(ustype == 10000)
			{
				cout<<"～～～～～～～这是来自CDN的消息！～～～～～～～"<<endl;
				if(type == 14)   //命令字14
				{	
					struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
					Cbody_FileInfo obj_fileinfo;
					bodytemp = obj_fileinfo.Parse_FileInfo(obj_fileinfo, buf->base);

					UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
					int lookupcdn;
					lookupcdn = usertohandler->UserIDtoHandlerManager::FindCDNHandler(client);  //遍历map，查找记录
					if(lookupcdn == 1)   //id已经存在了，无需重复插入
					{
						cout<<"～～～～～～～～～CDN的ID已经存在!～～～～～～～～"<<endl;
					}
					else
					{
						usertohandler->UserIDtoHandlerManager::AddCDNIDtoHandler(bodytemp.cdnid ,client);
					}	
				}
			}			
			/*反序列化收到的数据*/
			struct casereturn caseresult;//定义结构提接收不同命令字对应的case的处理结果，返回值包括待发送的数据和句柄的映射值，结构体里的字段：int clienthandler;int ManagerID;string userid;string senddata;
			UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
			struct StateandID state_id;  //结构体里的字段：int result;int client_id;
			printf("*******************读取CDN和后端发来的相关信息***************\n");
			printf("现在处理的客户端句柄是：%d \n",client);
			cout<<"收到的客户端消息是："<<buf->base;
			cout<<endl; //输出空行
            
			if (type == 0 && ustype == 0)
			{
				cout<<"该用户输入了错误的指令！！！"<<endl;
			}
			/**********************查找混合存放map结束*********************/
			/*----------------开始调用处理各种case的程序---------------*/
			//struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
			//从这儿开始就要根据CDN或者Manager来实现查找Map获得句柄对应的int型的ID
			if(ustype == 10000 && type != 13)
			{
				struct StateandID state_id1;
				state_id1 = usertohandler->FindCDNMapValue(client);
				printf("CDN句柄对应的键值：%d\n",state_id1.client_id);
				caseresult = CTcpServer::Start_to_SerializeorParseCDN(buf->base, state_id1.cdnid); //
			}

			if(ustype == 10001)
			{
				struct StateandID state_id1;
				state_id1 = usertohandler->FindManagerMapValue(client);
				caseresult = CTcpServer::Start_to_SerializeorParseManager(buf->base, state_id1.client_id); //
			}

			if(caseresult.clienthandler == 30000)
			{
				cout<<"用户重定向中......"<<endl;
			}
			else if(caseresult.clienthandler == -1)
			{
				cout<<"已经反馈添加镜像信息至企业用户！"<<endl;
			}
			else if(caseresult.clienthandler == 0)
			{
				cout<<"～～～对于命令字的处理结果异常，客户端可能填入了错误的命令字！～～～"<<endl;
			} 
			else    //这里需要根据不同用户，查找Map记录
			{
				if(type == 1 || type == 7 || type == 8|| type == 15)
				{
					struct FindClient findclient;
					if(type == 15)
					{
						findclient = usertohandler->GetCDNHandler(caseresult.cdnid);//caseresult.clienthandler为int类型
						printf("查找后的CDN客户端句柄是：%d,准备发送数据! \n",findclient.sendclient);
						printf("命令字15返回给CDN的数据是：%s\n",buf->base);
						//实际读取到了内容，准备发送数据 
					    if(findclient.result == 1)
					    {
					    	CTcpServer::client_write(findclient.sendclient, buf->base,0);//
					    }
						else
						{
							cout<<"找不到关于CDN客户端的记录，无法完成数据的发送！"<<endl;
						}
					}
					else
					{
						findclient = usertohandler->GetCDNHandler(caseresult.cdnid);//caseresult.clienthandler为int类型
						if(findclient.result == 1)
						{
							printf("查找后的CDN客户端句柄是：%d,准备发送数据! \n",findclient.sendclient);
							//实际读取到了内容，准备发送数据 
							CTcpServer::client_write(findclient.sendclient, caseresult.senddata,0);//
							printf("*********************************\n");
						}
						else
						{
							cout<<"找不到关于CDN客户端的记录，无法完成数据的发送！"<<endl;
						}
					}
					
				}
				if (type == 13)
				{
					cout<<"这是CDN发送的心跳包！"<<endl;
					printf("CDN的句柄是：%d \n",client);
					UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();   //单例模式
					struct FindClient clientfind;  //定义一个句柄，用于存放在Map中查询后的结果
					clientfind = usertohandler->LookUpManagerMap();
					Serialize_head obj_Serialize;  // head object
					string temp_head;    //暂时存储头部序列化后的结果
					temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", "13", "100", "50");
					Cbody_ReturnManagerIDtoCDN report_ip; //
					//bodytemp can be used to send to users!
					string temp_body = report_ip.Serial_ReturnManagerIDtoCDN(report_ip, clientfind.result, "0");
					caseresult.senddata = temp_head + temp_body;
					cout<<"准备发送数据给CDN"<<endl;
					//实际读取到了内容，准备发送数据 
					CTcpServer::client_write(client, caseresult.senddata,0);
					printf("*********************************\n");
				}
				if(type == 3 || type == 9 || type == 10 || type == 14)
				{
					struct FindClient findclient;
					//以下为测试情况
					/*if(type == 15)
					{
						findclient = usertohandler->GetManagerHandler(1);//caseresult.clienthandler为int类型
						printf("查找后Manager的客户端句柄是：%d,准备发送数据! \n",findclient.sendclient);
					    //实际读取到了内容，准备发送数据 
					    string tt = caseresult.senddata;
					    CTcpServer::client_write(findclient.sendclient, buf->base,0);//
					}
					else
					{*/
					findclient = usertohandler->GetManagerHandler(caseresult.clienthandler);//caseresult.clienthandler为int类型
					if(findclient.result == 1)
					{
						printf("查找后Manager的客户端句柄是：%d,准备发送数据! \n",findclient.sendclient);
					    //实际读取到了内容，准备发送数据 
					    CTcpServer::client_write(findclient.sendclient, caseresult.senddata,0);//
					    printf("*********************************\n");

					}
					else
					{
						cout<<"找不到关于Manager客户端的记录，无法完成数据的发送！"<<endl;
					}
					//}
					
				}	
			}		   	
		}
	}
	
} 

/*连接成功后最先调用的预读取函数*/
void on_uv_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf) 
{
	//judge the received data is correct or wrong
	if (nread < 0) 
	{  
	    if (nread == UV_EOF)
	    {
	    	printf("此%d ",client);
	    	cout<<"client has closed!"<<endl;
			fprintf(stderr, "UV_EOF....\n");
			struct StateandID state_id;
			UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
            //查询后删除Map中的记录
			state_id = usertohandler->FindManagerMapValue(client);
			usertohandler->UserIDtoHandlerManager::DeleteManagerHandler(state_id.client_id);

			state_id = usertohandler->FindCDNMapValue(client);
			usertohandler->UserIDtoHandlerManager::DeleteCDNHandler(state_id.cdnid);    
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
		char *pending_find_str = "User-Agent";  //为了判别是否为HTTP请求
       	if(strstr(buf->base,pending_find_str)!=NULL && strstr(buf->base,"gzip")!=NULL && strstr(buf->base,"HTTP"))
		{
			printf("\n*******************This is a HTTP user*******************\n");
			printf("HTTP用户句柄：%d\n",client);
			//printf("用户请求：%s\n",buf->base);
			if(strstr(buf->base,"(null)")!=NULL || strstr(buf->base,"favicon.ico")!=NULL)
			{
				printf("请求里不可以包含null！\n");
				//uv_close((uv_handle_t*)client , on_uv_close);
				//free((uv_handle_t*)client->data);
			}
			else
			{
				cout<<"*********************开始调用读取HTTP的函数**********************"<<endl;
				on_read_http(client, nread, buf);  //HTTP的读取处理函数
			}
		}
		else
		{
			printf("****************This is a Manager or CDN node***************\n");
			on_read_client(client, nread, buf);  //manager后端和CDN节点的读取处理函数
		}
	} 
	else if(nread == -1) 
	{
		tinyweb_close_client(client);
	}
	//if(buf->base)
	free(buf->base);
}

void tinyweb_on_connection(uv_stream_t* server, int status)
{
	printf("enter on_new_connection...\n");
	printf("*******************************************************\n");
	assert(server == (uv_stream_t*)&_server);
	if(status == 0) 
	{
		int r;  
		uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
		client->data = malloc(sizeof(membuf_t));

		membuf_init((membuf_t*)client->data, 128);
		uv_tcp_init(_loop, client);  //将全局的主循环和处理连接的对象关联起来  
		uv_accept((uv_stream_t*)&_server, (uv_stream_t*)client);
		//开始读取客户端发送的数据，并设置好接收缓存分配的函数alloc_buffer和读取完毕后的回调函数on_uv_read
		r = uv_read_start((uv_stream_t*)client, alloc_buffer, on_uv_read);
		if(r<0)  
		{  
			fprintf(stderr, "Read error %s\n", uv_err_name(r));  
			uv_close((uv_handle_t*)client, NULL);  
		} 
	} 

}

int ReadPort()    //从文件里面读取端口信息
{
	ifstream infile("src/ipaddress.cfg");//打开1.txt    
	string s;
	stringstream ssw;
	int port;  
	if (!infile)  
		cout<<"文件打开失败！"<<endl;   //不能打开文件，报错  
	else  
	{  
		while(getline(infile,s))  
		{  
            ssw << s;
			ssw >> port;
			//cout<<port<<endl;   
  		}  
	 }
	return port; 
}
//start web server, linstening ip:port
//ip can be NULL or "", which means "0.0.0.0"
void CTcpServer::start_listener(uv_loop_t* loop, const char* ip, int port) 
{
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
		printf("*******************************************************\n");
		printf("前端开启监听......\n");
		//uv_timer_start(&timer_req, callback, 1000, 1000);
	}
	//sockname，获得监听自己的ip和端口  
	memset(&sockname, -1, sizeof sockname);  
	namelen = sizeof sockname;  
	r = uv_tcp_getsockname(&_server, &sockname, &namelen);
	CTcpServer::check_sockname(&sockname, "0.0.0.0", port, "server socket"); 
	//没有连接时，peername是无意义的  
	namelen = sizeof peername;  
	r = uv_tcp_getpeername(&_server, &peername, &namelen);  
	if(r == -1)
	{  
		printf("socket is not connected.\n");  
	}    
}

int main()
{
	cout<<"------------------------------------------------------------------"<<endl;
	cout<<"--  * * * * *        * * * *     * * * * *       *           *  --"<<endl;
	cout<<"--  *        *     *             *         *     * *         *  --"<<endl;
	cout<<"--  *        *    *              *          *    *   *       *  --"<<endl;
	cout<<"--  * * * * *     *              *          *    *     *     *  --"<<endl;
	cout<<"--  *        *    *              *          *    *       *   *  --"<<endl;
	cout<<"--  *        *     *             *         *     *         * *  --"<<endl;
	cout<<"--  * * * * *        * * * *     * * * * *       *           *  --"<<endl;
	cout<<"------------------------------------------------------------------"<<endl;
	int server_port =  ReadPort();
	CTcpServer::start_listener(uv_default_loop(), "0.0.0.0", server_port);
	uv_run(uv_default_loop(), UV_RUN_DEFAULT);  //运行主循环
}
