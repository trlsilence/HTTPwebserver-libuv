#include <string> 
#include <time.h>
#include <iostream>  
#include <map>
#include "uv.h"

using namespace std;
int returnid;
string returncdn;
string returnIP;

struct StateandID
{
	int result = 0;
	int client_id = 0;
	string cdnid = "0";
};

struct FindURL
{
	int result;
	char* findurl;
};

struct FindClient
{
	int result = 0;
	int jieguo = 0;
	uv_stream_t* sendclient;
};
//***************管理维护到的MAP*******************//
class UserIDtoHandlerManager
{
    public:
        static UserIDtoHandlerManager *Instance();//单例模式
        string AddCDNIDtoHandler(const string &cdnid, uv_stream_t *client);
        int AddManagerIDtoHandler(uv_stream_t *client);
        void InsertURLtoMap(char* userip, char* userurl);
        FindURL FindURLtoMap(char* userip);
        void DeleteURLtoMap(char* userip);
        int FindCDNHandler(uv_stream_t *client);
        int FindManagerHandler(uv_stream_t *client1);
        void DeleteManagerHandler(int clientid);
        void DeleteCDNHandler(const string &clientid);

        FindClient LookUpManagerMap();
        FindClient GetCDNHandler(const string &clientid);
        FindClient GetManagerHandler(int clientid);
		StateandID FindManagerMapValue(uv_stream_t *client);
		StateandID FindCDNMapValue(uv_stream_t *client);

