#include "tinyweb2.h"
#include <sstream>
#include <iostream>

casereturn Deail_caseone(const string &stemp, int client);  //cmdtype 1
casereturn Deail_CaseThree(const char* stemp, int client); //cmdtype 3
casereturn Deail_CaseSeven(const string &stemp, int client);  //cmdtype 7
casereturn Deail_caseEight(const string &stemp, int client);  //cmdtype 8
casereturn Deail_CaseNine(const string &stemp, int client); //cmdtype 9
casereturn Deail_CaseTen(const string &stemp, int client);  //cmdtype 10
casereturn Deail_caseEleven(const string &stemp, int client);  //cmdtype 11
casereturn Deail_CaseTwelve(const string &stemp, int client); //cmdtype 12
casereturn Deail_CaseThirting(const string &stemp, int client);  //cmdtype 13

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
	printf("即将CDN的回源URL是：%s\n",bodytemp.url.c_str());
	printf("用户请求的方法是：%s\n",METHOD);
	printf("Manager的id是：%d\n",client);
	temp_body = obj_assigncdn.Serial_AssignCDN(obj_assigncdn, METHOD, bodytemp.url, client);  //client用于标识最初的请求是由谁发出的,写入到body中
	temp_head.append(temp_body);
	casedeail.clienthandler = bodytemp.iuserid;
	cout<<"解析后的CDN的标识是："<<casedeail.clienthandler<<endl;
	casedeail.senddata = temp_head;
	return casedeail;
}

casereturn Deail_CaseThree(const char* stemp, int client)  //处理命令字1
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
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", headstruct.cmdtype, default_StateID, default_rsptime);
	Cbody_AssignCDNRep obj_assigncdnrep;
	//userid is CDNurl ; url is the url requested by the user;
	temp_body = obj_assigncdnrep.Serial_AssignCDNRep(obj_assigncdnrep, client, bodytemp.url);
	temp_head.append(temp_body);
    casedeail.clienthandler=bodytemp.cdnid;
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
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", "7", default_StateID, default_rsptime);
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
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", "8", default_StateID, default_rsptime);
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
	temp_head = obj_Serialize.Serialhead(obj_Serialize, fversion, "7", "9", default_StateID, default_rsptime);
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

	cout<<"needs to send respose to the user!"<<endl;
    Cbody_GetCDNRspAnother  obj_getcdnrsp1;  //
	bodytemp = obj_getcdnrsp1.Parse_GetCDNRspAnother(obj_getcdnrsp1,stemp);
	//bodytemp can be used to send to users!
	casedeail.senddata = bodytemp.url;
	casedeail.userid = bodytemp.userid;  //这里是有问题的，应该改为string型
	printf("user IP:%s\n",casedeail.userid.c_str());
	printf("URL:%s\n",casedeail.senddata.c_str());
	char* temp1 = (char*)malloc(1024*sizeof(char));
	sprintf(temp1,"%s", bodytemp.url.c_str());
	temp1[bodytemp.url.length()]='\0';
	char* temp2 = (char*)malloc(1024*sizeof(char));
	sprintf(temp2,"%s", bodytemp.userid.c_str());
	temp2[bodytemp.userid.length()]='\0';
	char* id = temp2;
	char* url = temp1;

	UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();
	printf("用户IP222：%s\n",id);
	usertohandler->InsertURLtoMap(id, url);   //将用户IP和可供重定向的URL插入到MAp 

	struct FindURL findurl; //定义一个结构体，用于存放查询IP对应的URL //
	findurl = usertohandler->FindURLtoMap(id);
	printf("Ensure URL:%s\n", findurl.findurl);
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

	cout<<"needs to send respose to the user!"<<endl;
	bodytemp = findresult.Parse_CDNFindSourceResult(findresult, stemp);
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
	casedeail.clienthandler = 0; 
	return casedeail;
}

casereturn Deail_CaseThirting(const string &stemp, int client) //此时。stemp作为首部的数据，直接发送
{
    string temp_body,temp_head;
	struct common bodytemp;//用于存放序列化后的结果(需要用的字段为:string:url,string userid,int managerid,int cdnid）
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
    int managerid = 1; //暂时只填写第一个,实际上这里需要查找
	cout<<"needs to send managerid to CDN!"<<endl;
    Cbody_ReturnManagerIDtoCDN report_ip; //
	//需要添加查找ManagerID的函数,这里需要添加查找ManagerID的函数，暂时写为1
	//bodytemp can be used to send to users!
	temp_body = report_ip.Serial_ReturnManagerIDtoCDN(report_ip, managerid, client);
	temp_head = stemp;
	temp_head = stemp + temp_body;
	casedeail.senddata = temp_head; 
	casedeail.clienthandler = 2;  //client
	return casedeail;
}

casereturn CTcpServer::Start_to_SerializeorParse(const char* stemp, int client)
{
	cout<<"Start to read headedr!"<<endl;
	struct casereturn casedeail;   //用于存放序列化后的string和需要发送的客户端ID(int clienthandler,string senddata)
	Serialize_head obj_Serialize;  // head object
	struct heads headstruct;   
	struct common bodytemp;
	//char* temp1 = (char*)malloc(1024*sizeof(char));
	//sprintf(temp1,"%s", c.c_str());
	//temp1[c.length()]='\0';
	headstruct = obj_Serialize.Parsehead(obj_Serialize, stemp);
	int type;
	stringstream ss;
	string cmd = headstruct.cmdtype;
	ss<<cmd;
	ss>>type;
	cout<<"命令字:"<<type<<endl;
	
	string temp_head;    //暂时存储头部序列化后的结果
	string temp_body;  //暂时存储body序列化后的结果

	switch (type)
	{
		//后端返回获取CDN的结果，后端没有信息
		case 1: 
		{
			casedeail = Deail_CaseOne(stemp, client);
			cout<<"CDN的标识是："<<casedeail.clienthandler<<endl;
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
			if(headstruct.stateid=="100")
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
			casedeail.clienthandler = 0;
			//cout<<"pending......"<<endl;
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
