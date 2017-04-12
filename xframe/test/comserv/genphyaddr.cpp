#include <iostream>
using namespace std;

#define __MAX_HOSTID			0xFFFFF		//默认hostid共计主机1048575个（主机数量）
#define __MAX_TASKCONTAINERID   0xFFF		//默认TaskContainerID共计4096个（主机上的线程数量）
#define __MAX_TASKCONTAINERID_BIT   12		//默认TaskContainerID占用12bit/0xFFF

#define genPhyAddr(hostid, threadid, phyaddr) \
	   phyaddr=hostid; \
   phyaddr<<=__MAX_TASKCONTAINERID_BIT;\
   phyaddr+=threadid;
   
#define parsePhyAddr(phyaddr, hostid, threadid) \
	   hostid=phyaddr; \
   hostid>>=__MAX_TASKCONTAINERID_BIT;\
   threadid=phyaddr&__MAX_TASKCONTAINERID;   

main()
{
	int hostid=10;
	int instid=2;
	int phyaddr=0;
	genPhyAddr(hostid, instid, phyaddr);

	cout<<"HostID="<<hostid<<";InstID="<<instid<<";PhyAddr="<<phyaddr<<endl;

	hostid=0;
	instid=0;
	parsePhyAddr(phyaddr, hostid, instid);
	cout<<"PhyAddr="<<phyaddr<<";HostID="<<hostid<<";InstID="<<instid<<endl;

}



