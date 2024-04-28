# include <string.h>
# include <stdlib.h>
# include <time.h>
# include "CRedPackage.h"

//初始化函数，对红包金额、个数、抢过的人以及已开过个数初始化
CRedPackage::CRedPackage(int m, int n)
{
	//初始化私有成员
	iMoneyTotal = 0;
	iNum = 0;
	iOpened = 0;
	ptParts = NULL;
	
	//利用srand产生随机数种子
	srand((unsigned int)time(0));
	SetPackage(m, n);
}

CRedPackage::~CRedPackage()
{
	//析构时释放指针空间,并将抢红包人数置零
	//不对金额和个数操作，因为在重新构造时会将金额和个数重新定义
	if(ptParts)
	{
		delete []ptParts;
		ptParts = NULL;
	}
	iOpened = 0;
}

bool CRedPackage::SetPackage(int m=100, int n=3)
{
	//总金额和红包个数不能为不小于0的数
	if(m<=0 || n<=0) return false;
	//判断该红包是否已经被创建，同一个红包不能被创建两次
	if(ptParts)
	{
		delete []ptParts;
		ptParts =NULL;
	}
	//判断红包个数是否大于红包金额，确保每个人能够获得的金额最小为1分
	if(n>m) return false;

	//以上判断都没有问题，则初始化红包信息
	iMoneyTotal = m;
	iNum = n;
	iOpened = 0;
	//开辟抢夺红包人信息存储的空间
	ptParts = new struct Part [n];
	//至此红包创建初始化成功，返回True
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
	//通过id号直接获取对应用户打开红包的信息
	if(idNum<0 || idNum>iOpened-1)
		return 0;		//返回值为0，说明输入的id非法
	else
		return ptParts[idNum].Money;
}

TCHAR * CRedPackage::GetptUser(int idNum)
{
	//通过id号直接获取对应用户打开红包的信息
	if(idNum<0 || idNum>iOpened-1)
		return 0;		//返回值为0，说明输入的id非法
	else
		return ptParts[idNum].User;
}

int CRedPackage::OpenOnePack(TCHAR * user)
{
	//判断红包状态是否可以被抢
	if(0==iMoneyTotal) return 0;	//红包尚未存有金额
	if(0==iNum) return -1;			//红包尚未分配个数
	
	int iMoneyRemain = iMoneyTotal - GetMoneyOpened();
	int iNumRemain = iNum - iOpened;
	int iGetMoney;

	if(0==iNumRemain)	//剩余红包个数为0
		return -2;		//返回值为2则说明红包已被抢完
	else if(1==iNumRemain)			//剩余红包个数为1
		iGetMoney = iMoneyRemain;	//则用户将红包内剩余金额全部取走
	else	//剩余红包个数大于2
	{
		//利用随机数原则，分配红包内剩余钱数（随机数种子在构造对象时已经创建）
		//获得钱数在1～剩余钱数-剩余红包个数+1 （即确保每个人在分到1分钱的前提下，每个人可以获得的最大金额）
		iGetMoney = rand() % (iMoneyRemain - iNumRemain + 1);
		if(0==iGetMoney) iGetMoney=1;
	}

	iOpened++;
	
	// 将此个被打开红包的信息存储到ptPart中
	ptParts[iOpened-1].Money = iGetMoney;
	_tcscpy(ptParts[iOpened-1].User, user); //_tcscpy(),将一个字符串赋值到另一个字符串

	//返回获得的钱数
	return iGetMoney;
}