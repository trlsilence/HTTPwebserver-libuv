#include "tinyweb2.h"
#include <sstream>
#include <iostream>

casereturn Deail_caseone(const string &stemp, int client);  //cmdtype 1
casereturn Deail_CaseThree(const char* stemp, const string &client); //cmdtype 3
casereturn Deail_CaseSix(const string &stemp, int client); //cmdtype 6
casereturn Deail_CaseSeven(const string &stemp, int client);  //cmdtype 7
casereturn Deail_caseEight(const string &stemp, int client);  //cmdtype 8
casereturn Deail_CaseNine(const string &stemp, const string &client); //cmdtype 9
casereturn Deail_CaseTen(const string &stemp, const string &client);  //cmdtype 10
casereturn Deail_caseEleven(const string &stemp, int client);  //cmdtype 11
casereturn Deail_CaseTwelve(const string &stemp, int client); //cmdtype 12
casereturn Deail_CaseThirting(const string &stemp, const string &client);  //cmdtype 13
casereturn Deail_Casefourting(const string &stemp, const string &client);  //cmdtype 14
casereturn Deail_Casefifting(const string &stemp, int client);   //cmdtype 15,后端返回对code码得 校验结果

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
	headstruct.cmdtype = "2";
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", headstruct.cmdtype, default_StateID, default_rsptime);
	Cbody_AssignCDN obj_assigncdn;
	cout<<"**********命令字1：后端指派CDN的处理***********"<<endl;
	printf("即将CDN的回源URL是：%s\n",bodytemp.url.c_str());
	printf("用户请求的方法是：%s\n",METHOD);
	printf("Manager的id是：%d\n",client);
	temp_body = obj_assigncdn.Serial_AssignCDN(obj_assigncdn, METHOD, bodytemp.url, client, bodytemp.livetime, bodytemp.number);  //client用于标识最初的请求是由谁发出的,写入到body中
	temp_head.append(temp_body);
	casedeail.cdnid = bodytemp.cdnid;
	cout<<"解析后的CDN的标识是："<<casedeail.cdnid.c_str()<<endl;
	cout<<"******************************************"<<endl;
	casedeail.senddata = temp_head;
	return casedeail;
}

casereturn Deail_CaseThree(const char* stemp, const string &client)  //处理命令字3
{
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	cout<<"***********命令字3：CDN回源上报的处理***********"<<endl;
	Cbody_AssignCDNRsp obj_assigncdnrsp;
	bodytemp = obj_assigncdnrsp.Parse_AssignCDNRsp(obj_assigncdnrsp,stemp);
	headstruct.cmdtype = "4";
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", headstruct.cmdtype, default_StateID, default_rsptime);
	Cbody_AssignCDNRep obj_assigncdnrep;
	//userid is CDNurl ; url is the url requested by the user;
	temp_body = obj_assigncdnrep.Serial_AssignCDNRep(obj_assigncdnrep, bodytemp.cdnid, bodytemp.url);
	temp_head.append(temp_body);
    casedeail.clienthandler=bodytemp.managerid;
	casedeail.senddata=temp_head;
	return casedeail; 
}

