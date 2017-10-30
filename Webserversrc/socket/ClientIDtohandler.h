#include <string> 
#include <time.h>
#include <iostream>  
#include <map>
#include "uv.h"

using namespace std;
int returnid;
struct StateandID
{
	int result;
	int client_id;
};
//***************管理维护到的MAP*******************//
class UserIDtoHandlerManager
{
    public:
        static UserIDtoHandlerManager *Instance();//单例模式
        int AddIDtoHandler(uv_stream_t *client);
        void DeleteIDtoHandler(int clientid);
        uv_stream_t* GetUserHandler(int clientid);
	StateandID FindMapValue(uv_stream_t *client);

    private:
        map<int, uv_stream_t*>  m_mapUserIDtoHandler;
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

int UserIDtoHandlerManager::AddIDtoHandler(uv_stream_t *client)//添加用户请求任务，id增。
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
	uv_stream_t *clienthandle = it->second;
	return clienthandle;
}

StateandID UserIDtoHandlerManager::FindMapValue(uv_stream_t *client)   //查找map中是否有客户端的句柄
{
	struct StateandID stateandid;
	map<int,uv_stream_t*>::iterator it; 
	it = m_mapUserIDtoHandler.begin();
	while(it != m_mapUserIDtoHandler.end())
	{
		if(it->second==client)
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

