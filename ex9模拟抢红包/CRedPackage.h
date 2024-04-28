# include <tchar.h>

struct Part
{
	int Money;
	TCHAR User[20];
};

class CRedPackage
{
public:
	//构造函数
	//红包默认m/100元，共5个;
	CRedPackage(int m=100, int n=5);
	//析构函数
	~CRedPackage();

	//设置红包的金额、个数
	//设置成功返回True，失败返回False
	bool SetPackage(int m, int n);
	
	struct Part * GetptParts();	//获得打开红包的用户的个人信息
	int GetiMoneyTotal();		//获得红包的总金额		
	int GetMoneyOpened();		//获得红包已被领取的金额
	int GetiOpened();			//获得已被打开的红包的个数
	int GetiNum();				//获得红包的总个数

	int GetptMoney(int idNum);
	TCHAR * GetptUser(int idNum);

	//开一个红包，返回获得的金额（单位：分），记录用户的名字user
	//未成功领取红包或红包已经领完时,返回0
	int OpenOnePack(TCHAR * user);
private:
	int iMoneyTotal;
	int iNum;
	struct Part * ptParts;
	int iOpened;
};