casereturn Deail_CaseSix(const string &stemp, int client)   //提取URL和UserID,返回给用户进行重定向
{
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	cout<<"*********命令字6：后端返回添加镜像反馈的处理*********"<<endl;
	cout<<"needs to send respose to the enterprize user!"<<endl;
    Cbody_AddMirrorRsp  obj_addmirrorrsp;  //
	bodytemp = obj_addmirrorrsp.Parse_AddMirrorRsp(obj_addmirrorrsp,stemp);
	//bodytemp can be used to send to users!
	casedeail.senddata = bodytemp.url;
	casedeail.userid = bodytemp.userid;  //这里是有问题的，应该改为string型

	printf("user IP:%s\n",casedeail.userid.c_str());
	printf("URL:%s\n",casedeail.senddata.c_str());
	char* temp1 = (char*)malloc(1024*sizeof(char));   //char*向string的转换
	sprintf(temp1,"%s", bodytemp.url.c_str());   //char*向string的转换
	temp1[bodytemp.url.length()]='\0';   //char*向string的转换

	char* temp3 = (char*)malloc(1024*sizeof(char));   //char*向string的转换
	sprintf(temp3,"%s", bodytemp.url.c_str());   //char*向string的转换
	temp3[bodytemp.url.length()]='\0';   //char*向string的转换

	char* temp2 = (char*)malloc(1024*sizeof(char));
	sprintf(temp2,"%s", bodytemp.userid.c_str());
	temp2[bodytemp.userid.length()]='\0';
	char* id = temp2;  //为了使含义更清晰
	char* url = temp1;  //同上
    char* sendurl = temp3;

	printf("转换后的用户IP：%s\n",id);
	struct FindIPHandler findiphandler;  //临时定义一个通信句柄
	IPMapManager *iphandler = IPMapManager::Instance();  //单例模式
	findiphandler = iphandler->GetUserhandlerValue(id);  //,获取用户IP对应的通信句柄，GetUserhandlerValue函数写在ClientIDtohandler.h文件中
	printf("HTTP用户句柄：%d\n",findiphandler.user);

	if(findiphandler.result == 1)
	{
		strcat(url,"/enterprize");
		CTcpServer::tinyweb_on_request_get(findiphandler.user, url, "AddMirror Feadback",sendurl);  //开始调用格式化函数向用户返回重定向
		casedeail.clienthandler = -1; 
		iphandler-> DeleteUserhandlerValue(id); //响应完毕后就删除Map中关于该用户ID的记录
	}
	else
	{
		cout<<"添加镜像反馈时找不到企业客户句柄！"<<endl;
		casedeail.clienthandler = -1; 
	}
	return casedeail;
}

casereturn Deail_CaseSeven(const string &stemp, int client)    
{
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	cout<<"***********命令字7：直播任务下发的处理**********"<<endl;
	Cbody_LiveTaskAssign obj_livetask;
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", "7", default_StateID, default_rsptime);
	bodytemp = obj_livetask.Parse_LiveTaskAssign(obj_livetask, stemp);
	temp_body = obj_livetask.Serial_LiveTaskAssign(obj_livetask, bodytemp.cdnid, bodytemp.livetime, bodytemp.number, bodytemp.userid);
	temp_head.append(temp_body); 
	casedeail.cdnid = bodytemp.cdnid;
	casedeail.senddata = temp_head;
	return casedeail;
}

casereturn Deail_CaseEight(const string &stemp, int client)  //cmdtype 7
{	
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	cout<<"***********命令字8：删除文件任务下发的处理**********"<<endl;
	Cbody_DelFile obj_delfile;
	//bodytemp = obj_delfile.parse_DelFile(obj_delfile,stemp);
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", "8", default_StateID, default_rsptime);
	bodytemp = obj_delfile.Parse_DelFile(obj_delfile, stemp);
	temp_body = obj_delfile.Serial_DelFile(obj_delfile, bodytemp.cdnid, bodytemp.url);
	temp_head.append(temp_body);
	casedeail.cdnid = bodytemp.cdnid;
	casedeail.senddata = temp_head;
	return casedeail;
}

casereturn Deail_CaseNine(const string &stemp, const string &client)
{
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	cout<<"***********命令字9：状态信息上报的处理**********"<<endl;
	struct StateInfo statedetail;
	Cbody_StateInfo obj_stateinfo;
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", "9", default_StateID, default_rsptime);
	statedetail = obj_stateinfo.Parse_StateInfo(obj_stateinfo, stemp);
	temp_body = obj_stateinfo.Serial_StateInfo(obj_stateinfo, statedetail.healthstate, statedetail.trafficstate, statedetail.warning, statedetail.managerid, statedetail.cdnid, statedetail.connectstate);  // ot use managerid , to find the true handler
	temp_head.append(temp_body);
	printf("CDN传过来的managerid:%d\n",statedetail.managerid);
	casedeail.clienthandler=statedetail.managerid;
	casedeail.senddata=temp_head;
	return casedeail;
}

