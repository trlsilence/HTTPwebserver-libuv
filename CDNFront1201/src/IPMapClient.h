#include <string> 
#include <time.h>
#include <iostream>  
#include <map>
#include "uv.h"

using namespace std;
struct FindIPHandler
{
	int result=0;
	uv_stream_t* user;
};

class IPMapManager
{
    public:
        static IPMapManager *Instance();//单例模式
        void AddIPtoMap(uv_stream_t *client,char* IP);
        void DeleteUserhandlerValue(char* ipaddress);
        FindIPHandler GetUserhandlerValue(char* IP);  	

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
    printf("\n-------------插入HTTP用户IP---------------\n");
	printf("HTTP用户句柄：%d\n",client);
	cout<<"IP地址为:"<<IP<<"的用户句柄添加成功"<<endl;

	map<char*, uv_stream_t*> aabb = usertohandler->m_mapIPtoHandler;
	map<char*, uv_stream_t*>::iterator itt; 
	for(itt = aabb.begin(); itt != aabb.end(); itt++)
	{
		printf("遍历Map中的记录：%s\n",itt->first);
		
	}
	printf("\n--------------用户IP插入---------------\n");
}

FindIPHandler IPMapManager::GetUserhandlerValue(char* IP)   //查找map中是否有客户端的句柄
{
	struct FindIPHandler findiphandler;
	map<char*,uv_stream_t*>::iterator it; 
	it = m_mapIPtoHandler.begin();
	while(it != m_mapIPtoHandler.end())
	{
		if(strcmp(it->first,IP)==0)
		{
	  		findiphandler.user = it->second;
	  		findiphandler.result = 1;
            cout<<"成功找到IP对应的handler"<<endl;
		}
		else
		{
	  		findiphandler.user = NULL;
	  		findiphandler.result = 0;
		}
	    it++;         
	}
	return findiphandler;
}

void IPMapManager::DeleteUserhandlerValue(char* ipaddress)//删除用户请求任务(id自减)。
{
	map<char*,uv_stream_t*>::iterator iter;
	iter= m_mapIPtoHandler.find(ipaddress);
	if(iter == m_mapIPtoHandler.end())
	{
	 	cout<<"在IPMapClient.h中没有找到该用户IP对应的句柄记录！"<<endl;
	}
	else
	{
	 	m_mapIPtoHandler.erase(ipaddress);
	 	cout<<iter->first<<"在IPMapClient.h中用户句柄成功删除"<<endl;
	}
}

/*
FindIPHandler IPMapManager::GetUserhandlerValue(char* IP)  //根据整型的ID获得用户的句柄
{
	struct FindIPHandler findiphandler;
	map<char*, uv_stream_t*>::iterator it;  
	it = m_mapIPtoHandler.find(IP);
	if(it != m_mapIPtoHandler.end())
	{
		cout<<"成功找到HTTP用户IP对应的handler!"<<endl;
		findiphandler.user = it->second;
		findiphandler.result = 1;
	}
	else
	{
		cout<<"没有找到handler!"<<endl;
		findiphandler.result = 0;
	}	
	return findiphandler;
}*/
//与普通用户相关的MAP结束
