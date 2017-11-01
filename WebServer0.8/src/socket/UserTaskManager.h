#include <string> 
#include <time.h>
#include <iostream>  
#include <map>
#include "uv.h"

using namespace std;

//***************     *******************//
class UserIDtoHandlerManager
{
    public:
        static UserIDtoHandlerManager *Instance();//单例模式
        void AddIDtoHandler(uv_stream_t *client);
        void DeleteIDtoHandler(int clientid);
        static uv_stream_t* GetUserHandler(int clientid);

    private:
        map<int, uv_stream_t*>  m_mapUserIDtoHandler;
        static UserIDtoHandlerManager *s_UserIDtoHandlerManager; //danlihua
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

void UserIDtoHandlerManager::AddIDtoHandler(uv_stream_t *client)//添加用户请求任务，id增。
{
    static int clientid = 1;
    cout<<"new clienthandler :"<<client<<endl;
    m_mapUserIDtoHandler[clientid] = client;
    cout<<"任务——"<<clientid<<"添加成功"<<endl;
    ++clientid;
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

uv_stream_t* UserIDtoHandlerManager::GetUserHandler(int clientid)
{
    map<int,uv_stream_t*>::iterator it;  
    it = m_mapUserIDtoHandler.find(UserTaskID);
    uv_stream_t *clienthandle = it->second;
    return clienthandle;
}
//-----------------------------------------------------------------------------------------------------------------------------------
/*
int main()
{
    string userIp1 = "123.123.123";
    string userIp2 = "123.123.234";
    string userSrcUrl1 = "www.baidu.com";
    string userSrcUrl2 = "www.edu.com";
    UserIDtoHandlerManager* UserIDtoHandlerManager=UserIDtoHandlerManager::Instance();
    UserIDtoHandlerManager->AddUserTask(userIp1, userSrcUrl1);
    UserIDtoHandlerManager->AddUserTask(userIp2, userSrcUrl2);
    UserIDtoHandlerManager->DeleteUserTask(1);
}
*/



