#include "resource.h"
#include "BForm.h"
#include "mdlOpenSaveDlg.h"
#include "BReadLinesEx.h"

CBForm form1(ID_Form1);
CBForm formInputEnc(ID_FormInputEnc);

bool m_fShowingStatus;		// 是否显示状态栏
bool m_blSetTop;			// 是否置顶窗口
bool m_blSetClose;			// 是否使能系统关闭菜单
bool m_blModified;			// 是否修改过文件
bool m_blIsTransParent;		// 是否设置窗体透明
TCHAR m_szFileName[1024];	// 保存当前文件的路径+文件名，""空字符串表示尚未指定文件
bool m_fEncInputOK;			// 输入加密密码时是否确认输入
char * m_EncryptStr;		// 输入的加密密码字符串地址
tstring m_HelpContent;		// 帮助中显示的内容

void form1_MenuClick(int menuID, int bIsFromAcce, int bIsFromSysMenu);
bool QuerySave();
bool InputEncrypt();
bool OpenFile(LPCTSTR szFileName, char * encKey);

void HelpAbout_Init()
{
	m_HelpContent = TEXT("我的记事本  当前版本：1.0.0\r\n作者：PBY  生物医学工程");
	m_HelpContent += TEXT("精密仪器与光电子工程学院\r\n版权所有(C) 2024\r\n\r\n");
	m_HelpContent += TEXT("此程序代码已经开源至github\r\n");
	m_HelpContent += TEXT("可至 https://limestart.cn/ 查看");
}

/*	
作  用：文本框内字体颜色设置
参  数：idResControl：菜单-编辑-颜色中选择的颜色控件ID
返回值：无
*/
void TextColorSet(unsigned short int idResControl)
{
	switch(idResControl)	
	{
	case ID_MenuEditColorRed:form1.Control(ID_TextMain).ForeColorSet(RGB(255,0,0));break;	// 设置文本颜色为红色
	case ID_MenuEditColorGreen:form1.Control(ID_TextMain).ForeColorSet(RGB(0,255,0));break;	// 设置文本颜色为绿色
	case ID_MenuEditColorBlue:form1.Control(ID_TextMain).ForeColorSet(RGB(0,0,255));break;	// 设置文本颜色为蓝色
	case ID_MenuEditColorBlack:form1.Control(ID_TextMain).ForeColorSet(RGB(0,0,0));break;	// 设置文本颜色为黑色
	}
	form1.Control(ID_TextMain).BackColorSet(RGB(255,255,255));	// 设置背景颜色为白色
	form1.Control(ID_TextMain).Refresh();						// 刷新使颜色设置立刻生效
	form1.Menu(idResControl).CheckedSet(true, true, 0, 3, true);	// 用原点表示选中
}

/*	
作  用：密钥输入窗口内确认键被按下时将全局密钥设置为输入的密钥
参  数：无
返回值：无
*/
void ButtonOK_Click()
{
	LPTSTR szText = formInputEnc.Control(ID_TextEnc).Text();
	if(TEXT('\0') == *szText)
		// 若未输入密钥，则全局密钥设置未空
		m_EncryptStr = NULL;
	else
		// 若输入密钥，则将全局密钥以ANSI码的形式保存
		m_EncryptStr = StrConvFromUnicode(szText);
	m_fEncInputOK = true;	// 此时密钥被输入，密钥是否输入状态转为true
	formInputEnc.UnLoad();	// 关闭窗口
}

/*	
作  用：密钥输入窗口内取消按键被按下时进行的操作
参  数：无
返回值：无
*/
void ButtonCancel_Click()
{
	m_fEncInputOK = false;	// 取消输入密钥，则密钥是否输入状态改未false
	formInputEnc.UnLoad();	// 关闭窗口
}

/*	
作  用：TextMain控件中文本被改变时进行的操作
参  数：无
返回值：无
*/
void TextMain_Change()
{
	m_blModified=true;	// 文本被修改，修改标志改为true
	form1.Control(ID_TextMain).Refresh();	// 在每次修改时刷新一次文本框
}

