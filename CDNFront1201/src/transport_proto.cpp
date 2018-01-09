#include <iostream>
#include <string>
#include "transport_proto.h"

using namespace std;
using namespace transport_proto;

//公共部分
string Serialize_head::Serialhead(Serialize_head heads, const string &version, const string &usertype, const string &cmdtype, const string &stateid, const string &rsptime)
{
	//Serialize_head heads;
	heads.set_version(version);
	heads.set_usertype(usertype);
	heads.set_cmdtype(cmdtype);
	heads.set_stateid(stateid);
	heads.set_rsptime(rsptime);
	string buf;
	bool ret = heads.SerializeToString(&buf);
	if (ret)  
	{  	
		cout<<"head encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"head encode faild!"<<endl;  
	}
	return buf;
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

string Cbody_ReturnManagerIDtoCDN::Serial_ReturnManagerIDtoCDN(Cbody_ReturnManagerIDtoCDN report_ip, int managerid,const string &cdnid)  //manager前端，命令字13
{
	string temp;
	report_ip.set_managerid(managerid);
	report_ip.set_cdnid(cdnid);
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

common Cbody_ReturnManagerIDtoCDN::Parse_ReturnManagerIDtoCDN(Cbody_ReturnManagerIDtoCDN report_ip, const string &stmp)  //CDN节点使用，命令字13
{
	struct common comm;
	bool ret = report_ip.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"report_ip decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"report_ip decode faild!"<<endl;  
	}
	comm.managerid= report_ip.managerid();
	comm.cdnid= report_ip.cdnid().c_str();
	return comm;
}

string Cbody_GetCDN::Serial_GetCDN(Cbody_GetCDN getcdn, const string &userurl, const string &userid) //前端调用，命令字0
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

common Cbody_GetCDN::Parse_GetCDN(Cbody_GetCDN getcdn, const string &stmp)  //后端调用，命令字0
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

string Cbody_CDNFindSourceResult::Serial_CDNFindSourceResult(Cbody_CDNFindSourceResult findresult, const string &redirecturl,const string &user_handler)//命令字12
{
	string temp;
	findresult.set_redirecturl(redirecturl);
	findresult.set_userid(user_handler);
	bool ret = findresult.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"CDNFindSourceResult encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"CDNFindSourceResult encode faild!"<<endl;  
	}
	return temp;
}

common Cbody_CDNFindSourceResult::Parse_CDNFindSourceResult(Cbody_CDNFindSourceResult findresult, const string &stmp)  //命令字12
{	
	struct common comm;
	bool ret = findresult.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"getcdn decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"getcdn decode faild!"<<endl;  
	}
	comm.url= findresult.redirecturl().c_str();
	comm.userid= findresult.userid().c_str();
	return comm;
}

string Cbody_AssignCDN::Serial_AssignCDN(Cbody_AssignCDN assigncdn, const string &method, const string &userurl, int managerid, int timehour, int number) //前端调用，命令字2
{	
	string temp;
	assigncdn.set_method(method);
	assigncdn.set_user_url(userurl);
	assigncdn.set_managerid(managerid);
	assigncdn.set_timehour(timehour);
	assigncdn.set_number(number);
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

common Cbody_AssignCDN::Parse_AssignCDN(Cbody_AssignCDN assigncdn, const string &stmp)  //CDN调用，命令字2
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
	comm.managerid = assigncdn.managerid();
	comm.livetime = assigncdn.timehour();
	comm.number = assigncdn.number();
	return comm;
}

string Cbody_AssignCDNRep::Serial_AssignCDNRep(Cbody_AssignCDNRep assigncdnRep, const string &cdnid, const string &requested_url) //命令字4
{
	string temp;
	assigncdnRep.set_cdnid(cdnid);
	assigncdnRep.set_requested_url(requested_url);
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

common Cbody_AssignCDNRep::Parse_AssignCDNRep(Cbody_AssignCDNRep assigncdnRep, const string &stmp) //命令字4
{
	struct common comm;
	bool ret = assigncdnRep.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"assigncdnRep decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"assigncdnRep decode faild!"<<endl;  
	}
	comm.cdnid= assigncdnRep.cdnid().c_str();
	comm.url = assigncdnRep.requested_url().c_str();
	return comm;
}

