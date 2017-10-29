#include <iostream>
#include "transport_proto.pb.h"
#include <string>
#include "transport_proto.h"

using namespace std;
using namespace transport_proto;

//公共部分
string Serialize_head::Serialhead(Serialize_head heads, float version, int usertype, int cmdtype, int stateid, int rsptime)
{
	//Serialize_head heads;
	heads.set_version(version);
	heads.set_usertype(usertype);
	heads.set_cmdtype(cmdtype);
	heads.set_stateid(stateid);
	heads.set_rsptime(rsptime);
	string buf;
	bool ret = heads.SerializeToString(&buf);
	//bool ret1 = pp.SerializeToString(&buf1);
	if (ret)  
	{  	
		cout<<"head encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"head encode faild!"<<endl;  
	}
	//buf.append(buf1);
	//cout<<buf<<endl; 
	return buf;
	//free(buf);
}

heads Serialize_head::Parsehead(Serialize_head heads,const string &stmp)
{	
	//Serialize_head heads;
	struct heads headss;
	//bool ret =  proto_headp.ParseFromArray(stmp,len);
	bool ret =  heads.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"head decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"head decode faild!"<<endl;  
	}
	//heads->ParseFromString(stmp);
	headss.version = heads.version();
	headss.usertype = heads.usertype();
	headss.cmdtype = heads.cmdtype();
	headss.stateid = heads.stateid();
	headss.rsptime = heads.rsptime();
	return headss;  //返回反序列化后的结构体	
}

string Serial_ReturnManagerIDtoCDN(Cbody_ReturnManagerIDtoCDN report_ip, int managerid)  //manager前端
{
	string temp;
	report_ip.set_managerid(managerid);
	bool ret = report_ip.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"report_ip encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"report_ip encode faild!"<<endl;  
	}
	return temp;
}

int Parse_ReturnManagerIDtoCDN(Cbody_ReturnManagerIDtoCDN report_ip, const string &stmp)  //CDN节点使用
{
	int temp;
	bool ret = report_ip.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"report_ip decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"report_ip decode faild!"<<endl;  
	}
	temp= report_ip.managerid();
	return temp;
}

string Cbody_GetCDN::Serial_GetCDN(Cbody_GetCDN getcdn, const string &userurl, const string &userid) //前端调用
{
	string temp;
	getcdn.set_user_url(userurl);
	getcdn.set_userid(userid);
	bool ret = getcdn.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"getcdn encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"getcdn encode faild!"<<endl;  
	}
	return temp;
}

common Cbody_GetCDN::Parse_GetCDN(Cbody_GetCDN getcdn, const string &stmp)  //后端调用
{	
	struct common comm;
	bool ret = getcdn.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"getcdn decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"getcdn decode faild!"<<endl;  
	}
	comm.url= getcdn.user_url().c_str();
	comm.userid= getcdn.userid().c_str();
	return comm;
}

string Cbody_AssignCDN::Serial_AssignCDN(Cbody_AssignCDN assigncdn, const string &method, const string &userurl, int managerid) //前端调用
{	
	string temp;
	assigncdn.set_method(method);
	assigncdn.set_user_url(userurl);
	assigncdn.set_managerid(managerid);
	bool ret = assigncdn.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"assigncdn encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"assigncdn encode faild!"<<endl;  
	}
	return temp;	
}

common Cbody_AssignCDN::Parse_AssignCDN(Cbody_AssignCDN assigncdn, const string &stmp)  //CDN调用
{
	struct common comm;
	bool ret = assigncdn.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"assigncdn decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"assigncdn decode faild!"<<endl;  
	}
	comm.url = assigncdn.method().c_str();
	comm.userid = assigncdn.user_url().c_str();
	comm.cdnid = assigncdn.managerid();
	return comm;
}

string Cbody_AssignCDNRep::Serial_AssignCDNRep(Cbody_AssignCDNRep assigncdnRep, const string &cdnip)
{
	string temp;
	assigncdnRep.set_cdnip(cdnip);
	bool ret = assigncdnRep.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"assigncdnRep encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"assigncdnRep encode faild!"<<endl;  
	}
	return temp;
}

string Cbody_AssignCDNRep::Parse_AssignCDNRep(Cbody_AssignCDNRep assigncdnRep, const string &stmp)
{
	string temp;
	bool ret = assigncdnRep.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"assigncdnRep decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"assigncdnRep decode faild!"<<endl;  
	}
	temp= assigncdnRep.cdnip().c_str();
	return temp;
}

string Cbody_AddMirror::Serial_AddMirror(Cbody_AddMirror addmirror, const string &domain, const string &en_ip)  //前端调用
{
	string temp;
	addmirror.set_domain(domain);
	addmirror.set_en_ip(en_ip);
	bool ret = addmirror.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"addmirror encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"addmirror encode faild!"<<endl;  
	}
	return temp;
}

common Cbody_AddMirror::Parse_AddMirror(Cbody_AddMirror addmirror, const string &stmp)  //后端调用
{
	struct common comm;
	bool ret = addmirror.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"addmirror decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"addmirror decode faild!"<<endl;  
	}
	comm.url= addmirror.domain().c_str();
	comm.userid= addmirror.en_ip().c_str();
	return comm;
}

