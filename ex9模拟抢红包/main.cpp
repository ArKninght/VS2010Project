#include "resource.h"
#include "BForm.h"
#include "CRedPackage.h"
#include <string.h>

CBForm form1(ID_Form1);
CRedPackage rpA(666, 6);
CRedPackage rpB, rpC;

void form1_load();
void ButtonOpenPack1_Click();
void ButtonOpenPack2_Click();
void ButtonOpenPack3_Click();
void ButtonSendPack_Click();
void ButtonCheck1_Click();
void ButtonCheck2_Click();
void ButtonCheck3_Click();
void TextPackMoney_KeyPress(int keyAscii, int pbCancel);
void TextPackNum_KeyPress(int keyAscii, int pbCancel);
void OpenRedPackage(unsigned short idResControl, CRedPackage & rpTemp);
void InfoList(CRedPackage & rpTemp);
int UserCheck(unsigned short idResControl, CRedPackage & rpTemp);

int main()
{
	form1.EventAdd(0, eForm_Load, form1_load);
	form1.EventAdd(ID_ButtonOpenPack1, eCommandButton_Click, ButtonOpenPack1_Click);
	form1.EventAdd(ID_ButtonOpenPack2, eCommandButton_Click, ButtonOpenPack2_Click);
	form1.EventAdd(ID_ButtonOpenPack3, eCommandButton_Click, ButtonOpenPack3_Click);
	form1.EventAdd(ID_ButtonSendPack, eCommandButton_Click, ButtonSendPack_Click);
	form1.EventAdd(ID_ButtonCheck1, eCommandButton_Click, ButtonCheck1_Click);
	form1.EventAdd(ID_ButtonCheck2, eCommandButton_Click, ButtonCheck2_Click);
	form1.EventAdd(ID_ButtonCheck3, eCommandButton_Click, ButtonCheck3_Click);
	form1.EventAdd(ID_TextPackMoney, eKeyPress, TextPackMoney_KeyPress);
	form1.EventAdd(ID_TextPackNum, eKeyPress, TextPackNum_KeyPress);
	form1.IconSet(ID_Icon1);
	form1.Show();
	return 0;
}

void form1_load()
{
	//װ��ͼƬ Ŀǰ��δ��ʼ
	//���ú��C�İ���Ϊ��ɫ����ΪC��δ��Ǯ
	form1.Control(ID_ButtonOpenPack3).EnabledSet(false);
	//��������һ�����B
	rpB.OpenOnePack(TEXT("������2"));
}

// ���ù��캯����������������
void ButtonOpenPack1_Click()
{
	OpenRedPackage(ID_TextUser1, rpA);
}

// �޲������룬Ĭ�Ϲ�����
void ButtonOpenPack2_Click()
{
	OpenRedPackage(ID_TextUser2, rpB);
}

void ButtonOpenPack3_Click()
{
	OpenRedPackage(ID_TextUser3, rpC);
}

void ButtonSendPack_Click()
{
	int iSetMoney = (int)(form1.Control(ID_TextPackMoney).TextVal()*100);
	int iSetNum = (int)form1.Control(ID_TextPackNum).TextVal();
	if(MsgBox(StrAppend(TEXT("ȷ�Ϸ��ͺ����\n�����"), Str(iSetMoney/100.0), 
		TEXT("Ԫ\n���������"), Str(iSetNum)),TEXT("ȷ��"), mb_OkCancel)!=idOk)
	{
		MsgBox(TEXT("��ȡ�����ͣ�"), TEXT("��ʾ"), mb_OK, mb_IconInformation);
		return;
	}
	else
		MsgBox(TEXT("����ѷ��ͣ�"), TEXT("��ʾ"), mb_OK, mb_IconInformation);
	rpC.SetPackage(iSetMoney, iSetNum);
	form1.Control(ID_TextPackMoney).TextSet(Str(iSetMoney/100.0));
	form1.Control(ID_ButtonOpenPack3).EnabledSet(true);
}

void ButtonCheck1_Click()
{
	InfoList(rpA);
}

void ButtonCheck2_Click()
{
	InfoList(rpB);
}

void ButtonCheck3_Click()
{
	InfoList(rpC);
}

void TextPackMoney_KeyPress(int keyAscii, int pbCancel)
{
	//������Ϊ0��9��С���㡢�˸�֮���������ȡ����������
	if(!(keyAscii>='0' && keyAscii<='9' || keyAscii=='.'|| 8==keyAscii))
		*(int*)pbCancel = 1;
}

