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
        void DeleteIDtoHandler(int clientid);
        void AddIPtoMap(uv_stream_t *client,char* IP);
        void InsertURLtoMap(char* userip, char* userurl);
        FindURL FindURLtoMap(char* userip);
        void DeleteURLtoMap(char* userip);

        uv_stream_t * GetUserhandlerValue(char* IP);  
        uv_stream_t* GetUserHandler(int clientid);
		StateandID FindMapValue(uv_stream_t *client);

    public:
        map<int, uv_stream_t*>  m_mapUserIDtoHandler;
        map<char *, uv_stream_t*>  m_mapIPtoHandler;
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
       	if(it->first!=userip)
       	{
       		findresult.result = 1;
       	}
       	else
       	{
       		findresult.result = 0;
       		findresult.findurl = it->second;
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

//与普通用户相关的MAP
void UserIDtoHandlerManager::AddIPtoMap(uv_stream_t *client, char* IP)      //添加用户id增。
{
	m_mapIPtoHandler.insert(pair<char*, uv_stream_t *>(IP, client));
	//m_mapIPtoHandler.insert(map<string, uv_stream_t *>::value_type(IP, client));
	cout<<"UserIP"<<IP<<"添加成功"<<endl;
}

uv_stream_t * UserIDtoHandlerManager::GetUserhandlerValue(char* IP)   //查找map中是否有客户端的句柄
{
	uv_stream_t *userhandler;
	map<char*,uv_stream_t*>::iterator it; 
	it = m_mapIPtoHandler.begin();
	while(it != m_mapIPtoHandler.end())
	{
		if(it->first!=IP)
		{
	  		userhandler = NULL;
		}
		else
		{
			userhandler = it->second;
		}
	    it ++;         
	}
	return userhandler;
} 
//与普通用户相关的MAP结束

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

