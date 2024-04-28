# include <tchar.h>

struct Part
{
	int Money;
	TCHAR User[20];
};

class CRedPackage
{
public:
	//���캯��
	//���Ĭ��m/100Ԫ����5��;
	CRedPackage(int m=100, int n=5);
	//��������
	~CRedPackage();

	//���ú���Ľ�����
	//���óɹ�����True��ʧ�ܷ���False
	bool SetPackage(int m, int n);
	
	struct Part * GetptParts();	//��ô򿪺�����û��ĸ�����Ϣ
	int GetiMoneyTotal();		//��ú�����ܽ��		
	int GetMoneyOpened();		//��ú���ѱ���ȡ�Ľ��
	int GetiOpened();			//����ѱ��򿪵ĺ���ĸ���
	int GetiNum();				//��ú�����ܸ���

	int GetptMoney(int idNum);
	TCHAR * GetptUser(int idNum);

	//��һ����������ػ�õĽ���λ���֣�����¼�û�������user
	//δ�ɹ���ȡ��������Ѿ�����ʱ,����0
	int OpenOnePack(TCHAR * user);
private:
	int iMoneyTotal;
	int iNum;
	struct Part * ptParts;
	int iOpened;
};