string Cbody_AddMirror::Serial_AddMirror(Cbody_AddMirror addmirror, const string &domain, const string &en_ip, int service, const string &userip, int livetime, int money, int size)  //前端调用，命令字5
{
	string temp;
	addmirror.set_domain(domain);
	addmirror.set_en_ip(en_ip);
	addmirror.set_servicetype(service);
	addmirror.set_userip(userip);
	addmirror.set_livetime(livetime);
	addmirror.set_money(money);
	addmirror.set_size(size);
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

common Cbody_AddMirror::Parse_AddMirror(Cbody_AddMirror addmirror, const string &stmp)  //后端调用，命令字5
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
	comm.url = addmirror.domain().c_str();
	comm.en_ip = addmirror.en_ip().c_str();
	comm.service = addmirror.servicetype();
	comm.userid = addmirror.userip().c_str();
	comm.livetime = addmirror.livetime();
	comm.money = addmirror.money();
	comm.size = addmirror.size();
	return comm;
}

string Cbody_AssignCDNRsp::Serial_AssignCDNRsp(Cbody_AssignCDNRsp assigncdnrsp,int managerid, const string &cdnurl, const string &cdnid)  //CDN调用，命令字8
{
	string temp;
	assigncdnrsp.set_managerid(managerid);
	assigncdnrsp.set_cdn_url(cdnurl);
	assigncdnrsp.set_cdnid(cdnid);
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

common Cbody_AssignCDNRsp::Parse_AssignCDNRsp(Cbody_AssignCDNRsp assigncdnrsp, const string &stmp)  //前端调用，命令字3
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
	comm.managerid = assigncdnrsp.managerid();
	comm.url= assigncdnrsp.cdn_url().c_str();
	comm.cdnid = assigncdnrsp.cdnid().c_str();
	return comm;
}

string Cbody_FileInfo::Serial_FileInfo(Cbody_FileInfo file, int managerid, const string &filemessage, const string &cdnid)  //CDN调用，命令字10
{
	string temp;
	file.set_managerid(managerid);
	file.set_fileinforep(filemessage);
	file.set_cdnid(cdnid);
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

common Cbody_FileInfo::Parse_FileInfo(Cbody_FileInfo file, const string &stmp)  //后端调用，命令字10
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
	comm.managerid = file.managerid();
	comm.url= file.fileinforep().c_str();
	comm.cdnid = file.cdnid().c_str();
	return comm;
}

string Cbody_StateInfo::Serial_StateInfo(Cbody_StateInfo State, const string &healthstate, const string &trafficstate, int warning, int managerid, const string &cdnid,  const string &ctstate)   //CDN调用，命令字9
{
	string temp;
	State.set_health_state(healthstate);
	State.set_traffic_state(trafficstate);
	State.set_warning(warning);
	State.set_managerid(managerid);
	State.set_cdnid(cdnid);
	State.set_connect_state(ctstate);
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

StateInfo Cbody_StateInfo::Parse_StateInfo(Cbody_StateInfo State, const string &stmp)  //后端调用，命令字9
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
	stateone.healthstate = State.health_state().c_str();
	stateone.trafficstate = State.traffic_state().c_str();
	stateone.warning = State.warning();
	stateone.connectstate = State.connect_state().c_str();
	stateone.managerid = State.managerid();
	stateone.cdnid = State.cdnid().c_str();
	return stateone;
}
 
/*****************后端**********************/
string Cbody_GetCDNRspAnother::Serial_GetCDNRspAnother(Cbody_GetCDNRspAnother getcdnrspanother,const string &cdnurl,const string &userid)  //后端调用，命令字11
{
	string temp;
	getcdnrspanother.set_cdn_url(cdnurl);
	getcdnrspanother.set_userid(userid);
	bool ret = getcdnrspanother.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"getcdnrspanother encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"getcdnrspanother encode failed!"<<endl;  
	}
	return temp;
}

common Cbody_GetCDNRspAnother::Parse_GetCDNRspAnother(Cbody_GetCDNRspAnother getcdnrspanother, const string &stmp)  //前端调用，命令字11
{
	struct common comm;
	bool ret = getcdnrspanother.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"getcdnrspanother decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"getcdnrspanother decode failed!"<<endl;  
	}
	comm.url = getcdnrspanother.cdn_url().c_str();
	comm.userid = getcdnrspanother.userid().c_str();
	return comm;
}

