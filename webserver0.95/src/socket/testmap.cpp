#include <string> 
#include <time.h>
#include <iostream>  
#include <map>
#include "uv.h"

int main()
{
    string userIp1 = "123.123.123";
    string userIp2 = "123.123.234";
    string userSrcUrl1 = "www.baidu.com";
    string userSrcUrl2 = "www.edu.com";
    UserTaskManager* UserTaskManager=UserTaskManager::Instance();
    UserTaskManager->AddUserTask(userIp1, userSrcUrl1);
    UserTaskManager->AddUserTask(userIp2, userSrcUrl2);
    UserTaskManager->DeleteUserTask(1);
}