    public:
        map<string, uv_stream_t*>  m_mapCDNIDtoHandler;
        map<int, uv_stream_t*>  m_mapManagerIDtoHandler;
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

//CDN related start
string UserIDtoHandlerManager::AddCDNIDtoHandler(const string &cdnid, uv_stream_t *client)//添加用户id。
{ 
    m_mapCDNIDtoHandler[cdnid] = client;
    cout<<"CDN 客户端:"<<cdnid.c_str()<<" 添加成功"<<endl;
    returncdn = cdnid;
    return returncdn;
}

FindClient UserIDtoHandlerManager::GetCDNHandler(const string &clientid)  //根据整型的ID获得用户的句柄
{
	struct FindClient findclient;
	map<string,uv_stream_t*>::iterator it;  
	it = m_mapCDNIDtoHandler.find(clientid);
	uv_stream_t *clienthandle;
	if(it!=m_mapCDNIDtoHandler.end())
	{
		cout<<"成功找到CDN handler!"<<endl;
		findclient.sendclient = it->second;
		findclient.result = 1;
	}
	else
	{
		cout<<"没有找到CDN handler!"<<endl;
		clienthandle = 0;
	}	
	return findclient;
}

int UserIDtoHandlerManager::FindCDNHandler(uv_stream_t *client)
{
	int findcdnresult = 0;
	map<string,uv_stream_t*>::iterator it; 
	it = m_mapCDNIDtoHandler.begin();
	while(it != m_mapCDNIDtoHandler.end())
	{
		if(it->second!=client)
		{
	  		//findcdnresult = 0;
		}
		else
		{
			findcdnresult = 1;
		}
	    it++;         
	}
	return findcdnresult;
}

StateandID UserIDtoHandlerManager::FindCDNMapValue(uv_stream_t *client)   //查找map中是否有客户端的句柄
{
	struct StateandID stateandid;
	map<string,uv_stream_t*>::iterator it; 
	it = m_mapCDNIDtoHandler.begin();
	while(it != m_mapCDNIDtoHandler.end())
	{
		if(it->second == client)
		{
			stateandid.result = 1;
			stateandid.cdnid = it->first;
		}
	    it++;         
	}
	return stateandid;
}

void UserIDtoHandlerManager::DeleteCDNHandler(const string &clientid)//删除用户请求任务(id自减)。
{
	map<string, uv_stream_t*>::iterator iter;
	iter= m_mapCDNIDtoHandler.find(clientid);
	if(iter == m_mapCDNIDtoHandler.end())
	{
	 	cout<<"没有找到该CDN"<<endl;
	}
	else
	{
	 	m_mapCDNIDtoHandler.erase(clientid);
	 	cout<<iter->first<<"CDN句柄成功删除"<<endl;
	}
}

//CDN相关结束

//后端相关开始
int UserIDtoHandlerManager::AddManagerIDtoHandler(uv_stream_t *client)//添加用户id。
{ 
    static int clientid = 1;
    //cout<<"new clienthandler :"<<client<<endl;
    m_mapManagerIDtoHandler[clientid] = client;
    cout<<"Manager客户端"<<clientid<<"添加成功"<<endl;
    //printf("客户端句柄是：%d\n",client);
    returnid = clientid;
    ++clientid;
    return returnid;
}

FindClient UserIDtoHandlerManager::GetManagerHandler(int clientid)  //根据整型的ID获得用户的句柄
{
	struct FindClient findclient;
	map<int,uv_stream_t*>::iterator it;  
	it = m_mapManagerIDtoHandler.find(clientid);
	uv_stream_t *clienthandle;
	if(it!= m_mapManagerIDtoHandler.end())
	{
		cout<<"成功找到Manager句柄!"<<endl;
		findclient.sendclient = it->second;
		findclient.result = 1;
	}
	else
	{
		cout<<"没有找到Manager句柄!"<<endl;
		findclient.result = 0;
	}	
	return findclient;
}

int UserIDtoHandlerManager::FindManagerHandler(uv_stream_t *client1)
{
	int findcdnresult = 0;
	int temp = 0;
	map<int,uv_stream_t*>::iterator it; 
	it = m_mapManagerIDtoHandler.begin();
	while(it != m_mapManagerIDtoHandler.end())
	{
		if(it->second!=client1)
		{
	  		//findcdnresult = 0;
		}
		else
		{
			findcdnresult = 1;
			cout<<"成功找到Manager句柄！"<<endl;
		}
	    it++;         
	}
	return findcdnresult;
}

StateandID UserIDtoHandlerManager::FindManagerMapValue(uv_stream_t *client)   //查找map中是否有客户端的句柄
{
	struct StateandID stateandid;
	map<int,uv_stream_t*>::iterator it; 
	it = m_mapManagerIDtoHandler.begin();
	while(it != m_mapManagerIDtoHandler.end())
	{
		if(it->second == client)
		{
			stateandid.result = 1;
			stateandid.client_id = it->first;
		}
	    it++;         
	}
	return stateandid;
}

void UserIDtoHandlerManager::DeleteManagerHandler(int clientid)//删除用户请求任务(id自减)。
{
	map<int, uv_stream_t*>::iterator iter;
	iter= m_mapManagerIDtoHandler.find(clientid);
	if(iter == m_mapManagerIDtoHandler.end())
	{
	 	cout<<"没有找到该Manager"<<endl;
	 	//return 0;
	}
	else
	{
	 	m_mapManagerIDtoHandler.erase(clientid);
	 	cout<<iter->first<<"Manager句柄成功删除"<<endl;
	}
}
//后端相关结束

//遍历Manager句柄存放的MaP
FindClient UserIDtoHandlerManager::LookUpManagerMap()
{
	struct FindClient lookupclient;
	int mum=0;
	UserIDtoHandlerManager *usertohandler = UserIDtoHandlerManager::Instance();  //单例模式
	//Map遍历开始
	cout<<"\n**********开始遍历存放Manager句柄的MAP*************\n"<<endl;
	map<int,uv_stream_t*> idhandler = usertohandler->m_mapManagerIDtoHandler;
	map<int,uv_stream_t*>::iterator it;  
	for(it = idhandler.begin(); it != idhandler.end(); it++)
	{
		++mum;
		printf("分配的标识：%d      ",it->first);
		printf("句柄：%d      \n",it->second);
		lookupclient.jieguo = 1;			  	
	}
	if(mum==0)
	{
		lookupclient.result = 0;
		lookupclient.jieguo = 0;
	}
	else
	{
			lookupclient.result = idhandler.begin()->first; 
			lookupclient.sendclient = idhandler.begin()->second;
	}
	printf("Map中的记录数为：%d\n",mum);
	return lookupclient;
}