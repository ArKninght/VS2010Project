#include "resource.h"
#include "BForm.h"

CBForm form1(ID_Form1);

void form1_load()
{
	// ���ÿؼ���ʼֵ

	
	form1.Control(ID_txtRate).TextSet(2.64);
	form1.Control(ID_txtDeposit).TextSet(10000);
	form1.Control(ID_txtGet).TextSet(100000);
	form1.Control(ID_labelResult).TextSet(TEXT(""));
}

void cmdCalc_Click()
{
	double x;       // ����
	double r;		// �껯����
	double xExp;	// Ԥ�ڱ���
	int i = 0;		// ��������

	// �ӿؼ�����û�����
	x = form1.Control(ID_txtDeposit).TextVal();
	r = form1.Control(ID_txtRate).TextVal() / 100; // ���û����뵥λת��Ϊ %
	xExp = Val(form1.Control(ID_txtGet).Text());
	// Text()Ϊ����ַ������׵�ַ����ʹ��Val��������õ��ַ���ת��Ϊdouble����
	// ע�⣬����ֱ��ǿ��ת��Text()����(double)Text()�ǽ���ַת��Ϊdouble��

	// �ж������Ƿ�Ϸ�
	if(x > xExp)
	{
		MsgBox(TEXT("Ԥ�����벻�ܵ��ڴ��Ŷ��"),
			TEXT("���벻�Ϸ�"), mb_OK,mb_IconExclamation); // �ַ���������Ҫ������TEXT��
		return;
	}

	// ����
	while(1)
	{
		x = x * (1 + r);
		i++;
		if(x>xExp)break;
	}

	// ��ʾ������ؼ� ID_labelResult
	form1.Control(ID_labelResult).TextSet(i);
}

void cmdExit_Click()
{
	form1.UnLoad();
}

void form1_unload(int pbCancel)
{
	if(MsgBox(TEXT("�Ƿ�ȷ���˳���"), TEXT("�˳�ѡ��"), 
		mb_YesNo, mb_IconQuestion) != idYes)
		*(int *)pbCancel = 1;

}

void txtInput_KeyPress(int keyAscii, int pbCancel)
{
	// ����deposit�����а��س�����ASCII��=13����ʾ������
	if(13==keyAscii)
		cmdCalc_Click();
	// ����deposit�����а�ESC����ASCII��=27����ʾ�˳�
	else if(27==keyAscii)
		cmdExit_Click();
	// ��deposit������ֻ��������ֵ��С���㣬���ţ��˸�8��
	else if( ! (keyAscii>='0' && keyAscii<='9' || '-'==keyAscii || '.'==keyAscii || 8==keyAscii))
		*(int*)pbCancel =1;
}

int main()
{
	form1.EventAdd(0, eForm_Load, form1_load); // �˺��������ڴ������֮ǰ 
	form1.EventAdd(ID_buttonCalc, eCommandButton_Click, cmdCalc_Click); 
	form1.EventAdd(ID_buttonExit, eCommandButton_Click, cmdExit_Click);
	form1.EventAdd(0, eForm_QueryUnload, form1_unload);
	form1.EventAdd(ID_txtDeposit, eKeyPress, txtInput_KeyPress);
	form1.EventAdd(ID_txtRate, eKeyPress, txtInput_KeyPress);
	form1.EventAdd(ID_txtGet, eKeyPress, txtInput_KeyPress);
	form1.IconSet(ID_Icon1);
	form1.Show();
	return 0;
}