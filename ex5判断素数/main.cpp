#include "resource.h"
#include "BForm.h"
#include <math.h>

CBForm form1(ID_IsPrimeForm);
void form1_load();
bool IsPrime(int num);
void buttonJudge_Click();
void buttonList100_Click();
void buttonListRange_Click();
void buttonExit_Click();
void form1_unload(int pbCancel);
void txtInput_KeyPress(int keyAscii, int pbCancel);
void txtMaxMin_KeyPress(int keyAscii, int pbCancel);

int main()
{
	form1.EventAdd(0, eForm_Load, form1_load);
	form1.EventAdd(0, eForm_QueryUnload, form1_unload);
	form1.EventAdd(ID_buttonJudge, eCommandButton_Click, buttonJudge_Click);
	form1.EventAdd(ID_buttonList100, eCommandButton_Click, buttonList100_Click);
	form1.EventAdd(ID_buttonListRange, eCommandButton_Click, buttonListRange_Click);
	form1.EventAdd(ID_buttonExit, eCommandButton_Click, buttonExit_Click);
	form1.EventAdd(ID_txtInput, eKeyPress, txtInput_KeyPress);
	form1.EventAdd(ID_txtMax, eKeyPress, txtMaxMin_KeyPress);
	form1.EventAdd(ID_txtMin, eKeyPress, txtMaxMin_KeyPress);
	form1.IconSet(ID_Icon1);
	form1.Show();
	return 0;
}
/*
��  �ã��ڴ������ʱ��һЩ�ؼ����ݡ�����Ƚ�������
��  ������
����ֵ����
*/
void form1_load()
{
	//�����ı�Ϊ��
	form1.Control(ID_txtInput).TextSet(TEXT("0"));
	form1.Control(ID_txtResult).TextSet(TEXT(""));
	form1.Control(ID_txtMax).TextSet(TEXT("0"));
	form1.Control(ID_txtMin).TextSet(TEXT("0"));
	//���������С
	form1.Control(ID_Label).FontSizeSet(10);
	form1.Control(ID_txtInput).FontSizeSet(15);
	form1.Control(ID_buttonJudge).FontSizeSet(10);
	form1.Control(ID_txtResult).FontSizeSet(15);
	form1.Control(ID_buttonList100).FontSizeSet(10);
	form1.Control(ID_buttonListRange).FontSizeSet(10);
	form1.Control(ID_txtMax).FontSizeSet(15);
	form1.Control(ID_txtMin).FontSizeSet(15);
	form1.Control(ID_buttonExit).FontSizeSet(10);
	form1.Control(ID_list).FontSizeSet(10);
}

/*
��  �ã�����ر�ʱ�Դ����һЩ����
��  ������
����ֵ����
*/
void form1_unload(int pbCancel)
{
	if(MsgBox(TEXT("�Ƿ�ȷ���˳���"), TEXT("�˳�ѡ��"), 
		mb_YesNo, mb_IconQuestion) != idYes)
		*(int *)pbCancel = 1;
}

/*
��  �ã��ж�һ�����Ƿ�Ϊ����
��  ����num:���������
����ֵ��True:��������False:��������
*/
bool IsPrime(int num)
{
	if(0 == num || 1 == num)
		return false;// ����Ϊ0����Ϊ������
	int i, k;
	k = (int)sqrt((double) num); // ������������Сʱ�临�Ӷ�
	for(i = 2; i <= k; i++)
		if(num % i == 0) break;
	// �ж�i���ж�num�Ƿ�Ϊ����
	if(i > k) 
		return true;
	else 
		return false;
}

/*
��  �ã����¡��ж������������ı�������ʾ�������Ƿ�Ϊ����
��  ������
����ֵ����
*/
void buttonJudge_Click()
{
	int input = (int)form1.Control(ID_txtInput).TextVal();	// ��ȡ��Ҫ�жϵ�����
	if (IsPrime(input))										// �ж����������ı�������ʾ���
		form1.Control(ID_txtResult).TextSet(TEXT("����"));
	else
		form1.Control(ID_txtResult).TextSet(TEXT("������"));
}

/*
��  �ã����°������г���2��ʼ��100������
��  ������
����ֵ����
*/
void buttonList100_Click()
{
	int count = 0; // ����ֵ����¼��ʾ�������ĸ���
	form1.Control(ID_list).ListClear();// ���������ʾ�б�
	for(int i = 2; count < 100; i++){ 
		if(IsPrime(i)){
			form1.Control(ID_list).AddItem(Str(i));
			count++;
		}
	}
}

/*
��  �ã����°������ж����뷶Χ�Ƿ�Ϸ����г���Χ�ڵ���������
��  ������
����ֵ����
*/
void buttonListRange_Click()
{
	int min_num, max_num;
	// ���������ʾ�б�
	form1.Control(ID_list).ListClear();
	// ��ȡ��Χ
	min_num = (int) form1.Control(ID_txtMin).TextVal();
	max_num = (int) form1.Control(ID_txtMax).TextVal();
	// �ж�����ֵ�Ƿ�Ϸ�
	if(min_num >= max_num){
		MsgBox(TEXT("�������ֵ���������Сֵ��"), TEXT("���벻�Ϸ�"), mb_OK, mb_IconError);
		form1.Control(ID_txtMin).TextSet(TEXT("0"));
		form1.Control(ID_txtMax).TextSet(TEXT("0"));
		return;
	}
	// �г���Χ�ڵ���������
	for(int i = min_num; i <= max_num; i++){
		if(IsPrime(i)){
			form1.Control(ID_list).AddItem(Str(i));
		}
	}
}

/*
��  �ã������˳������رմ���
��  ������
����ֵ����
*/
void buttonExit_Click()
{
	form1.UnLoad();
}

/*
��  �ã������¼�������������text���ı���������
��  ����keyAscii:���밴����Ascii��, pbCancel:�Ƿ�ȡ���������жϲ���
����ֵ����
*/
void txtInput_KeyPress(int keyAscii, int pbCancel)
{
	// ����Input�����а��س�����ASCII��=13����ʾ�жϽ��
	if(13 == keyAscii)
		buttonJudge_Click();
	// ����deposit�����а�ESC����ASCII��=27����ʾ�˳�
	else if(27==keyAscii)
		buttonExit_Click();
	// ��deposit������ֻ��������ֵ���˸�8��
	else if( ! (keyAscii>='0' && keyAscii<='9' || 8==keyAscii))
		*(int*)pbCancel =1;
}

void txtMaxMin_KeyPress(int keyAscii, int pbCancel)
{
	// ��deposit������ֻ��������ֵ���˸�8��
	if( ! (keyAscii>='0' && keyAscii<='9' || 8==keyAscii))
		*(int*)pbCancel =1;
}