/*	
作  用：将文件拖入TextMain控件中时进行的操作
参  数：ptrArrFile：文件文本指针的地址组成的数组的数组首地址，以整形的形式传入
		count：拖入文件的数目
		x,y：拖放文件时的鼠标光标所在位置
返回值：无
*/
void TextMain_FilesDrop(int ptrArrFiles, int count, int x, int y)
{
	TCHAR **files = (TCHAR **)ptrArrFiles;	// 将ptrArrFiles强制转换为 TCHAR ** 类型
	
	// 此处仅打开一个文件，忽略其他同时拖动的文件
	if(QuerySave()) return;		// 检查当前编辑文本是否保存
	
	if(InputEncrypt())			// 输入当前文件的密钥，同时更新全局密钥
		OpenFile(files[1], m_EncryptStr);	// 根据输入的密钥打开文件
}

/*	
作  用：在打开其他文件或者关闭窗口时确认当前文本是否修改，并在未保存时提示需要保存
参  数：无
返回值：true：保存失败或者取消当前打开或关闭的操作
		false：确认保存成功或选择不保存
*/
bool QuerySave()
{
	if(m_blModified)	// m_blModified为True文件未保存
	{
		EDlgBoxCmdID ms;
		ms = MsgBox(TEXT("是否保存对当前文本的修改。\r\n\r\n是否保存文件？"), 
			TEXT("我的记事本"), mb_YesNoCancel, mb_IconQuestion);	// 弹出文本框确认是否保存
		if(ms==idYes)	// 确认保存编辑后的文本
		{
			// 模拟单击菜单-保存，此时m_blModified被改变为false
			form1_MenuClick(ID_MenuFileSave, false, false);
			// 若保存失败，则m_blModified应还为true，则此函数返回true值
			if(m_blModified) return true;
		}
		else if(ms==idCancel)	// 取消当前的操作
			return true;
	}
	// 选择确认保存，并且保存成功则返回false
	// 或者选择No，则表示不保存当前文本，同样返回false
	return false;
}

/*	
作  用：在打开或保存文件时，弹出密钥窗口，获取解密或加密密钥（ANSI码）
参  数：无
返回值：m_fEncInputOK：true表示已经输入密钥，false表示未输入密钥
*/
bool InputEncrypt()
{
	// 在输入密钥之前需要检验一下原始窗体是否置顶，如果置顶需要暂时取消置顶，在密钥输入完成之后还原置顶状态
	// 否则密钥输入窗口由于显示在主窗口中间，会被置顶主窗口覆盖
	// 同时主窗口为模态窗体，无法被操作，会导致无法输入密钥，导致程序卡死
	if(m_blSetTop)
		SetWindowPos(form1.hWnd(),HWND_NOTOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);


	formInputEnc.Load();				// 加载输入窗体
	formInputEnc.IconSet(ID_IcoKey);	// 设置窗口图标
	formInputEnc.Control(ID_PicIcon).PictureSet(ID_BmpKey);		// 设置图片
	formInputEnc.Move(
		form1.Left()+(form1.Width()-formInputEnc.Width())/2,
		form1.Top()+(form1.Height()-formInputEnc.Height())/2);	// 设置窗口出现位置在form1窗体中间
	m_fEncInputOK = false;				// 默认未确认输入

	// 显示输入窗体
	formInputEnc.Show(1, form1.hWnd());	// 1表示模态窗体，此状态下主窗体无法被操作，程序在此暂停
	return m_fEncInputOK;
}

/*	
作  用：保存文件
参  数：szFileName：目标保存文件的文件名（包括路径）
		encKey：保存文件时的加密密钥
返回值：true：文件保存成功
		false：文件保存失败
*/
bool SaveFile(LPCTSTR szFileName, char * encKey)
{
	form1.Control(ID_LabStatus).TextSet(TEXT(""));	// 清除状态栏
	
	HANDLE hFile = EFOpen(szFileName, EF_OpStyle_Output);	// 覆盖写入文件
	// 直接写入以控件ID_TextMain文本内容为输入
	// 若写入字节数小于0，返回false，即写入失败 
	if(EFPrint(hFile, form1.Control(ID_TextMain).Text(), EF_LineSeed_None,
					-1, 1, TEXT("无法向文件中写入字符串"), encKey)<0) 
		return false;
	EFClose(hFile);	//关闭文件句柄
	
	_tcscpy(m_szFileName, szFileName);	// 保存文件名至全局
	form1.TextSet(m_szFileName);
	form1.TextAdd(TEXT("-我的记事本"));	// 窗体中显示"文件名-我的记事本"
	form1.Control(ID_LabStatus).TextSet(TEXT("文件保存成功。"));
	
	// 设置修改标志为false，表示文本未被修改或当前修改已被保存
	m_blModified = false;

	return true;
}

