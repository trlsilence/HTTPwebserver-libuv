#include "transport_proto.h"
#include <iostream>
#include <string>


using namespace std;
using namespace transport_proto;

int main()
{
	Serialize_head obj_Serialize;
	//char *a = (char*)calloc(256,sizeof(char*));
	string a;
	a=obj_Serialize.Serialhead(obj_Serialize, 1.0, 6, 1, 0, 5);
	struct heads headpp;
	headpp = obj_Serialize.Parsehead(obj_Serialize, a);	
	cout<<"version:"<<headpp.version<<endl;
	//cout<<"result:"<<a<<endl;
	return 0;
}