casereturn Deail_CaseTen(const string &stemp, const string &client)
{
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	cout<<"***********命令字10：文件信息上报的处理**********"<<endl;
	Cbody_FileInfo obj_fileinfo;
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", "10", default_StateID, default_rsptime);
	bodytemp = obj_fileinfo.Parse_FileInfo(obj_fileinfo, stemp);
	temp_body= obj_fileinfo.Serial_FileInfo(obj_fileinfo, bodytemp.managerid, bodytemp.url, bodytemp.cdnid);
	temp_head.append(temp_body);
	casedeail.clienthandler=bodytemp.managerid;
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
	cout<<"*********命令字11：后端返回重定向URL的处理*********"<<endl;
	cout<<"needs to send respose to the user!"<<endl;
    Cbody_GetCDNRspAnother  obj_getcdnrsp1;  //
	bodytemp = obj_getcdnrsp1.Parse_GetCDNRspAnother(obj_getcdnrsp1,stemp);
	//bodytemp can be used to send to users!
	casedeail.senddata = bodytemp.url;
	casedeail.userid = bodytemp.userid;  //这里是有问题的，应该改为string型
	printf("user IP:%s\n",casedeail.userid.c_str());
	printf("URL:%s\n",casedeail.senddata.c_str());
	char* temp1 = (char*)malloc(1024*sizeof(char));   //char*向string的转换
	sprintf(temp1,"%s", bodytemp.url.c_str());   //char*向string的转换
	temp1[bodytemp.url.length()]='\0';   //char*向string的转换
	char* temp2 = (char*)malloc(1024*sizeof(char));
	sprintf(temp2,"%s", bodytemp.userid.c_str());
	temp2[bodytemp.userid.length()]='\0';
	char* id = temp2;  //为了使含义更清晰
	char* url = temp1;  //同上

	UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
	printf("转换后的用户IP：%s\n",id);
	usertohandler->InsertURLtoMap(id, url);   //将用户IP和可供重定向的URL插入到MAp 

	struct FindURL findurl; //定义一个结构体，用于存放查询IP对应的URL //
	findurl = usertohandler->FindURLtoMap(id);
	printf("确认发送给用户的URL:%s\n", findurl.findurl);
	cout<<"**********************************************"<<endl;
	char* pathinfo = "/www";
	wating_to_find(pathinfo, id);
	casedeail.clienthandler = 0; 
	return casedeail;
}

casereturn Deail_CaseTwelve(const string &stemp, int client) //前端需要反序列化，提取URL和UserID,返回给用户进行重定向
{
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	Cbody_CDNFindSourceResult findresult;
	cout<<"*********命令字12：后端返回重定向URL的处理*********"<<endl;
	cout<<"needs to send respose to the user!"<<endl;
	bodytemp = findresult.Parse_CDNFindSourceResult(findresult, stemp);
	printf("Manager返回的URL:%s\n",bodytemp.url.c_str());
	printf("Manager返回的用户IP是:%s\n",bodytemp.userid.c_str());
	//bodytemp can be used to send to users!
	casedeail.senddata = bodytemp.url;
	casedeail.userid = bodytemp.userid; 
	char* temp1 = (char*)malloc(1024*sizeof(char));
	sprintf(temp1,"%s", bodytemp.url.c_str());
	temp1[bodytemp.url.length()]='\0';
	char* temp2 = (char*)malloc(1024*sizeof(char));
	sprintf(temp2,"%s", bodytemp.userid.c_str());
	temp2[bodytemp.userid.length()]='\0';
	char* id = temp2;
	char* url = temp1;
	UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
	usertohandler->InsertURLtoMap(id, url);   //将用户IP和可供重定向的URL插入到MAp
	
	struct FindURL findurl; //定义一个结构体，用于存放查询IP对应的URL //
	findurl = usertohandler->FindURLtoMap(id);
	printf("确认发送给用户的URL:%s\n", findurl.findurl);
	cout<<"******************************************"<<endl;
	char* pathinfo = "/www";
	wating_to_find(pathinfo, id);

	casedeail.clienthandler = 0; 
	return casedeail;
}