/*	
作  用：打开文件
参  数：szFileName：目标打开文件的文件名（包括路径）
		encKey：打开文件时解密的密钥
返回值：true：成功打开文件
		false：打开文件失败
*/
bool OpenFile(LPCTSTR szFileName, char * encKey)
{
	tstring sText(TEXT(""));				// 存储读取到的文件内容，初始为""
	LPTSTR szLine = NULL, szTemp = NULL;	// 两个指针，用于文件读取	
	CBReadLinesEx file;						// 实例化CBReadLinesEx类为file对象

	form1.Control(ID_LabStatus).TextSet(TEXT(""));	// 清空状态栏
	
	if(!file.OpenFile(szFileName)) return false;	// 打开文件，若打开失败返回false
	file.EncryptKey = encKey;						// 设置解密密钥
	pApp->MousePointerGlobalSet(ID_CurWait);		// 更改目标鼠标为等待状态

	while(!file.IsEndRead())		// 循环读取文件每一行，直至读取到文件末尾
	{
		file.GetNextLine(szLine);	// 读取文件的下一行
		if(file.IsErrOccured())		// 如果本行读取出错，返回flase
		{
			return false;
		}
		if(file.iEndLineSignLast>0)	// 若上一行存在换行，则同一显示为\r\n
			sText += TEXT("\r\n");
		sText += szLine;
	}
	if(file.iEndLineSignLast>0)		// 最后一行存在换行，连接换行符
		sText += TEXT("\r\n");
	
	Sleep(1000);
	pApp->MousePointerGlobalSet(0);	// 加载完成之后鼠标状态返回原始状态

	form1.Control(ID_TextMain).TextSet(sText);	// 在文本控件中显示读取到的文件内容

	_tcscpy(m_szFileName, szFileName);	//将文件名保存到全局
	form1.TextSet(m_szFileName);
	form1.TextAdd(TEXT("-我的记事本"));	// 窗体中显示"文件名-我的记事本"
	form1.Control(ID_LabStatus).TextSet(TEXT("文件打开成功。"));

	// 设置修改标志为false，表示文本未被修改或当前修改已被保存
	m_blModified = false;
	
	return true;
}

/*	
作  用：在主窗体大小被改变时对主窗体内的控件进行一些操作
参  数：无
返回值：无
*/
void form1_Resize()
{
	int iStatus = 0;	//状态栏高度
	if(m_fShowingStatus)
	{
		iStatus = form1.Control(ID_LabStatus).Height()+6;
		form1.Control(ID_LabStatus).Move(4,
			form1.ClientHeight()-iStatus+4,
			form1.ClientWidth()-8,iStatus-6);
	}
	form1.Control(ID_TextMain).Move(0,0,form1.ClientWidth(), form1.ClientHeight()-iStatus);
}

/*	
作  用：主窗体加载时对一些标志位以及一些控件参数初始化
参  数：无
返回值：无
*/
void form1_Load()
{
	m_fShowingStatus = true;	// 默认显示状态栏
	m_blSetTop = false;			// 默认不置顶窗口
	m_blSetClose = true;		// 默认使能系统关闭
	m_blIsTransParent = false;	// 默认设置窗体不透明
	m_szFileName[0] = 0;		// 初始默认文件名为空，表示尚未指定文件名
	m_fEncInputOK = false;		// 未确认输入密码
	m_EncryptStr = NULL;		// 未指定加密密码

	form1.Control(ID_LabStatus).TextSet(TEXT("欢迎使用！"));
	form1.Control(ID_LabStatus).FontSizeSet(10);
	form1.Control(ID_TextMain).FontSizeSet(14);
	form1.Control(ID_TextMain).DisableContextMenuSet(true);		// 屏蔽控件默认弹出菜单
	form1.Control(ID_LabStatus).DisableContextMenuSet(true);	
	form1_Resize();

	HelpAbout_Init();
}

/*
作  用：主窗体关闭时对窗体的一些操作
参  数：无
返回值：无
*/
void form1_Unload(int pbCancel)
{
	if(QuerySave()) // 返回为true, 表示取消保存或者保存失败，则取消关闭窗口
		*(int *)pbCancel = 1;
}

