#include "tcpserver.h"

casereturn CTcpServer::Start_to_SerializeorParse(const string &stemp, int client)
{
	
	switch (headstruct.cmdtype)
	{
		//case 0: cout<<"user needs to send request!"<<endl   ;break;
		case 1: //后端返回获取CDN的结果，后端没有信息
		{
			CTcpServer::GetCDNResponse();
			break;
		}
			     
		case 3: //CDN拉源响应
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
		case 6: //添加镜像响应
		{
			if(headstruct.stateid==100)
			{ 
				cout<<"Successfully Add Mirror! "<<endl;
			}
			break;    
		} 
			
		case 7:  //CDN文件添加
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
			
		case 8:  //CDN文件删除
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
			
		case 9: //CDN状态信息上报
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
			
		case 10: //CDN文件信息上报
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
			
		case 11: //后端返回获取CDN的结果，后端存有CDN的信息
		{
			cout<<"needs to send respose to the user!"<<endl;
            Cbody_GetCDNRsp  obj_getcdnrsp1;  //
			bodytemp = obj_getcdnrsp1.Parse_GetCDNRsp(obj_getcdnrsp1, stemp);
			//bodytemp can be used to send to users!
		    break;  
		} 
		case 12: cout<<"CDN successfully scratches contents!";break;  //只需要解析头部
		default: cout<<"the cmdtype is deyond the accepted field!"<<endl;break;
	}
	return casedeail; 
}
void CTcpServer::GetCDNResponse()
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
}