casereturn Deail_CaseThirting(const string &stemp, const string &client) //此时。stemp作为首部的数据，直接发送
{
    string temp_body,temp_head;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
    UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();   //单例模式
	struct FindClient clientfind;  //定义一个句柄，用于存放在Map中查询后的结果
	clientfind = usertohandler->LookUpManagerMap();   //查询句柄	
    cout<<"*********命令字13：处理CDN发来的心跳包*********"<<endl;
	cout<<"needs to send managerid to CDN!"<<endl;
	cout<<"Managerid:"<<clientfind.result<<endl;
	Cbody_ReturnManagerIDtoCDN report_ip; //
	//bodytemp can be used to send to users!
	temp_body = report_ip.Serial_ReturnManagerIDtoCDN(report_ip, clientfind.result, client);
	temp_head = stemp;
	temp_head = stemp + temp_body;
	casedeail.senddata = temp_head; 
	casedeail.clienthandler = 1;  //这里暂时写死了CDN的句柄
	return casedeail;
}

casereturn Deail_Casefourting(const string &stemp, const string &client)    //CDN IP信息上报的处理
{
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	cout<<"***********命令字14：CDN IP信息上报的处理**********"<<endl;
	Cbody_FileInfo obj_fileinfo;
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", "14", default_StateID, default_rsptime);
	bodytemp = obj_fileinfo.Parse_FileInfo(obj_fileinfo, stemp);
	temp_body= obj_fileinfo.Serial_FileInfo(obj_fileinfo, bodytemp.managerid, bodytemp.url, bodytemp.cdnid);
	temp_head.append(temp_body);
	casedeail.clienthandler = bodytemp.managerid;
	casedeail.senddata = temp_head;
	return casedeail;
}

casereturn Deail_Casefifting(const string &stemp, int client)   //cmdtype 15,后端返回对code码得 校验结果
{
    Serialize_head obj_Serialize;  // head object
	struct heads headstruct;  
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	cout<<"***********命令字15：后端返回对code码校验结果的处理**********"<<endl;
	Cbody_Check_Code checkcode;
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", "15", headstruct.stateid, default_rsptime);
	bodytemp = checkcode.Parse_Check_Code(checkcode, stemp);
	cout<<endl;
	printf("接收反馈码：%s\n", bodytemp.stateid.c_str());
	cout<<endl;
	temp_body = checkcode.Serial_Check_Code(checkcode, bodytemp.cdnid, bodytemp.stateid);
	/*Cbody_FileInfo obj_fileinfo;
	temp_body = obj_fileinfo.Serial_FileInfo(obj_fileinfo, 1000, "useless", "1000");
	

	struct common bodytt;
	bodytt = checkcode.Parse_Check_Code(checkcode, temp_head);
	printf("反馈码：%s\n", bodytt.stateid.c_str()); */
	temp_head = temp_head + temp_body;
	printf("即将发送给CDN的数据是：%s\n", temp_body.c_str());
	casedeail.cdnid = bodytemp.cdnid;
	casedeail.senddata = temp_head;
	return casedeail;
}

