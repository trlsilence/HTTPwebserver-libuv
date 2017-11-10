#include <string> 
#include <time.h>
#include <iostream>  
#include <map>
#include "uv.h"

using namespace std;

class IPMapManager
{
    public:
        static IPMapManager *Instance();//单例模式
        void AddIPtoMap(uv_stream_t *client,char* IP);
        uv_stream_t* GetUserhandlerValue(char* IP);  	

    public:
        map<char*, uv_stream_t*>  m_mapIPtoHandler;

    private:
        static IPMapManager *s_IPMapManager;
};

IPMapManager *IPMapManager::s_IPMapManager = NULL;
IPMapManager *IPMapManager::Instance()//单例模式
{
    if(s_IPMapManager == NULL)
    {
        s_IPMapManager = new IPMapManager();
    }
    return  s_IPMapManager;
}

//与普通用户相关的MAP
void IPMapManager::AddIPtoMap(uv_stream_t *client, char* IP)      //添加用户id增。
{
	IPMapManager *usertohandler = IPMapManager::Instance();
	//usertohandler->m_mapIPtoHandler.insert(pair<char*, uv_stream_t*>(IP, client));
	//m_mapIPtoHandler.insert(map<string, uv_stream_t *>::value_type(IP, client));
	usertohandler->m_mapIPtoHandler[IP]=client;
	printf("××××××=我和呵呵666\n");
	printf("HTTP用户句柄：%d\n",client);
	cout<<"UserIP:"<<IP<<"添加成功"<<endl;

	map<char*, uv_stream_t*> aabb = usertohandler->m_mapIPtoHandler;
	map<char*, uv_stream_t*>::iterator itt; 
	for(itt = aabb.begin(); itt != aabb.end(); itt++)
	{
		printf("bianliheheheh:%s\n",itt->first);
		
	}
}

uv_stream_t* IPMapManager::GetUserhandlerValue(char* IP)   //查找map中是否有客户端的句柄
{
	uv_stream_t *userhandler;
	map<char*,uv_stream_t*>::iterator it; 
	it = m_mapIPtoHandler.begin();
	while(it != m_mapIPtoHandler.end())
	{
		if(strcmp(it->first,IP)==0)
		{
	  		userhandler = it->second;
            cout<<"成功找到IP对应的handler"<<endl;
		}
		else
		{
	  		userhandler = NULL;
		}
	    it++;         
	}
	return userhandler;
} 
//与普通用户相关的MAP结束