void TextPackNum_KeyPress(int keyAscii, int pbCancel)
{
	//������Ϊ0��9���˸�֮���������ȡ����������
	if(!(keyAscii>='0' && keyAscii<='9' || 8==keyAscii))
		*(int*)pbCancel = 1;
}

void OpenRedPackage(unsigned short idResControl, CRedPackage & rpTemp)
{
	TCHAR * tcUser = form1.Control(idResControl).Text();
	if(TEXT('\0')==*tcUser)
	{
		MsgBox(TEXT("�������û����ƣ�"), TEXT("�û�������"), mb_OK, mb_IconExclamation);
		return;
	}
	int uc = UserCheck(idResControl, rpTemp);
	if(0==uc)
	{
		MsgBox(TEXT("�����û����Ƴ����������20���ַ��ڣ�"),TEXT("����"), mb_OK, mb_IconError);
		return;
	}
	else if(1==uc)
	{
		MsgBox(TEXT("�����û����ظ������������룡"), TEXT("����"), mb_OK, mb_IconError);
		return;
	}
	int iGet = rpTemp.OpenOnePack(tcUser);
	if(0==iGet)			//����ֵΪ0��������޽��
		MsgBox(TEXT("��ѽ������ﻹû��ǮŶ��"), TEXT("����"), mb_OK, mb_IconInformation);
	else if(-1==iGet)	//����ֵΪ-1�����δ����
		MsgBox(TEXT("��ѽ����û�ֺ��Ŷ"), TEXT("���δ����"), mb_OK, mb_IconInformation);
	else if(-2==iGet)	//����ֵΪ-2������ѱ�����
		MsgBox(TEXT("�����ˣ�����ѱ����꣡"), TEXT("���������"), mb_OK, mb_IconInformation);
	else
		MsgBox(StrAppend(TEXT("��ϲ"), tcUser, TEXT("����"),Str(iGet/100.0),TEXT("Ԫ�����")),
		TEXT("������"),mb_OK, mb_IconExclamation); // iGetΪ�֣���100.0�õ�Ԫ������.0����ʡ�ԣ�����ֻ��õ�����
}

void InfoList(CRedPackage & rpTemp)
{
	//���List�е�����
	form1.Control(ID_List).ListClear();
	//��������������Ϣ
	Part * ptList = rpTemp.GetptParts();
	int iOpenedNum = rpTemp.GetiOpened();
	//�ж������û��Ƿ�Ϊ�գ���Ϊ������ʾ�����δ���򿪣����ҷ���
	if(0==iOpenedNum) 
	{
		MsgBox(TEXT("�����δ���򿪣�"), TEXT("��ʾ"), mb_OK, mb_IconInformation);
		return;
	}
	int iMaxUser = 0;	// ������ѵ��û��ı��Ĭ��Ϊ1
	//��ȡ������ѵ��û��ı��
	for(int i=0;i<iOpenedNum;i++)
	{
		if(ptList[i].Money>ptList[iMaxUser].Money)
			iMaxUser = i;
	}
	TCHAR * tcItem;
	for(int i=0;i<iOpenedNum;i++)
	{
		tcItem = StrAppend(ptList[i].User, TEXT("����"),Str(ptList[i].Money/100.0), TEXT("Ԫ"));
		if(i == iMaxUser) tcItem = StrAppend(tcItem, TEXT(",������ѣ�"));
		form1.Control(ID_List).AddItem(tcItem);
	}
	LPTSTR labText = StrAppend(TEXT("�����"),Str(rpTemp.GetiMoneyTotal()/100.0),TEXT("Ԫ������"),
		Str(rpTemp.GetMoneyOpened()/100.0), TEXT("Ԫ��ʣ��"), Str(rpTemp.GetiOpened()), TEXT("/"), Str(rpTemp.GetiNum()));
	form1.Control(ID_LabPackageInfo).TextSet(labText);
}

int UserCheck(unsigned short idResControl, CRedPackage & rpTemp)
{
	TCHAR * tcTemp = form1.Control(idResControl).Text();
	if(_tcslen(tcTemp)>20)
		return 0;
	else
		for(int i=0;i<rpTemp.GetiOpened();i++)
		{
			if(_tcscmp(tcTemp, rpTemp.GetptUser(i))==0)
				return 1;
		}
	return -1;
}