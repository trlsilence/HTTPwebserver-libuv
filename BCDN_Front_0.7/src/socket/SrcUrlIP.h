#include <string> 
#include <iostream>  
#include <map>

using namespace std;

class SrcUrlIp    
{
    public:
        static SrcUrlIp *Instance();//单例化
        void AddUrlIP(const string &m_SrcUrl, const string &m_SrcIp);
        string GetIP(const string &m_SrcUrl);

    private:    
        static SrcUrlIp *s_SrcUrlIp;
        map<string,string> m_mapUrlIP;//用于回源查找
};

SrcUrlIp  *SrcUrlIp::s_SrcUrlIp = NULL;
SrcUrlIp  *SrcUrlIp::Instance()
{
    if(s_SrcUrlIp == NULL)
    {
        s_SrcUrlIp = new  SrcUrlIp();
    }
    return  s_SrcUrlIp;
}

void SrcUrlIp::AddUrlIP(const string &m_SrcUrl, const string &m_SrcIp)//添加url->IP的对应
{
    m_mapUrlIP[m_SrcUrl]=m_SrcIp;
}


string SrcUrlIp::GetIP(const string &m_SrcUrl)//根据url查询IP
{
     if(m_mapUrlIP.count(m_SrcUrl) )
     {
         string m_SrcIp=m_mapUrlIP[m_SrcUrl];
         return m_SrcIp;
     }
     else
     { 
         return "";
     }
}
/*
int main()
{
    string  m_strSrcUrl1="baidu.com/config/a.txt";
    string  m_srcIP="139.25.23";
    SrcUrlIp* SrcUrlIp =  SrcUrlIp::Instance();
    SrcUrlIp->AddUrlIP(m_strSrcUrl1, m_srcIP);
    string ss = SrcUrlIp->GetIP(m_strSrcUrl1);
    cout<<ss<<endl;

}
*/

