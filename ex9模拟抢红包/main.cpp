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
	//装载图片 目前还未开始
	//设置红包C的按键为灰色，因为C还未塞钱
	form1.Control(ID_ButtonOpenPack3).EnabledSet(false);
	//机器人抢一个红包B
	rpB.OpenOnePack(TEXT("机器人2"));
}

// 利用构造函数输入参数创建红包
void ButtonOpenPack1_Click()
{
	OpenRedPackage(ID_TextUser1, rpA);
}

// 无参数输入，默认构造红包
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
	if(MsgBox(StrAppend(TEXT("确认发送红包？\n红包金额："), Str(iSetMoney/100.0), 
		TEXT("元\n红包个数："), Str(iSetNum)),TEXT("确认"), mb_OkCancel)!=idOk)
	{
		MsgBox(TEXT("已取消发送！"), TEXT("提示"), mb_OK, mb_IconInformation);
		return;
	}
	else
		MsgBox(TEXT("红包已发送！"), TEXT("提示"), mb_OK, mb_IconInformation);
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
	//若输入为0～9、小数点、退格之外的数，则取消本次输入
	if(!(keyAscii>='0' && keyAscii<='9' || keyAscii=='.'|| 8==keyAscii))
		*(int*)pbCancel = 1;
}

void TextPackNum_KeyPress(int keyAscii, int pbCancel)
{
	//若输入为0～9、退格之外的数，则取消本次输入
	if(!(keyAscii>='0' && keyAscii<='9' || 8==keyAscii))
		*(int*)pbCancel = 1;
}

void OpenRedPackage(unsigned short idResControl, CRedPackage & rpTemp)
{
	TCHAR * tcUser = form1.Control(idResControl).Text();
	if(TEXT('\0')==*tcUser)
	{
		MsgBox(TEXT("请输入用户名称！"), TEXT("用户名错误"), mb_OK, mb_IconExclamation);
		return;
	}
	int uc = UserCheck(idResControl, rpTemp);
	if(0==uc)
	{
		MsgBox(TEXT("输入用户名称超额！请限制在20个字符内！"),TEXT("警告"), mb_OK, mb_IconError);
		return;
	}
	else if(1==uc)
	{
		MsgBox(TEXT("输入用户名重复！请重新输入！"), TEXT("警告"), mb_OK, mb_IconError);
		return;
	}
	int iGet = rpTemp.OpenOnePack(tcUser);
	if(0==iGet)			//返回值为0，红包内无金额
		MsgBox(TEXT("哎呀，红包里还没有钱哦！"), TEXT("金额不足"), mb_OK, mb_IconInformation);
	else if(-1==iGet)	//返回值为-1，红包未划分
		MsgBox(TEXT("哎呀，还没分红包哦"), TEXT("红包未划分"), mb_OK, mb_IconInformation);
	else if(-2==iGet)	//返回值为-2，红包已被抢完
		MsgBox(TEXT("来晚了！红包已被领完！"), TEXT("红包已领完"), mb_OK, mb_IconInformation);
	else
		MsgBox(StrAppend(TEXT("恭喜"), tcUser, TEXT("抢到"),Str(iGet/100.0),TEXT("元红包！")),
		TEXT("抢到啦"),mb_OK, mb_IconExclamation); // iGet为分，除100.0得到元，并且.0不可省略，否则只会得到整数
}

void InfoList(CRedPackage & rpTemp)
{
	//清空List中的数据
	form1.Control(ID_List).ListClear();
	//获得已抢红包的信息
	Part * ptList = rpTemp.GetptParts();
	int iOpenedNum = rpTemp.GetiOpened();
	//判断已抢用户是否为空，若为空则提示红包还未被打开，并且返回
	if(0==iOpenedNum) 
	{
		MsgBox(TEXT("红包尚未被打开！"), TEXT("提示"), mb_OK, mb_IconInformation);
		return;
	}
	int iMaxUser = 0;	// 手气最佳的用户的编号默认为1
	//获取手气最佳的用户的编号
	for(int i=0;i<iOpenedNum;i++)
	{
		if(ptList[i].Money>ptList[iMaxUser].Money)
			iMaxUser = i;
	}
	TCHAR * tcItem;
	for(int i=0;i<iOpenedNum;i++)
	{
		tcItem = StrAppend(ptList[i].User, TEXT("抢到"),Str(ptList[i].Money/100.0), TEXT("元"));
		if(i == iMaxUser) tcItem = StrAppend(tcItem, TEXT(",手气最佳！"));
		form1.Control(ID_List).AddItem(tcItem);
	}
	LPTSTR labText = StrAppend(TEXT("红包共"),Str(rpTemp.GetiMoneyTotal()/100.0),TEXT("元，已抢"),
		Str(rpTemp.GetMoneyOpened()/100.0), TEXT("元，剩余"), Str(rpTemp.GetiOpened()), TEXT("/"), Str(rpTemp.GetiNum()));
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