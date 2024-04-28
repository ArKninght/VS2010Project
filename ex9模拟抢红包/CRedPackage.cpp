# include <string.h>
# include <stdlib.h>
# include <time.h>
# include "CRedPackage.h"

//��ʼ���������Ժ�������������������Լ��ѿ���������ʼ��
CRedPackage::CRedPackage(int m, int n)
{
	//��ʼ��˽�г�Ա
	iMoneyTotal = 0;
	iNum = 0;
	iOpened = 0;
	ptParts = NULL;
	
	//����srand�������������
	srand((unsigned int)time(0));
	SetPackage(m, n);
}

CRedPackage::~CRedPackage()
{
	//����ʱ�ͷ�ָ��ռ�,�����������������
	//���Խ��͸�����������Ϊ�����¹���ʱ�Ὣ���͸������¶���
	if(ptParts)
	{
		delete []ptParts;
		ptParts = NULL;
	}
	iOpened = 0;
}

bool CRedPackage::SetPackage(int m=100, int n=3)
{
	//�ܽ��ͺ����������Ϊ��С��0����
	if(m<=0 || n<=0) return false;
	//�жϸú���Ƿ��Ѿ���������ͬһ��������ܱ���������
	if(ptParts)
	{
		delete []ptParts;
		ptParts =NULL;
	}
	//�жϺ�������Ƿ���ں����ȷ��ÿ�����ܹ���õĽ����СΪ1��
	if(n>m) return false;

	//�����ж϶�û�����⣬���ʼ�������Ϣ
	iMoneyTotal = m;
	iNum = n;
	iOpened = 0;
	//��������������Ϣ�洢�Ŀռ�
	ptParts = new struct Part [n];
	//���˺��������ʼ���ɹ�������True
	return true;
}

struct Part * CRedPackage::GetptParts()
{
	return ptParts;
}

int CRedPackage::GetiMoneyTotal()
{
	return iMoneyTotal;
}

int CRedPackage::GetMoneyOpened()
{
	int MoneyOpened=0;
	if(0==iOpened)
		return 0;
	else
	{
		for(int i=0;i<iOpened;i++)
		{
			MoneyOpened+=ptParts[i].Money;
		}
	}
	return MoneyOpened;
}

int CRedPackage::GetiOpened()
{
	return iOpened;
}

int CRedPackage::GetiNum()
{
	return iNum;
}

int CRedPackage::GetptMoney(int idNum)
{
	//ͨ��id��ֱ�ӻ�ȡ��Ӧ�û��򿪺������Ϣ
	if(idNum<0 || idNum>iOpened-1)
		return 0;		//����ֵΪ0��˵�������id�Ƿ�
	else
		return ptParts[idNum].Money;
}

TCHAR * CRedPackage::GetptUser(int idNum)
{
	//ͨ��id��ֱ�ӻ�ȡ��Ӧ�û��򿪺������Ϣ
	if(idNum<0 || idNum>iOpened-1)
		return 0;		//����ֵΪ0��˵�������id�Ƿ�
	else
		return ptParts[idNum].User;
}

int CRedPackage::OpenOnePack(TCHAR * user)
{
	//�жϺ��״̬�Ƿ���Ա���
	if(0==iMoneyTotal) return 0;	//�����δ���н��
	if(0==iNum) return -1;			//�����δ�������
	
	int iMoneyRemain = iMoneyTotal - GetMoneyOpened();
	int iNumRemain = iNum - iOpened;
	int iGetMoney;

	if(0==iNumRemain)	//ʣ��������Ϊ0
		return -2;		//����ֵΪ2��˵������ѱ�����
	else if(1==iNumRemain)			//ʣ��������Ϊ1
		iGetMoney = iMoneyRemain;	//���û��������ʣ����ȫ��ȡ��
	else	//ʣ������������2
	{
		//���������ԭ�򣬷�������ʣ��Ǯ��������������ڹ������ʱ�Ѿ�������
		//���Ǯ����1��ʣ��Ǯ��-ʣ��������+1 ����ȷ��ÿ�����ڷֵ�1��Ǯ��ǰ���£�ÿ���˿��Ի�õ�����
		iGetMoney = rand() % (iMoneyRemain - iNumRemain + 1);
		if(0==iGetMoney) iGetMoney=1;
	}

	iOpened++;
	
	// ���˸����򿪺������Ϣ�洢��ptPart��
	ptParts[iOpened-1].Money = iGetMoney;
	_tcscpy(ptParts[iOpened-1].User, user); //_tcscpy(),��һ���ַ�����ֵ����һ���ַ���

	//���ػ�õ�Ǯ��
	return iGetMoney;
}