string Cbody_AssignCDNRsp::Serial_AssignCDNRsp(Cbody_AssignCDNRsp assigncdnrsp,int managerid, const string &cdnurl)  //CDN调用
{
	string temp;
	assigncdnrsp.set_managerid(managerid);
	assigncdnrsp.set_cdn_url(cdnurl);
	bool ret = assigncdnrsp.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"assigncdnrsp encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"assigncdnrsp encode faild!"<<endl;  
	}
	return temp;
}

common Cbody_AssignCDNRsp::Parse_AssignCDNRsp(Cbody_AssignCDNRsp assigncdnrsp, const string &stmp)  //前端调用
{
	struct common comm;
	bool ret = assigncdnrsp.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"assigncdnrsp decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"assigncdnrsp decode faild!"<<endl;  
	}
	comm.cdnid = assigncdnrsp.managerid();
	comm.url= assigncdnrsp.cdn_url().c_str();
	return comm;
}

string Cbody_FileInfo::Serial_FileInfo(Cbody_FileInfo file, int managerid, const string &filemessage)  //CDN调用
{
	string temp;
	file.set_managerid(managerid);
	file.set_fileinforep(filemessage);
	bool ret = file.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"fileInfo encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"fileInfo encode faild!"<<endl;  
	}
	return temp;
}

common Cbody_FileInfo::Parse_FileInfo(Cbody_FileInfo file, const string &stmp)  //后端调用
{
	struct common comm;
	bool ret = file.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"fileInfo decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"fileInfo decode faild!"<<endl;  
	}
	comm.cdnid = file.managerid();
	comm.url= file.fileinforep().c_str();
	return comm;
}

string Cbody_StateInfo::Serial_StateInfo(Cbody_StateInfo State,int maxband, int useband, int dcdnband, int maxspace, int usespace, int healthdegree, int lastkeeptime, int managerid)   //CDN调用
{
	string temp;
	State.set_m_ullmaxband(maxband);
	State.set_m_ulluseband(useband);
	State.set_m_ulldcdnband(dcdnband);
	State.set_m_ullmaxspace(maxspace);
	State.set_m_ullusespace(usespace);
	State.set_m_uihealthdegree(healthdegree);
	State.set_m_ilastkeepalivetime(lastkeeptime);
	State.set_managerid(managerid);
	bool ret = State.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"State encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"State encode faild!"<<endl;  
	}
	return temp;
}

StateInfo Cbody_StateInfo::Parse_StateInfo(Cbody_StateInfo State, const string &stmp)  //后端调用
{
	struct StateInfo stateone;
	bool ret = State.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"State decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"State decode faild!"<<endl;  
	}
	stateone.m_ullMaxBand = State.m_ullmaxband();
	stateone.m_ullUseBand = State.m_ulluseband();
	stateone.m_ullDCDNBand = State.m_ulldcdnband();
	stateone.m_ullMaxSpace = State.m_ullmaxspace();
	stateone.m_ullUseSpace = State.m_ullusespace();
	stateone.m_uiHealthDegree = State.m_uihealthdegree();
	stateone.m_iLastKeepAliveTime = State.m_ilastkeepalivetime();
	stateone.managerid = State.managerid();
	return stateone;
}

//后端
string Cbody_GetCDNRsp::Serial_GetCDNRsp(Cbody_GetCDNRsp getcdnrsp,const string &cdnurl,const string &userid)  //后端调用
{
	string temp;
	getcdnrsp.set_cdn_url(cdnurl);
	getcdnrsp.set_userid(userid);
	bool ret = getcdnrsp.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"getcdnrsp encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"getcdnrsp encode failed!"<<endl;  
	}
	return temp;
}

common Cbody_GetCDNRsp::Parse_GetCDNRsp(Cbody_GetCDNRsp getcdnrsp, const string &stmp)  //前端调用
{
	struct common comm;
	bool ret = getcdnrsp.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"fileInfo decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"fileInfo decode failed!"<<endl;  
	}
	comm.url = getcdnrsp.cdn_url().c_str();
	comm.userid = getcdnrsp.userid().c_str();
	return comm;
}

string Cbody_AddFile::Serial_AddFile(Cbody_AddFile addfile, int cdnhandler, const string &filelist)  //后端调用
{
	string temp;
	addfile.set_addfilelist(filelist);
	bool ret = addfile.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"AddFile encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"AddFile encode failed!"<<endl;  
	}
	return temp;
} 

common Cbody_AddFile::Parse_AddFile(Cbody_AddFile addfile , const string &stmp)   //CDN调用
{
	struct common comm;
	bool ret = addfile.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"addfile decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"addfile decode failed!"<<endl;  
	}
	comm.cdnid = addfile.cdnhandler();
	comm.url= addfile.addfilelist().c_str();
	return comm;
}

string Cbody_DelFile::Serial_DelFile(Cbody_DelFile delfile, int cdnhandler, const string &filelist)  //后端调用
{
	string temp;
	delfile.set_cdnhandler(cdnhandler);
	delfile.set_delfilelist(filelist);
	bool ret = delfile.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"delfile encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"delfile encode failed!"<<endl;  
	}
	return temp;
}

common Cbody_DelFile::Parse_DelFile(Cbody_DelFile delfile , const string &stmp)  //CDN调用
{
	struct common comm;
	bool ret = delfile.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"delfileInfo decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"delfileInfo decode failed!"<<endl;  
	}
	comm.cdnid = delfile.cdnhandler();
    comm.url = delfile.delfilelist().c_str();
	return comm;
}
