#include "resource.h"
#include "BForm.h"

CBForm form1(ID_Form1);
CBForm form2(ID_Form2);

void form1_Resize()
{
	form1.Control(ID_Label1).Move(
		(form1.Width() - form1.Control(ID_Label1).Width())/2,
		(form1.ClientHeight() - form1.Control(ID_Label1).Height())/2
		);
}

void Button1_Click()
{
	form2.Show();
	form2.IconSet(ID_Icon1);
}

int main()
{
	form1.EventAdd(0, eForm_Resize, form1_Resize);
	form1.EventAdd(ID_Button1, eCommandButton_Click, Button1_Click);
	form1.IconSet(ID_Icon1);
	form1.Show();
	form1.Control(ID_Label1).FontSizeSet(20);
	form1.Control(ID_Label1).ForeColorSet(RGB(255,0,0));
	form1.Control(ID_Button1).FontSizeSet(20);
	return 0;
}