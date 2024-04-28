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
作  用：在窗体加载时对一些控件内容、字体等进行设置
参  数：无
返回值：无
*/
void form1_load()
{
	//设置文本为空
	form1.Control(ID_txtInput).TextSet(TEXT("0"));
	form1.Control(ID_txtResult).TextSet(TEXT(""));
	form1.Control(ID_txtMax).TextSet(TEXT("0"));
	form1.Control(ID_txtMin).TextSet(TEXT("0"));
	//设置字体大小
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
作  用：窗体关闭时对窗体的一些操作
参  数：无
返回值：无
*/
void form1_unload(int pbCancel)
{
	if(MsgBox(TEXT("是否确认退出？"), TEXT("退出选项"), 
		mb_YesNo, mb_IconQuestion) != idYes)
		*(int *)pbCancel = 1;
}

/*
作  用：判断一个数是否为质数
参  数：num:输入的整数
返回值：True:是质数，False:不是质数
*/
bool IsPrime(int num)
{
	if(0 == num || 1 == num)
		return false;// 输入为0返回为非素数
	int i, k;
	k = (int)sqrt((double) num); // 将数开根，减小时间复杂度
	for(i = 2; i <= k; i++)
		if(num % i == 0) break;
	// 判断i以判断num是否为素数
	if(i > k) 
		return true;
	else 
		return false;
}

/*
作  用：按下“判断素数”后，在文本框中显示输入数是否为质数
参  数：无
返回值：无
*/
void buttonJudge_Click()
{
	int input = (int)form1.Control(ID_txtInput).TextVal();	// 获取需要判断的数字
	if (IsPrime(input))										// 判断质数并在文本框中显示结果
		form1.Control(ID_txtResult).TextSet(TEXT("素数"));
	else
		form1.Control(ID_txtResult).TextSet(TEXT("非素数"));
}

/*
作  用：按下按键，列出从2开始的100个质数
参  数：无
返回值：无
*/
void buttonList100_Click()
{
	int count = 0; // 计数值，记录显示的质数的个数
	form1.Control(ID_list).ListClear();// 清空质数显示列表
	for(int i = 2; count < 100; i++){ 
		if(IsPrime(i)){
			form1.Control(ID_list).AddItem(Str(i));
			count++;
		}
	}
}

/*
作  用：按下按键，判断输入范围是否合法，列出范围内的所有素数
参  数：无
返回值：无
*/
void buttonListRange_Click()
{
	int min_num, max_num;
	// 清空质数显示列表
	form1.Control(ID_list).ListClear();
	// 获取范围
	min_num = (int) form1.Control(ID_txtMin).TextVal();
	max_num = (int) form1.Control(ID_txtMax).TextVal();
	// 判断输入值是否合法
	if(min_num >= max_num){
		MsgBox(TEXT("输入最大值必须大于最小值！"), TEXT("输入不合法"), mb_OK, mb_IconError);
		form1.Control(ID_txtMin).TextSet(TEXT("0"));
		form1.Control(ID_txtMax).TextSet(TEXT("0"));
		return;
	}
	// 列出范围内的所有素数
	for(int i = min_num; i <= max_num; i++){
		if(IsPrime(i)){
			form1.Control(ID_list).AddItem(Str(i));
		}
	}
}

/*
作  用：按下退出键，关闭窗口
参  数：无
返回值：无
*/
void buttonExit_Click()
{
	form1.UnLoad();
}

/*
作  用：按键事件函数，对输入text的文本进行限制
参  数：keyAscii:输入按键的Ascii码, pbCancel:是否取消按键的判断参数
返回值：无
*/
void txtInput_KeyPress(int keyAscii, int pbCancel)
{
	// 若在Input窗口中按回车键（ASCII码=13）表示判断结果
	if(13 == keyAscii)
		buttonJudge_Click();
	// 若在deposit窗口中按ESC键（ASCII码=27）表示退出
	else if(27==keyAscii)
		buttonExit_Click();
	// 在deposit窗口中只能输入数值，退格（8）
	else if( ! (keyAscii>='0' && keyAscii<='9' || 8==keyAscii))
		*(int*)pbCancel =1;
}

void txtMaxMin_KeyPress(int keyAscii, int pbCancel)
{
	// 在deposit窗口中只能输入数值，退格（8）
	if( ! (keyAscii>='0' && keyAscii<='9' || 8==keyAscii))
		*(int*)pbCancel =1;
}