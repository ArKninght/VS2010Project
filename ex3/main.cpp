#include "resource.h"
#include "BForm.h"

CBForm form1(ID_Form1);

void form1_QueryUnload(int pCancel);
void ButtonExit_Click();
void ButtonAdd_Click();
void ButtonMinus_Click();
void ButtonMulti_Click();
void ButtonDiv_Click();
void form1_Resize();
void FormInit();

int main()
{
	FormInit();
	form1.EventAdd(0, eForm_QueryUnload, form1_QueryUnload);
	form1.EventAdd(0, eForm_Resize, form1_Resize);
	form1.EventAdd(ID_ButtonExit, eCommandButton_Click, ButtonExit_Click);
	form1.EventAdd(ID_ButtonAdd, eCommandButton_Click, ButtonAdd_Click);
	form1.EventAdd(ID_ButtonMinus, eCommandButton_Click, ButtonMinus_Click);
	form1.EventAdd(ID_ButtonMulti, eCommandButton_Click, ButtonMulti_Click);
	form1.EventAdd(ID_ButtonDiv, eCommandButton_Click, ButtonDiv_Click);
	form1.IconSet(ID_Icon1);
	form1.Show(); 
	form1_Resize();
	return 0;
}

void form1_QueryUnload(int pCancel)
{
	if(MsgBox(TEXT("确认要退出吗？"),TEXT("确认退出"), mb_YesNo, mb_IconQuestion)==idNo)
	{
		*((int *)pCancel)=1;
	}
}

void ButtonExit_Click()
{
	form1.UnLoad();
}

void ButtonAdd_Click()
{
	double a,b;
	a = form1.Control(ID_TextData1).TextVal();
	b = form1.Control(ID_TextData2).TextVal();
	form1.Control(ID_TextResult).TextSet(a+b);
}

void ButtonMinus_Click()
{
	double a,b;
	a = form1.Control(ID_TextData1).TextVal();
	b = form1.Control(ID_TextData2).TextVal();
	form1.Control(ID_TextResult).TextSet(a-b);
}

void ButtonMulti_Click()
{
	double a,b;
	a = form1.Control(ID_TextData1).TextVal();
	b = form1.Control(ID_TextData2).TextVal();
	form1.Control(ID_TextResult).TextSet(a*b);
}

void ButtonDiv_Click()
{
	double a,b;
	a = form1.Control(ID_TextData1).TextVal();
	b = form1.Control(ID_TextData2).TextVal();
	if(0==b)
	{
		MsgBox(TEXT("除数不能为0"), TEXT("出错！"), mb_OK, mb_IconError);
		form1.Control(ID_TextResult).TextSet(TEXT(""));
	}
	else
		form1.Control(ID_TextResult).TextSet(a/b);
}

void form1_Resize()
{
	int SetWidth1 = (int)form1.Width()/5;
	int SetHeight1 = (int)form1.ClientHeight()/5;
	int ButtonWidth = (int)form1.Width()/4;
	int LabelLeft = form1.Control(ID_Label1).Left();
	int Labe1Top = form1.Control(ID_Label1).Top();
	form1.Control(ID_Label1).Move(LabelLeft, Labe1Top,SetWidth1, SetHeight1);
	form1.Control(ID_Label3).Move(LabelLeft, Labe1Top+SetHeight1+10, SetWidth1, SetHeight1);
	form1.Control(ID_TextData1).Move(LabelLeft+SetWidth1+10, Labe1Top, SetWidth1*2, SetHeight1);
	form1.Control(ID_TextData2).Move(LabelLeft+SetWidth1+10, Labe1Top+SetHeight1+10, SetWidth1*2, SetHeight1);
	form1.Control(ID_ButtonAdd).Move(0, Labe1Top+(SetHeight1+10)*2, ButtonWidth, SetHeight1);
	form1.Control(ID_ButtonMinus).Move(ButtonWidth, Labe1Top+(SetHeight1+10)*2, ButtonWidth, SetHeight1);
	form1.Control(ID_ButtonMulti).Move(ButtonWidth*2, Labe1Top+(SetHeight1+10)*2, ButtonWidth, SetHeight1);
	form1.Control(ID_ButtonDiv).Move(ButtonWidth*3, Labe1Top+(SetHeight1+10)*2, ButtonWidth, SetHeight1);
	form1.Control(ID_Label2).Move(0,Labe1Top+(SetHeight1+10)*3, SetWidth1, SetHeight1);
	form1.Control(ID_TextResult).Move(SetWidth1+10, Labe1Top+(SetHeight1+10)*3, SetWidth1*2, SetHeight1);
	form1.Control(ID_ButtonExit).Move(SetWidth1*3+30, Labe1Top+(SetHeight1+10)*3, ButtonWidth, SetHeight1);

}

void FormInit()
{
	form1.Control(ID_Label1).FontSizeSet(15);
	form1.Control(ID_Label2).FontSizeSet(15);
	form1.Control(ID_Label3).FontSizeSet(15);
	form1.Control(ID_TextData1).FontSizeSet(25);
	form1.Control(ID_TextData2).FontSizeSet(25);
	form1.Control(ID_TextResult).FontSizeSet(25);
	form1.Control(ID_ButtonAdd).FontSizeSet(15);
	form1.Control(ID_ButtonMinus).FontSizeSet(15);
	form1.Control(ID_ButtonMulti).FontSizeSet(15);
	form1.Control(ID_ButtonDiv).FontSizeSet(15);
	form1.Control(ID_ButtonExit).FontSizeSet(15);
}
