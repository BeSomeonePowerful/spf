#include <iostream>
#include "comtypedef_vchar.h"

using namespace std;

char* appName="";
main()
{
	CStr uri="tcp://127.0.0.1:8000";
	cout<<"----cut----"<<endl;
	cout<<uri.c_str()<<endl;
	CStr type;
	uri.cut("://",type);
	cout<<"type="<<type.c_str()<<endl;

	cout<<"----nCat, bprintf----"<<endl;
	char buf[6];
	buf[0]='t';
	buf[1]='e';
	buf[2]=0;
	buf[3]='s';
	buf[4]='t';
	buf[5]=0;
	cout<<"orig string="<<uri.c_str()<<", size="<<uri.size()<<endl;
	uri.nCat(buf,5);
	ostrstream st;
	uri.bprint(st);
	cout<<"after string="<<st.str()<<", size="<<uri.size()<<endl;
	delete st.str();

	cout<<"----split----"<<endl;
	CStr ip="127.0.0.1";
	CStr ps[4];
	ip.split(ps, 4, ".");
	cout<<"orig string="<<ip.c_str()<<endl;
	cout<<"after string="<<ps[0].c_str()<<" "<<ps[1].c_str()<<" "<<ps[2].c_str()<<" "<<ps[3].c_str()<<endl;

	CStr domain="sina.com.cn";

}