casereturn CTcpServer::Start_to_SerializeorParseCDN(const char* stemp, const string &client)
{
	cout<<"Start to read headedr!"<<endl;
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;   
	struct common bodytemp;

	headstruct = obj_Serialize.Parsehead(obj_Serialize, stemp);
	int type,user_identity;  //262行-266行完成head中string向int型的转换
	stringstream m_cmd,m_temp;
	string u_type = headstruct.usertype;
	string cmd = headstruct.cmdtype;
	m_cmd << cmd;
	m_cmd >> type;
	m_temp << u_type;
	m_temp >> user_identity;  
	cout<<"******************************************"<<endl;
	cout<<"               命令字:"<<type<<endl;
	cout<<"               用户码:"<<user_identity<<endl;
	cout<<"******************************************"<<endl;
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;  //暂时存储body序列化后的结果
	switch (type)
	{	
		//CDN拉源响应	     
		case 3: 
		{
			casedeail = Deail_CaseThree(stemp, client);
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
		//与CDN之间的keep-alive包
		case 13: 
		{
			cout<<"This is a leep-Allive package sent by CDN!"<<endl;
			casedeail = Deail_CaseThirting(stemp, client);
			break;
		}
		//CDN IP上报
		case 14:
		{
			casedeail = Deail_Casefourting(stemp, client);
			break;
		}
		default: 
		{
			cout<<"the cmdtype is beyond the accepted field!"<<endl;
			casedeail.clienthandler = 0;
			break;
		}
	}
	return casedeail; 
}

casereturn CTcpServer::Start_to_SerializeorParseManager(const char* stemp, int client)
{
	cout<<"Start to read headedr!"<<endl;
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;   
	struct common bodytemp;
	headstruct = obj_Serialize.Parsehead(obj_Serialize, stemp);
	int type,user_identity;  //262行-266行完成head中string向int型的转换
	stringstream m_cmd,m_temp;
	string u_type = headstruct.usertype;
	string cmd = headstruct.cmdtype;
	m_cmd << cmd;
	m_cmd >> type;
	m_temp << u_type;
	m_temp >> user_identity;  
	cout<<"******************************************"<<endl;
	cout<<"               命令字:"<<type<<endl;
	cout<<"               用户码:"<<user_identity<<endl;
	cout<<"******************************************"<<endl;
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;  //暂时存储body序列化后的结果
	switch (type)
	{
		//后端返回获取CDN的结果，后端没有信息
		case 1: 
		{
			casedeail = Deail_CaseOne(stemp, client);
			cout<<"******************************************"<<endl;
			cout<<"          CDN的标识是："<<casedeail.clienthandler<<endl;
			cout<<"******************************************"<<endl;
			casedeail.clienthandler = 6;
			break;
		}
		//添加镜像响应
		case 6: 
		{
			if(headstruct.stateid=="100")
			{ 
				cout<<"*************Successfully Add Mirror!***************"<<endl;
			}
			cout<<"*************收到后端对于添加镜像的反馈***************"<<endl;
			casedeail = Deail_CaseSix(stemp, client);
			casedeail.clienthandler = 6;
			break;    
		}  	
		//直播任务下发
		case 7:  
		{
			cout<<"*************等待下发直播任务***************"<<endl;
			casedeail = Deail_CaseSeven(stemp, client);
			casedeail.clienthandler = 6;
			break;    
		}
		//CDN文件删除	
		case 8:  
		{
			casedeail = Deail_CaseEight(stemp, client);
			casedeail.clienthandler = 6;
			break;    
		}
		//后端返回获取CDN的结果，后端存有CDN的信息	
		case 11: 
		{
			//casedeail = Deail_CaseEleven(stemp, client);
			casedeail = Deail_CaseEleven(stemp, client);
			casedeail.clienthandler = 30000;
		    break;  
		} 
		//只需要解析头部
		case 12:
		{
			cout<<"CDN拉源成功，后端成功返回信息，即将发送给用户进行冲定向!"<<endl;
			//后端发送重定向URL到前端,前端需要反序列化，提取URL和UserID
			casedeail = Deail_CaseTwelve(stemp, client);
			casedeail.clienthandler = 30000;
			break;
		} 
		
		case 15: //CDN上报code码供后端校验
		{
			cout<<"后端返回对于CDN code码的校验结果！"<<endl;
			casedeail = Deail_Casefifting(stemp, client);
			casedeail.clienthandler = 6;
			break;
		}
		default: 
		{
			cout<<"the cmdtype is beyond the accepted field!"<<endl;
			casedeail.clienthandler = 0;
			break;
		}
	}
	return casedeail; 
}