/*	
作  用：主窗体中菜单栏被点击时进行的操作
参  数：menuID：被单机的菜单的ID
		bIsFromAcce：菜单ID是否来自于Acce
		bIsFromSysMenu：菜单ID是否来自于系统菜单
返回值：空
*/
void form1_MenuClick(int menuID, int bIsFromAcce, int bIsFromSysMenu)
{
	switch(menuID)	// 通过switch函数来实现某一按键被点击时实现按键功能
	{
	case ID_MenuFileExit:		// 文件-退出被点击
		form1.UnLoad();
		break;
	
	case ID_MenuHelpAbout:		// 帮助-关于被点击
		MsgBox(m_HelpContent,
			TEXT("关于 我的记事本"), mb_OK, mb_IconNone);
		break;
	
	case ID_MenuViewStatus:		// 查看-状态栏被点击
		m_fShowingStatus = !m_fShowingStatus;
		form1.Control(ID_LabStatus).VisibleSet(m_fShowingStatus);
		form1.Menu(ID_MenuViewStatus).CheckedSet(m_fShowingStatus);
		form1_Resize();
		break;
	
	case ID_MenuEditSelectAll:	// 编辑-全选被点击
		form1.Control(ID_TextMain).SelSet();
		break;
	
	case ID_MenuEditDate:		// 编辑-日期时间被点击
		form1.Control(ID_TextMain).SelTextSet(Now());
		break;

	case ID_MenuEditCut:		// 编辑-剪切被点击
		form1.Control(ID_TextMain).Cut();
		form1.Control(ID_TextMain).Refresh();	// 刷新文件，防止文字混乱
		break;

	case ID_MenuEditCopy:		// 编辑-复制被点击
		form1.Control(ID_TextMain).Copy();
		break;

	case ID_MenuEditPaste:		// 编辑-粘贴被点击
		form1.Control(ID_TextMain).Paste();
		break;

	case ID_MenuEditFontSong:	// 编辑-字体-宋体被点击
		form1.Control(ID_TextMain).FontNameSet(TEXT("宋体"));
		form1.Menu(ID_MenuEditFontSong).CheckedSet(true, false, 0, 0, true);	// 在被选中的字体前打勾，此时忽略范围参数
		form1.Menu(ID_MenuEditFontHan).CheckedSet(false, false, 0, 0, true);	// 由CheckedSet定义得当bRadio=false时，
		form1.Menu(ID_MenuEditFontHei).CheckedSet(false, false, 0, 0, true);	// 需要对其他选项进行更新，去除其他的勾选
		break;

	case ID_MenuEditFontHan:	// 编辑-字体-汉书被点击
		form1.Control(ID_TextMain).FontNameSet(TEXT("汉书"));
		form1.Menu(ID_MenuEditFontSong).CheckedSet(false, false, 0, 0, true);	// 在被选中的字体前打勾
		form1.Menu(ID_MenuEditFontHan).CheckedSet(true, false, 0, 0, true);
		form1.Menu(ID_MenuEditFontHei).CheckedSet(false, false, 0, 0, true);
		break;

	case ID_MenuEditFontHei:	// 编辑-字体-黑体被点击
		form1.Control(ID_TextMain).FontNameSet(TEXT("黑体"));
		form1.Menu(ID_MenuEditFontSong).CheckedSet(false, false, 0, 0, true);
		form1.Menu(ID_MenuEditFontHan).CheckedSet(false, false, 0, 0, true);
		form1.Menu(ID_MenuEditFontHei).CheckedSet(true, false, 0, 0, true);
		break;

	case ID_MenuEditColorRed:	// 编辑-颜色-红色被点击
		TextColorSet(ID_MenuEditColorRed);
		break;

	case ID_MenuEditColorGreen:	// 编辑-颜色-绿色被点击
		TextColorSet(ID_MenuEditColorGreen);
		break;

	case ID_MenuEditColorBlue:	// 编辑-颜色-蓝色被点击
		TextColorSet(ID_MenuEditColorBlue);
		break;

	case ID_MenuEditColorBlack:	// 编辑-颜色-黑色被点击
		TextColorSet(ID_MenuEditColorBlack);
		break;

	case ID_MenuViewTop:		// 编辑-查看-窗口置顶被点击
		m_blSetTop = !m_blSetTop;	// 反转置顶状态
		if(m_blSetTop){
			SetWindowPos(form1.hWnd(),HWND_TOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
			form1.Menu(ID_MenuViewTop).TextSet(TEXT("取消置顶(&T)"));
		}
		else{
			SetWindowPos(form1.hWnd(),HWND_NOTOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
			form1.Menu(ID_MenuViewTop).TextSet(TEXT("窗口置顶(&T)"));
		}
		form1.Menu(ID_MenuViewTop).CheckedSet(m_blSetTop);
		break;

	case ID_MenuPopOpen:		// 弹出菜单-打开被点击
		form1_MenuClick(ID_MenuFileOpen, bIsFromAcce, bIsFromSysMenu);
		break;

	case ID_MenuPopSave:		// 弹出菜单-保存被点击
		form1_MenuClick(ID_MenuFileSave, bIsFromAcce, bIsFromSysMenu);
		break;

	case ID_MenuPopRefresh:		// 弹出菜单-刷新被点击
		form1.Control(ID_TextMain).Refresh();
		break;

	case ID_MenuPopAbout:		// 弹出菜单-关于被点击
		form1_MenuClick(ID_MenuHelpAbout, bIsFromAcce, bIsFromSysMenu);
		break;

	case ID_MenuFileConcentrate:// 文件-专注被点击
		// 此功能关闭系统关闭操作，使编辑者无法通过常用方式关闭文本框
		m_blSetClose = !m_blSetClose;
		if(m_blSetClose)
			form1.Menu(ID_MenuFileConcentrate).TextSet(TEXT("专注(&C)"));
		else
			form1.Menu(ID_MenuFileConcentrate).TextSet(TEXT("取消专注(&C)"));
		form1.Menu(SC_CLOSE).EnabledSet(m_blSetClose);
		form1.Menu(ID_MenuFileExit).EnabledSet(m_blSetClose);
		form1.Menu(ID_MenuFileConcentrate).CheckedSet(!m_blSetClose);
		break;

	case ID_MenuFileSave:		// 文件-保存被点击
		if (0 == *m_szFileName)	// 判断当前文件是否被保存过
			// 若m_szFileName为""，则当前文件未被保存过，则弹出另存为对话框，进行另存为操作
			form1_MenuClick(ID_MenuFileSaveas, bIsFromAcce, bIsFromSysMenu);
		else
			// 若m_szFileName不为""，则当前文件被保存过，以当前文件路径保存文件
			if(!SaveFile(m_szFileName, m_EncryptStr))
				MsgBox(TEXT("保存失败！"), TEXT("提示"),mb_OK, mb_IconError);	// 保存失败，弹窗显示
		break;

	case ID_MenuFileSaveas:		// 文件-另存为被点击
		{
			LPTSTR szFileSave;								// 定义临时变量接收文件名
			OsdSetFilter(TEXT("文本文件(*.txt)|*.txt"));	// 设置文件过滤器
			szFileSave = OsdSaveDlg(form1.hWnd());			// 打开文件另存为窗口

			if(*szFileSave)		// 若用户输入了文件名则保存文件，若按下取消则文件名为空""，则不保存文件
				if(InputEncrypt())
					SaveFile(szFileSave, m_EncryptStr);
			else
				MsgBox(TEXT("取消保存！"), TEXT("提示"), mb_OK, mb_IconInformation);
		}
		break;

	case ID_MenuFileOpen:		// 文件-打开被点击
		{
			if(QuerySave()) return;	// 表示取消打开或者当前文件保存失败，则返回

			LPTSTR szFileOpen;								// 定义临时变量接收文件名
			OsdSetFilter(TEXT("文本文件(*.txt)|*.txt"));	// 设置文件过滤器
			szFileOpen = OsdOpenDlg(form1.hWnd());			// 打开文件打开窗口
			
			if(*szFileOpen)		// 若用户选择了打开的文件则打开文件，若按下取消则文件名为空""，则不打开文件
				if(InputEncrypt())
					OpenFile(szFileOpen, m_EncryptStr);
			else
				MsgBox(TEXT("取消打开！"), TEXT("提示"), mb_OK, mb_IconInformation);
		}
		break;

	case ID_MenuFileNew:		// 文件-新建被点击
		if(QuerySave()) return;	// 判断当前文件是否需要保存
		
		// 新建文件
		form1.Control(ID_TextMain).TextSet(TEXT(""));	// 清空当前文本框中的内容
		form1.TextSet(TEXT("我的记事本"));				// 清空窗体标题栏中的文件名
		form1.Control(ID_LabStatus).TextSet(TEXT(""));	// 清空状态栏中的内容

		*m_szFileName = 0;		// 重置指向的文件名，表示尚未指定文件名
		m_blModified = false;	// 新建完成后未修改过内容
		m_fEncInputOK = false;	// 尚未确定加密密码
		m_EncryptStr = NULL;	// 表示当前没有加密密码
		break;

	case ID_MenuViewTransParent:// 查看-透明度设置被点击
		m_blIsTransParent = !m_blIsTransParent;	// 每次点击透明状态都取反
		if(m_blIsTransParent)
			form1.OpacitySet(128);
		else
			form1.OpacitySet(255);
		form1.Menu(ID_MenuViewTransParent).CheckedSet(m_blIsTransParent);	// 每次点击都切换打勾状态
	} // end of switch(menuID)
}

/*	
作  用：当鼠标指到某一位置时，在状态栏中显示当前鼠标指到的菜单的信息
参  数：ptrCBMenuItem：一个指向菜单项处理对象的指针
返回值：无
*/
void form1_MenuSelect(int ptrCBMenuItem)
{
	CBMenuItem * pMenu = (CBMenuItem *)ptrCBMenuItem;
	tstring sPrompt(TEXT(""));	// 定义需要在状态栏中显示的字符
	if (ptrCBMenuItem)			// 即ptrCBMenuItem不为空，表示鼠标指针位于菜单项上
	{
		if (pMenu->IsSeparator())	// 指针指向分割线
			sPrompt = TEXT("鼠标指向了菜单分割线");
		else	// 鼠标指向非分割线的部分
		{
			sPrompt = TEXT("您当前所指向菜单为：");	sPrompt += pMenu->Text();
			sPrompt += TEXT("  菜单位置是：");		sPrompt += Str(pMenu->Position());
			if(pMenu->hSubMenu())	// 鼠标指向了下一个菜单中的子菜单
			{
				sPrompt += TEXT("  引出的子菜单句柄为：");
				sPrompt += Str((unsigned int)pMenu->hSubMenu());
			}
			else	// 鼠标没有指向菜单中的子菜单
			{
				sPrompt += TEXT("  资源ID是：");
				sPrompt += Str(pMenu->IDRes());
			} // end if if(pMenu->hSubMenu())
		} // end if if (pMenu->IsSeparator())	//指针指向分割线
	} // end if if (ptrCBMenuItem)	// 即ptrCBMenuItem不为空，表示鼠标指针位于菜单项上
	// 鼠标指针不在菜单上，保持sPromp为空
	form1.Control(ID_LabStatus).TextSet(sPrompt);
}

/*	
作  用：密钥输入窗口关闭时进行的操作
参  数：无
返回值：无
*/
void formInputEnc_Unload()
{
	// 在密钥输入窗口打开时会判断主窗口是否置顶，若主窗口置顶，则会取消主窗口的置顶
	// 则在密钥输入窗口关闭时需要判断并恢复主窗口原本的状态
	if(m_blSetTop)
		SetWindowPos(form1.hWnd(),HWND_TOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
}

/*	
作  用：在TextMain控件中按下鼠标按键时进行的操作
参  数：button：1、2、4表示鼠标左、右、中是否被按下
		shift：1、2、4表示Shift、Ctrl、Alt是否被按下
		x，y：鼠标按下按键时鼠标指针所处的位置参数
返回值：无
*/
void TextMain_MouseDown(int button, int shift, int x, int y)
{
	if (2==button)	// button为2表示按下鼠标右键
	{	// 将x,y转换为窗体坐标系(窗体左上角为(0，0))
		x = form1.Control(ID_TextMain).Left() + x;
		y = form1.Control(ID_TextMain).Top() + y;
		form1.PopupMenu(ID_PopMenu, x, y);
	}
}

/*	
作  用：在TextMain控件中按下鼠标按键时进行的操作
参  数：button：1、2、4表示鼠标左、右、中是否被按下
		shift：1、2、4表示Shift、Ctrl、Alt是否被按下
		x，y：鼠标按下按键时鼠标指针所处的位置参数
返回值：无
*/
void LabStatus_MouseDown(int button, int shift, int x, int y)
{
	if (2==button)
	{	// 将x,y转换为窗体坐标系(窗体左上角为(0，0))
		x = form1.Control(ID_LabStatus).Left() + x;
		y = form1.Control(ID_LabStatus).Top() + y;
		form1.PopupMenu(ID_PopMenu, x, y);
	}
}

int main()
{
	form1.EventAdd(0, eForm_Load, form1_Load);
	form1.EventAdd(0, eForm_QueryUnload, form1_Unload);
	form1.EventAdd(0, eForm_Resize, form1_Resize);
	form1.EventAdd(0, eMenu_Click, form1_MenuClick);
	form1.EventAdd(0, eMenu_Select, form1_MenuSelect);
	form1.EventAdd(ID_TextMain, eMouseUp, TextMain_MouseDown);
	form1.EventAdd(ID_TextMain, eEdit_Change, TextMain_Change);
	form1.EventAdd(ID_TextMain, eFilesDrop, TextMain_FilesDrop);
	form1.EventAdd(ID_LabStatus, eMouseUp, LabStatus_MouseDown);
	
	formInputEnc.EventAdd(0, eForm_Unload, formInputEnc_Unload);
	formInputEnc.EventAdd(ID_ButtonOK, eCommandButton_Click, ButtonOK_Click);
	formInputEnc.EventAdd(ID_ButtonCancel, eCommandButton_Click, ButtonCancel_Click);

	form1.Menu(4,0).RightJustifySet(true);		// 将帮助菜单移动至最左侧

	// 为主菜单项设置位图标识
	form1.Menu(ID_MenuFileNew).SetCheckBitmaps(ID_BmpNew, ID_BmpNew);
	form1.Menu(ID_MenuFileOpen).SetCheckBitmaps(ID_BmpOpen, ID_BmpOpen);
	form1.Menu(ID_MenuFileSave).SetCheckBitmaps(ID_BmpSave, ID_BmpSave);
	form1.Menu(ID_MenuFileSaveas).SetCheckBitmaps(ID_BmpSaveas, ID_BmpSaveas);
	form1.Menu(ID_MenuFileExit).SetCheckBitmaps(ID_BmpExit, ID_BmpExit);
	form1.Menu(ID_MenuFileConcentrate).SetCheckBitmaps(ID_BmpConcentrate,ID_BmpClose);
	form1.Menu(ID_MenuEditCut).SetCheckBitmaps(ID_BmpCut, ID_BmpCut);
	form1.Menu(ID_MenuEditCopy).SetCheckBitmaps(ID_BmpCopy, ID_BmpCopy);
	form1.Menu(ID_MenuEditPaste).SetCheckBitmaps(ID_BmpPaste, ID_BmpPaste);
	form1.Menu(ID_MenuEditDate).SetCheckBitmaps(ID_BmpDate, ID_BmpDate);
	form1.Menu(ID_MenuEditColorRed).SetCheckBitmaps(ID_BmpRedUnPick, ID_BmpRedPick);
	form1.Menu(ID_MenuEditColorGreen).SetCheckBitmaps(ID_BmpGreenUnPick, ID_BmpGreenPick);
	form1.Menu(ID_MenuEditColorBlue).SetCheckBitmaps(ID_BmpBlueUnPick, ID_BmpBluePick);
	form1.Menu(ID_MenuEditColorBlack).SetCheckBitmaps(ID_BmpBlackUnPick, ID_BmpBlackPick);
	form1.Menu(ID_MenuViewStatus).SetCheckBitmaps(ID_BmpStatus, ID_BmpNostatus);
	form1.Menu(ID_MenuViewTop).SetCheckBitmaps(ID_BmpTop,ID_BmpBack);
	form1.Menu(ID_MenuHelpAbout).SetCheckBitmaps(ID_BmpHelp, ID_BmpHelp);
	
	form1.SetAccelerator(ID_Accelerator);		// 关联加速键资源，使加速键生效
	form1.IconSet(ID_Icon1);
	form1.Show();
	return 0;
}