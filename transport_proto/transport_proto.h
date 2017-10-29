#include <iostream>
#include "transport_proto.pb.h"
#include <cstring>
#include <string>

using namespace transport_proto;
using namespace std;
struct heads
{
	float version;
	int usertype;
	int cmdtype;
	int stateid;
	int rsptime;
};

struct common
{
	string url;
	string userid;
	int cdnid;
};

struct StateInfo
{
	int m_ullMaxBand;
	int m_ullUseBand;
	int m_ullDCDNBand;
	int m_ullMaxSpace;
	int m_ullUseSpace;
	int m_uiHealthDegree;
	int m_iLastKeepAliveTime;
	int managerid;
};
//头部为所有对象公用
class Serialize_head : public Transport_proto_CHead
{
	public:
		Serialize_head(){}
		~ Serialize_head(){}
		virtual string Serialhead(Serialize_head heads,float version, int usertype, int cmdtype, int stateid, int rsptime);
		virtual heads Parsehead(Serialize_head heads,const string &stmp);	
};
//Manager前端使用
class Cbody_ReturnManagerIDtoCDN : public Transport_proto_CBodyFront_ReturnManagerIDtoCDN   //命令字还是13，返回给CDN的心跳包
{
	public:
		Cbody_ReturnManagerIDtoCDN(){}
		~ Cbody_ReturnManagerIDtoCDN(){}
		virtual string Serial_ReturnManagerIDtoCDN(Cbody_ReturnManagerIDtoCDN report_ip, int managerid);
		virtual int Parse_ReturnManagerIDtoCDN(Cbody_ReturnManagerIDtoCDN report_ip, const string &stmp);
		
};
class Cbody_GetCDN : public Transport_proto_CBodyFront_GetCDN  //命令字0：获取CDN
{
	public:
		Cbody_GetCDN(){}
		~ Cbody_GetCDN(){}
		virtual string Serial_GetCDN(Cbody_GetCDN getcdn,const string &userurl, const string &userid);
		virtual common Parse_GetCDN(Cbody_GetCDN getcdn,const string &stmp);
};

class Cbody_AssignCDN : public Transport_proto_CBodyFront_AssignCDN  //命令字2：指派CDN
{
	public:
		Cbody_AssignCDN(){}
		~ Cbody_AssignCDN(){}
		virtual string Serial_AssignCDN(Cbody_AssignCDN assigncdn, const string &method, const string &userurl, int managerid);
		virtual common Parse_AssignCDN(Cbody_AssignCDN assigncdn, const string &stmp);
};
class Cbody_AssignCDNRep : public Transport_proto_CBodyFront_AssignCDNRep //命令字4：指派CDN结果上报
{
	public:
		Cbody_AssignCDNRep(){}
		~ Cbody_AssignCDNRep(){}
		//填充首部
		virtual string Serial_AssignCDNRep(Cbody_AssignCDNRep assigncdnRep,	const string &cdnip);
		virtual string Parse_AssignCDNRep(Cbody_AssignCDNRep assigncdnRep, const string &stmp);
};

class Cbody_AddMirror : public Transport_proto_CBodyFront_AddMirror  //命令字5：添加镜像
{
	public:
		Cbody_AddMirror(){}
		~ Cbody_AddMirror(){}
		virtual string Serial_AddMirror(Cbody_AddMirror addmirror, const string &domain, const string &en_ip);
		virtual common Parse_AddMirror(Cbody_AddMirror addmirror, const  string &stmp);
};

//CND端使用
class Cbody_CDNReportIP : public Transport_proto_CBodyCDN_CDNReportIP  //命令字13：
{
	public: 
		Cbody_CDNReportIP(){}
		~ Cbody_CDNReportIP(){}
                //暂时填充首部，无需添加body，为后面添加以太坊钱包地址做准备
		//CPP文件中没有方法的具体实现
};
class Cbody_AssignCDNRsp : public Transport_proto_CBodyCDN_AssignCDNRsp  //命令字3：指派CDN的响应
{
	public:
		Cbody_AssignCDNRsp(){}
		~ Cbody_AssignCDNRsp(){}
		virtual string Serial_AssignCDNRsp(Cbody_AssignCDNRsp assigncdnrsp, int managerid, const string &cdnurl);
		virtual common Parse_AssignCDNRsp(Cbody_AssignCDNRsp assigncdnrsp, const string &stmp);
};

class Cbody_FileInfo : public Transport_proto_CBodyCDN_FileInfo  //命令字10：CDN文件信息上报 
{
	public:
		Cbody_FileInfo(){}
		~ Cbody_FileInfo(){}
		virtual string Serial_FileInfo(Cbody_FileInfo file, int managerid, const string &filemessage);
		virtual common Parse_FileInfo(Cbody_FileInfo file, const string &stmp);
};

class Cbody_StateInfo : public Transport_proto_CBodyCDN_StateInfo  //命令字9：CDN状态信息上报
{
	public:
		Cbody_StateInfo(){}
		~ Cbody_StateInfo(){}
		virtual string Serial_StateInfo(Cbody_StateInfo State,int maxband, int useband, int dcdnband, int maxspace,int usespace, int healthdegree, int lastkeeptime, int managerid);
		virtual StateInfo Parse_StateInfo(Cbody_StateInfo State, const string &stmp);
};

//manager后端使用
class Cbody_GetCDNRsp : public Transport_proto_CBodyBack_GetCDNRsp  //命令字1：获取CDN的响应
{
	public:
		Cbody_GetCDNRsp(){}
		~ Cbody_GetCDNRsp(){}
	virtual string Serial_GetCDNRsp(Cbody_GetCDNRsp getcdnrsp,const string &cdnurl, const string &userid);
	virtual common Parse_GetCDNRsp(Cbody_GetCDNRsp getcdnrsp, const string &stmp);
};

class Cbody_GetCDNRspAnother : public Transport_proto_CBodyBack_GetCDNRspAnother  //命令字11：获取CDN的响应，CDN有请求的信息
{
	public:
		Cbody_GetCDNRspAnother(){}
		~ Cbody_GetCDNRspAnother(){}
	virtual string Serial_GetCDNRspAnother(Cbody_GetCDNRspAnother getcdnrspanother,const string &requestedurl, int cdn_handler);  
	virtual common Parse_GetCDNRspAnother(Cbody_GetCDNRspAnother getcdnrspanother, const string &stmp);
};


class Cbody_AddMirrorRsp : public Transport_proto_CBodyBack_AddMirrorRsp //命令字6：添加镜像的响应
{
	public:
		Cbody_AddMirrorRsp(){}
		~ Cbody_AddMirrorRsp(){}
	//填充首部即可
};

class Cbody_AddFile : public Transport_proto_CBodyBack_AddFile  //命令字7：指示CDN添加文件
{
	public:
		Cbody_AddFile(){}
		~ Cbody_AddFile(){}
	virtual string Serial_AddFile(Cbody_AddFile addfile, int cdnhandler, const string &filelist);
	virtual common Parse_AddFile(Cbody_AddFile addfile , const string &stmp);
		
};

class Cbody_DelFile : public Transport_proto_CBodyBack_DelFile  //命令字8：指示CDN删除文件
{
	public:
		Cbody_DelFile(){}
		~ Cbody_DelFile(){}
		virtual string Serial_DelFile(Cbody_DelFile delfile, int cdnhandler, const string &filelist);
		virtual common Parse_DelFile(Cbody_DelFile delfile , const string &stmp);
};






