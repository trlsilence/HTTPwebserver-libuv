#include <string> 
#include <time.h>
#include <iostream>  
#include <map>
#include "uv.h"

using namespace std;
int returnid;
string returnIP;

struct StateandID
{
	int result;
	int client_id;
};
struct FindURL
{
	int result;
	char* findurl;
};
//***************管理维护到的MAP*******************//
class UserIDtoHandlerManager
{
    public:
        static UserIDtoHandlerManager *Instance();//单例模式
        int AddIDtoHandler(uv_stream_t *client);
        int AddCDNIDtoHandler(uv_stream_t *client);
        void DeleteIDtoHandler(int clientid);
        void InsertURLtoMap(char* userip, char* userurl);
        FindURL FindURLtoMap(char* userip);
        void DeleteURLtoMap(char* userip);

        uv_stream_t* GetUserHandler(int clientid);
        uv_stream_t* GetCDNHandler(int clientid);
		StateandID FindMapValue(uv_stream_t *client);

    public:
        map<int, uv_stream_t*>  m_mapUserIDtoHandler;
        map<int, uv_stream_t*>  m_mapCDNIDtoHandler;

        map<char*, char*>  m_mapIPtoURL;

    private:
        static UserIDtoHandlerManager *s_UserIDtoHandlerManager;
};

UserIDtoHandlerManager *UserIDtoHandlerManager::s_UserIDtoHandlerManager = NULL;
UserIDtoHandlerManager *UserIDtoHandlerManager::Instance()//单例模式
{
    if(s_UserIDtoHandlerManager == NULL)
    {
        s_UserIDtoHandlerManager = new UserIDtoHandlerManager();
    }
    return  s_UserIDtoHandlerManager;
}



// userIP to URl begin
void UserIDtoHandlerManager::InsertURLtoMap(char* userip, char* userurl)
{
	m_mapIPtoURL.insert(pair<char*, char*>(userip, userurl));
	//m_mapIPtoHandler.insert(map<string, uv_stream_t *>::value_type(IP, client));
	cout<<"UserURL"<<userurl<<"添加成功"<<endl;
}

FindURL UserIDtoHandlerManager::FindURLtoMap(char* userip)
{
	struct FindURL findresult;
	map<char*, char*>::iterator it; 
	for(it = m_mapIPtoURL.begin(); it != m_mapIPtoURL.end(); it++)
	{
       	if(strcmp(it->first,userip)!=0)
       	{
       		findresult.result = 1;
       	}
       	else
       	{
       		findresult.result = 0;
       		findresult.findurl = it->second;
       		break;
       	}
    }
    return findresult;
}

void UserIDtoHandlerManager::DeleteURLtoMap(char* userip)
{
	map<char*,char*>::iterator iter;
	iter = m_mapIPtoURL.find(userip);
	if(iter == m_mapIPtoURL.end())
	{
	 	cout<<"Map中没有该用户的记录"<<endl;
	 	//return 0;
	}
	else
	{
	 	m_mapIPtoURL.erase(userip);
	 	cout<<iter->first<<"用户记录成功删除"<<endl;
	}
}
// userIP to URl end

//Manager Related start
int UserIDtoHandlerManager::AddIDtoHandler(uv_stream_t *client)//添加用户id。
{ 
    static int clientid = 1;
    //cout<<"new clienthandler :"<<client<<endl;
    m_mapUserIDtoHandler[clientid] = client;
    cout<<"客户端"<<clientid<<"添加成功"<<endl;
    returnid = clientid;
    ++clientid;
    return returnid;
}
void UserIDtoHandlerManager::DeleteIDtoHandler(int clientid)//删除用户请求任务(id自减)。
{
	map<int, uv_stream_t*>::iterator iter;
	iter= m_mapUserIDtoHandler.find(clientid);
	if(iter == m_mapUserIDtoHandler.end())
	{
	 	cout<<"没有找到该用户任务"<<endl;
	 	//return 0;
	}
	else
	{
	 	m_mapUserIDtoHandler.erase(clientid);
	 	cout<<iter->first<<"用户任务成功删除"<<endl;
	}
}

uv_stream_t* UserIDtoHandlerManager::GetUserHandler(int clientid)  //根据整型的ID获得用户的句柄
{
	map<int,uv_stream_t*>::iterator it;  
	it = m_mapUserIDtoHandler.find(clientid);
	uv_stream_t *clienthandle;
	if(it!=m_mapUserIDtoHandler.end())
	{
		cout<<"成功找到handler!"<<endl;
		clienthandle = it->second;
	}
	else
	{
		cout<<"没有找到handler!"<<endl;
	}	
	return clienthandle;
}

StateandID UserIDtoHandlerManager::FindMapValue(uv_stream_t *client)   //查找map中是否有客户端的句柄
{
	struct StateandID stateandid;
	map<int,uv_stream_t*>::iterator it; 
	it = m_mapUserIDtoHandler.begin();
	while(it != m_mapUserIDtoHandler.end())
	{
		if(it->second!=client)
		{
	  		stateandid.result = 0;
			stateandid.client_id = 0;
		}
		else
		{
			stateandid.result = 1;
			stateandid.client_id = it->first;
		}
	    it ++;         
	}
	return stateandid;
}
//Manager Related end

//CDN related start
int UserIDtoHandlerManager::AddCDNIDtoHandler(uv_stream_t *client)//添加用户id。
{ 
    static int clientid = 1;
    //cout<<"new clienthandler :"<<client<<endl;
    m_mapCDNIDtoHandler[clientid] = client;
    cout<<"客户端"<<clientid<<"添加成功"<<endl;
    returnid = clientid;
    ++clientid;
    return returnid;
}

uv_stream_t* UserIDtoHandlerManager::GetCDNHandler(int clientid)  //根据整型的ID获得用户的句柄
{
	map<int,uv_stream_t*>::iterator it;  
	it = m_mapCDNIDtoHandler.find(clientid);
	uv_stream_t *clienthandle;
	if(it!=m_mapCDNIDtoHandler.end())
	{
		cout<<"成功找到handler!"<<endl;
		clienthandle = it->second;
	}
	else
	{
		cout<<"没有找到handler!"<<endl;
	}	
	return clienthandle;
}