string Cbody_GetCDNRsp::Serial_GetCDNRsp(Cbody_GetCDNRsp getcdnrsp,const string &requestedurl, const string &cdnid, int timehour, int number)  //后端调用，命令字1
{
	string temp;
	getcdnrsp.set_requested_url(requestedurl);
	getcdnrsp.set_cdnhandler(cdnid);
	getcdnrsp.set_timehour(timehour);
	getcdnrsp.set_number(number);
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

common Cbody_GetCDNRsp::Parse_GetCDNRsp(Cbody_GetCDNRsp getcdnrsp, const string &stmp)  //前端调用，命令字1
{
	struct common comm;
	bool ret = getcdnrsp.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"getcdnrsp decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"getcdnrsp decode failed!"<<endl;  
	}
	comm.url = getcdnrsp.requested_url().c_str();
	comm.cdnid = getcdnrsp.cdnhandler().c_str();
	comm.livetime = getcdnrsp.timehour();
	comm.number = getcdnrsp.number();
	return comm;
}

string Cbody_LiveTaskAssign::Serial_LiveTaskAssign(Cbody_LiveTaskAssign livetaskassign, const string &cdnhandler, int livetime, int number, const string &cdnip) //后端调用，命令字8
{
	string temp;
	livetaskassign.set_cdnhandler(cdnhandler);
	livetaskassign.set_livetime(livetime);
	livetaskassign.set_number(number);
	livetaskassign.set_cdnip(cdnip);
	bool ret = livetaskassign.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"livetaskassign encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"livetaskassign encode failed!"<<endl;  
	}
	return temp;
} 

common Cbody_LiveTaskAssign::Parse_LiveTaskAssign(Cbody_LiveTaskAssign livetaskassign, const string &stmp)   //CDN调用，命令字8
{
	struct common comm;
	bool ret = livetaskassign.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"livetaskassign decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"livetaskassign decode failed!"<<endl;  
	}
	comm.cdnid = livetaskassign.cdnhandler().c_str();
	comm.livetime = livetaskassign.livetime();
	comm.number = livetaskassign.number();
	comm.userid= livetaskassign.cdnip().c_str();
	return comm;
}

string Cbody_DelFile::Serial_DelFile(Cbody_DelFile delfile, const string &cdnhandler, const string &filelist)  //后端调用，命令字7
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

common Cbody_DelFile::Parse_DelFile(Cbody_DelFile delfile , const string &stmp)  //CDN调用，命令字7
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
	comm.cdnid = delfile.cdnhandler().c_str();
   	comm.url = delfile.delfilelist().c_str();
	return comm;
}

string Cbody_AddMirrorRsp::Serial_AddMirrorRsp(Cbody_AddMirrorRsp addmirrorrsp, const string &userip, const string &cdnurl)  //后端调用，命令字6
{
	string temp;
	addmirrorrsp.set_userip(userip);
	addmirrorrsp.set_cdnurl(cdnurl);
	bool ret = addmirrorrsp.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"addmirrorrsp encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"addmirrorrsp encode failed!"<<endl;  
	}
	return temp;
}

common Cbody_AddMirrorRsp::Parse_AddMirrorRsp(Cbody_AddMirrorRsp addmirrorrsp, const string &stmp)  //后端调用，命令字6
{
	struct common comm;
	bool ret = addmirrorrsp.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"addmirrorrsp decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"addmirrorrsp decode failed!"<<endl;  
	}
	comm.userid = addmirrorrsp.userip().c_str();
   	comm.url = addmirrorrsp.cdnurl().c_str();
	return comm;
}

string Cbody_Check_Code::Serial_Check_Code(Cbody_Check_Code checkcode, const string &cdnid,const string &stateid)   //后端调用，命令字15
{
	string temp;
	checkcode.set_cdnid(cdnid);
	checkcode.set_useless(stateid);
	bool ret = checkcode.SerializeToString(&temp);
	if (ret)  
	{  	
		cout<<"checkcode encode success!"<<endl; 

	}  
	else  
	{  
		cout<<"checkcode encode failed!"<<endl;  
	}
	return temp;
}
common Cbody_Check_Code::Parse_Check_Code(Cbody_Check_Code checkcode, const string &stmp)   //前端和CDN节点调用，命令字15
{
	struct common comm;
	bool ret = checkcode.ParseFromString(stmp);
	if (ret)  
	{  
		cout<<"checkcode decode success!"<<endl; 
	}  
	else  
	{  
		cout<<"checkcode decode failed!"<<endl;
	}  
	comm.cdnid = checkcode.cdnid().c_str();
	comm.stateid = checkcode.useless().c_str();
	return comm;
}
