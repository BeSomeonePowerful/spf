#ifndef _SIPSTACKPSA_DIALOG
#define _SIPSTACKPSA_DIALOG

#include "cexpirebuffer.h"
#include "cmd5.h"
#include <ctime>

#include "msgdatadef.h"
#include "msgdef_sip.h"

class CSIPPsaDialogInfo
{
public:
	CSIPPsaDialogInfo() {};
	CStr			mDialogID_F;	//����ID
	CStr			mDialogID_B;	//����ID
	TMsgAddress 	mAddr;			//toAddr
	void setDialogtAddr(TMsgAddress& taddr) { mAddr = taddr;};
};

class CSIPPsaDialog
{
private:
	//���Ƶ�����¼���� 0��Ĭ��û������
	//���ټ�¼�Ĳ��� Ĭ��0�����Զ����ٹ��ڼ�¼��1���Զ����ٹ��ڼ�¼������������¼������ʱ�򣬴����ϵĿ�ʼ����
	//����ʱ�䣬��λ�룬Ĭ��172800�룻�����һ��ˢ�µ�ʱ�䳬����һʱ�䳤�ȣ�currenttime-refreshtime > m_living_time���������
	CExpireBuffer<CStr, CSIPPsaDialogInfo>  mDialogMap_F;
	CExpireBuffer<CStr, CStr> 				mDialogMap_B;

public:
	CSIPPsaDialog(unsigned int max_count=0, bool destroy_old=false, unsigned int living_time=172800)
	{
		setMaxCount(max_count);
		setLivingTime(destroy_old);
		setDestroyPolicy(living_time);
	};

	void setMaxCount(unsigned int max_count=0) { mDialogMap_F.setMaxCount(max_count);  mDialogMap_B.setMaxCount(max_count);};
	void setLivingTime(unsigned int living_time=172800) { mDialogMap_F.setLivingTime(living_time);  mDialogMap_B.setLivingTime(living_time);};
	void setDestroyPolicy(bool destroy_old=false) { mDialogMap_F.setDestroyPolicy(destroy_old); mDialogMap_B.setDestroyPolicy(destroy_old);};

	//������ܹ�����������Ŀ����
	int getMaxSize() {return mDialogMap_F.getMaxSize();};
	BOOL getDestoryPolicy() {return mDialogMap_F.getDestoryPolicy();};
	int getLivingTime() {return mDialogMap_F.getLivingTime();};
	
	//��ѯ�Ի�
	BOOL QueryDialogTab(CStr& dialogid, CSIPPsaDialogInfo &dialoginfo);
	//�ж��Ƿ���ڶԻ�
	BOOL ExistDialogTab(CStr& dialogid);
	//ɾ���Ի�
	void RemoveDialogTab(CStr& dialogid);
	//����Ի�
	BOOL InsertDialogTab(CStr& dialogid_F, CStr& dialogid_B);
	BOOL InsertDialogTab(CStr& dialogid_F, CStr& dialogid_B, TMsgAddress& addr);
	//���¶Ի�״̬
	BOOL UpdateDialogAddr(CStr& dialogid, TMsgAddress& tAddr);
	//���¶Ի�������ʱ�䣬һ���ڽ��յ���Ϣ��ʱ��ʹ��
	BOOL UpdateDialogExpire(CStr& dialogid, time_t currenttime=0);
	//ɾ�����ڶ���
	INT CleanExpireDialog(time_t currenttime=0);

	INT Size();
};

#endif
