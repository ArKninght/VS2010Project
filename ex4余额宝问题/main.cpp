#include "resource.h"
#include "BForm.h"

CBForm form1(ID_Form1);

void form1_load()
{
	// 设置控件初始值

	
	form1.Control(ID_txtRate).TextSet(2.64);
	form1.Control(ID_txtDeposit).TextSet(10000);
	form1.Control(ID_txtGet).TextSet(100000);
	form1.Control(ID_labelResult).TextSet(TEXT(""));
}

void cmdCalc_Click()
{
	double x;       // 本金
	double r;		// 年化收益
	double xExp;	// 预期本利
	int i = 0;		// 经过年数

	// 从控件获得用户输入
	x = form1.Control(ID_txtDeposit).TextVal();
	r = form1.Control(ID_txtRate).TextVal() / 100; // 将用户输入单位转换为 %
	xExp = Val(form1.Control(ID_txtGet).Text());
	// Text()为获得字符串（首地址），使用Val函数将获得的字符串转换为double类型
	// 注意，不能直接强制转换Text()。（(double)Text()是将地址转换为double）

	// 判断输入是否合法
	if(x > xExp)
	{
		MsgBox(TEXT("预期收入不能低于存款哦！"),
			TEXT("输入不合法"), mb_OK,mb_IconExclamation); // 字符串内容需要放置在TEXT中
		return;
	}

	// 计算
	while(1)
	{
		x = x * (1 + r);
		i++;
		if(x>xExp)break;
	}

	// 显示结果到控件 ID_labelResult
	form1.Control(ID_labelResult).TextSet(i);
}

void cmdExit_Click()
{
	form1.UnLoad();
}

void form1_unload(int pbCancel)
{
	if(MsgBox(TEXT("是否确认退出？"), TEXT("退出选项"), 
		mb_YesNo, mb_IconQuestion) != idYes)
		*(int *)pbCancel = 1;

}

void txtInput_KeyPress(int keyAscii, int pbCancel)
{
	// 若在deposit窗口中按回车键（ASCII码=13）表示计算结果
	if(13==keyAscii)
		cmdCalc_Click();
	// 若在deposit窗口中按ESC键（ASCII码=27）表示退出
	else if(27==keyAscii)
		cmdExit_Click();
	// 在deposit窗口中只能输入数值，小数点，负号，退格（8）
	else if( ! (keyAscii>='0' && keyAscii<='9' || '-'==keyAscii || '.'==keyAscii || 8==keyAscii))
		*(int*)pbCancel =1;
}

int main()
{
	form1.EventAdd(0, eForm_Load, form1_load); // 此函数必须在窗体加载之前 
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