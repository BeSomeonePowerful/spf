#include "clist.h"
#include <iostream>

using namespace std;

class A
{
public:
	int a;
	int b;
};

class B
{
public:
	CList<A> mList;
	int c;
	B() {}
	~B() {}
};

main()
{
	CList<B>  mList1;
	CList<B*> mList2;
	CList<B>  mList3;

	for(int i=0;i<3;i++)
	{
		B* b=new B();
		B  bx;
		A a;
		a.a=i;
		a.b=i;
		b->mList.push_back(a);
		b->c=i;
		bx.c=i;
		bx.mList.push_back(a);

		B by=bx;
		
		mList1.push_back(bx);
		mList2.push_back(b);
		mList3.push_back(by);
	}

	cout<<"A.count="<<mList1.count()<<";B.count="<<mList2.count()<<";C.count="<<mList3.count()<<endl;
       for(CList<B>::iterator iter=mList1.begin(); iter!=mList1.end(); iter++)
        {
                A ta;
                (*iter).mList.front(ta);
                cout<<"Bx.c="<<(*iter).c<<";Bx.list.a="<<ta.a<<endl;
        }

      for(CList<B>::iterator iter=mList3.begin(); iter!=mList3.end(); iter++)
        {
                A ta;
                (*iter).mList.front(ta);
                cout<<"By.c="<<(*iter).c<<";By.list.a="<<ta.a<<endl;
        }

	for(CList<B*>::iterator iter=mList2.begin(); iter!=mList2.end(); iter++)
	{
		A ta;
		(*iter)->mList.front(ta);
		cout<<"B.c="<<(*iter)->c<<";B.list.a="<<ta.a<<endl;		
	}

	mList2.reset_d();
	while(mList2.next())
	{
		B* b;
		mList2.current(b);
		if(b)
		{
			cout<<"delete B.c="<<b->c<<endl;
			b->mList.remove();
			 delete b;

		}
	}
	mList2.remove();	
}
