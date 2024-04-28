#include "resource.h"
#include "BForm.h"
#include "mdlOpenSaveDlg.h"
#include "BReadLinesEx.h"

CBForm form1(ID_Form1);
CBForm formInputEnc(ID_FormInputEnc);

bool m_fShowingStatus;		// �Ƿ���ʾ״̬��
bool m_blSetTop;			// �Ƿ��ö�����
bool m_blSetClose;			// �Ƿ�ʹ��ϵͳ�رղ˵�
bool m_blModified;			// �Ƿ��޸Ĺ��ļ�
bool m_blIsTransParent;		// �Ƿ����ô���͸��
TCHAR m_szFileName[1024];	// ���浱ǰ�ļ���·��+�ļ�����""���ַ�����ʾ��δָ���ļ�
bool m_fEncInputOK;			// �����������ʱ�Ƿ�ȷ������
char * m_EncryptStr;		// ����ļ��������ַ�����ַ
tstring m_HelpContent;		// ��������ʾ������

void form1_MenuClick(int menuID, int bIsFromAcce, int bIsFromSysMenu);
bool QuerySave();
bool InputEncrypt();
bool OpenFile(LPCTSTR szFileName, char * encKey);

void HelpAbout_Init()
{
	m_HelpContent = TEXT("�ҵļ��±�  ��ǰ�汾��1.0.0\r\n���ߣ�PBY  ����ҽѧ����");
	m_HelpContent += TEXT("�������������ӹ���ѧԺ\r\n��Ȩ����(C) 2024\r\n\r\n");
	m_HelpContent += TEXT("�˳�������Ѿ���Դ��github\r\n");
	m_HelpContent += TEXT("���� https://limestart.cn/ �鿴");
}

/*	
��  �ã��ı�����������ɫ����
��  ����idResControl���˵�-�༭-��ɫ��ѡ�����ɫ�ؼ�ID
����ֵ����
*/
void TextColorSet(unsigned short int idResControl)
{
	switch(idResControl)	
	{
	case ID_MenuEditColorRed:form1.Control(ID_TextMain).ForeColorSet(RGB(255,0,0));break;	// �����ı���ɫΪ��ɫ
	case ID_MenuEditColorGreen:form1.Control(ID_TextMain).ForeColorSet(RGB(0,255,0));break;	// �����ı���ɫΪ��ɫ
	case ID_MenuEditColorBlue:form1.Control(ID_TextMain).ForeColorSet(RGB(0,0,255));break;	// �����ı���ɫΪ��ɫ
	case ID_MenuEditColorBlack:form1.Control(ID_TextMain).ForeColorSet(RGB(0,0,0));break;	// �����ı���ɫΪ��ɫ
	}
	form1.Control(ID_TextMain).BackColorSet(RGB(255,255,255));	// ���ñ�����ɫΪ��ɫ
	form1.Control(ID_TextMain).Refresh();						// ˢ��ʹ��ɫ����������Ч
	form1.Menu(idResControl).CheckedSet(true, true, 0, 3, true);	// ��ԭ���ʾѡ��
}

/*	
��  �ã���Կ���봰����ȷ�ϼ�������ʱ��ȫ����Կ����Ϊ�������Կ
��  ������
����ֵ����
*/
void ButtonOK_Click()
{
	LPTSTR szText = formInputEnc.Control(ID_TextEnc).Text();
	if(TEXT('\0') == *szText)
		// ��δ������Կ����ȫ����Կ����δ��
		m_EncryptStr = NULL;
	else
		// ��������Կ����ȫ����Կ��ANSI�����ʽ����
		m_EncryptStr = StrConvFromUnicode(szText);
	m_fEncInputOK = true;	// ��ʱ��Կ�����룬��Կ�Ƿ�����״̬תΪtrue
	formInputEnc.UnLoad();	// �رմ���
}

/*	
��  �ã���Կ���봰����ȡ������������ʱ���еĲ���
��  ������
����ֵ����
*/
void ButtonCancel_Click()
{
	m_fEncInputOK = false;	// ȡ��������Կ������Կ�Ƿ�����״̬��δfalse
	formInputEnc.UnLoad();	// �رմ���
}

/*	
��  �ã�TextMain�ؼ����ı����ı�ʱ���еĲ���
��  ������
����ֵ����
*/
void TextMain_Change()
{
	m_blModified=true;	// �ı����޸ģ��޸ı�־��Ϊtrue
	form1.Control(ID_TextMain).Refresh();	// ��ÿ���޸�ʱˢ��һ���ı���
}

/*	
��  �ã����ļ�����TextMain�ؼ���ʱ���еĲ���
��  ����ptrArrFile���ļ��ı�ָ��ĵ�ַ��ɵ�����������׵�ַ�������ε���ʽ����
		count�������ļ�����Ŀ
		x,y���Ϸ��ļ�ʱ�����������λ��
����ֵ����
*/
void TextMain_FilesDrop(int ptrArrFiles, int count, int x, int y)
{
	TCHAR **files = (TCHAR **)ptrArrFiles;	// ��ptrArrFilesǿ��ת��Ϊ TCHAR ** ����
	
	// �˴�����һ���ļ�����������ͬʱ�϶����ļ�
	if(QuerySave()) return;		// ��鵱ǰ�༭�ı��Ƿ񱣴�
	
	if(InputEncrypt())			// ���뵱ǰ�ļ�����Կ��ͬʱ����ȫ����Կ
		OpenFile(files[1], m_EncryptStr);	// �����������Կ���ļ�
}

/*	
��  �ã��ڴ������ļ����߹رմ���ʱȷ�ϵ�ǰ�ı��Ƿ��޸ģ�����δ����ʱ��ʾ��Ҫ����
��  ������
����ֵ��true������ʧ�ܻ���ȡ����ǰ�򿪻�رյĲ���
		false��ȷ�ϱ���ɹ���ѡ�񲻱���
*/
bool QuerySave()
{
	if(m_blModified)	// m_blModifiedΪTrue�ļ�δ����
	{
		EDlgBoxCmdID ms;
		ms = MsgBox(TEXT("�Ƿ񱣴�Ե�ǰ�ı����޸ġ�\r\n\r\n�Ƿ񱣴��ļ���"), 
			TEXT("�ҵļ��±�"), mb_YesNoCancel, mb_IconQuestion);	// �����ı���ȷ���Ƿ񱣴�
		if(ms==idYes)	// ȷ�ϱ���༭����ı�
		{
			// ģ�ⵥ���˵�-���棬��ʱm_blModified���ı�Ϊfalse
			form1_MenuClick(ID_MenuFileSave, false, false);
			// ������ʧ�ܣ���m_blModifiedӦ��Ϊtrue����˺�������trueֵ
			if(m_blModified) return true;
		}
		else if(ms==idCancel)	// ȡ����ǰ�Ĳ���
			return true;
	}
	// ѡ��ȷ�ϱ��棬���ұ���ɹ��򷵻�false
	// ����ѡ��No�����ʾ�����浱ǰ�ı���ͬ������false
	return false;
}

/*	
��  �ã��ڴ򿪻򱣴��ļ�ʱ��������Կ���ڣ���ȡ���ܻ������Կ��ANSI�룩
��  ������
����ֵ��m_fEncInputOK��true��ʾ�Ѿ�������Կ��false��ʾδ������Կ
*/
bool InputEncrypt()
{
	// ��������Կ֮ǰ��Ҫ����һ��ԭʼ�����Ƿ��ö�������ö���Ҫ��ʱȡ���ö�������Կ�������֮��ԭ�ö�״̬
	// ������Կ���봰��������ʾ���������м䣬�ᱻ�ö������ڸ���
	// ͬʱ������Ϊģ̬���壬�޷����������ᵼ���޷�������Կ�����³�����
	if(m_blSetTop)
		SetWindowPos(form1.hWnd(),HWND_NOTOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);


	formInputEnc.Load();				// �������봰��
	formInputEnc.IconSet(ID_IcoKey);	// ���ô���ͼ��
	formInputEnc.Control(ID_PicIcon).PictureSet(ID_BmpKey);		// ����ͼƬ
	formInputEnc.Move(
		form1.Left()+(form1.Width()-formInputEnc.Width())/2,
		form1.Top()+(form1.Height()-formInputEnc.Height())/2);	// ���ô��ڳ���λ����form1�����м�
	m_fEncInputOK = false;				// Ĭ��δȷ������

	// ��ʾ���봰��
	formInputEnc.Show(1, form1.hWnd());	// 1��ʾģ̬���壬��״̬���������޷��������������ڴ���ͣ
	return m_fEncInputOK;
}

/*	
��  �ã������ļ�
��  ����szFileName��Ŀ�걣���ļ����ļ���������·����
		encKey�������ļ�ʱ�ļ�����Կ
����ֵ��true���ļ�����ɹ�
		false���ļ�����ʧ��
*/
bool SaveFile(LPCTSTR szFileName, char * encKey)
{
	form1.Control(ID_LabStatus).TextSet(TEXT(""));	// ���״̬��
	
	HANDLE hFile = EFOpen(szFileName, EF_OpStyle_Output);	// ����д���ļ�
	// ֱ��д���Կؼ�ID_TextMain�ı�����Ϊ����
	// ��д���ֽ���С��0������false����д��ʧ�� 
	if(EFPrint(hFile, form1.Control(ID_TextMain).Text(), EF_LineSeed_None,
					-1, 1, TEXT("�޷����ļ���д���ַ���"), encKey)<0) 
		return false;
	EFClose(hFile);	//�ر��ļ����
	
	_tcscpy(m_szFileName, szFileName);	// �����ļ�����ȫ��
	form1.TextSet(m_szFileName);
	form1.TextAdd(TEXT("-�ҵļ��±�"));	// ��������ʾ"�ļ���-�ҵļ��±�"
	form1.Control(ID_LabStatus).TextSet(TEXT("�ļ�����ɹ���"));
	
	// �����޸ı�־Ϊfalse����ʾ�ı�δ���޸Ļ�ǰ�޸��ѱ�����
	m_blModified = false;

	return true;
}

/*	
��  �ã����ļ�
��  ����szFileName��Ŀ����ļ����ļ���������·����
		encKey�����ļ�ʱ���ܵ���Կ
����ֵ��true���ɹ����ļ�
		false�����ļ�ʧ��
*/
bool OpenFile(LPCTSTR szFileName, char * encKey)
{
	tstring sText(TEXT(""));				// �洢��ȡ�����ļ����ݣ���ʼΪ""
	LPTSTR szLine = NULL, szTemp = NULL;	// ����ָ�룬�����ļ���ȡ	
	CBReadLinesEx file;						// ʵ����CBReadLinesEx��Ϊfile����

	form1.Control(ID_LabStatus).TextSet(TEXT(""));	// ���״̬��
	
	if(!file.OpenFile(szFileName)) return false;	// ���ļ�������ʧ�ܷ���false
	file.EncryptKey = encKey;						// ���ý�����Կ
	pApp->MousePointerGlobalSet(ID_CurWait);		// ����Ŀ�����Ϊ�ȴ�״̬

	while(!file.IsEndRead())		// ѭ����ȡ�ļ�ÿһ�У�ֱ����ȡ���ļ�ĩβ
	{
		file.GetNextLine(szLine);	// ��ȡ�ļ�����һ��
		if(file.IsErrOccured())		// ������ж�ȡ��������flase
		{
			return false;
		}
		if(file.iEndLineSignLast>0)	// ����һ�д��ڻ��У���ͬһ��ʾΪ\r\n
			sText += TEXT("\r\n");
		sText += szLine;
	}
	if(file.iEndLineSignLast>0)		// ���һ�д��ڻ��У����ӻ��з�
		sText += TEXT("\r\n");
	
	Sleep(1000);
	pApp->MousePointerGlobalSet(0);	// �������֮�����״̬����ԭʼ״̬

	form1.Control(ID_TextMain).TextSet(sText);	// ���ı��ؼ�����ʾ��ȡ�����ļ�����

	_tcscpy(m_szFileName, szFileName);	//���ļ������浽ȫ��
	form1.TextSet(m_szFileName);
	form1.TextAdd(TEXT("-�ҵļ��±�"));	// ��������ʾ"�ļ���-�ҵļ��±�"
	form1.Control(ID_LabStatus).TextSet(TEXT("�ļ��򿪳ɹ���"));

	// �����޸ı�־Ϊfalse����ʾ�ı�δ���޸Ļ�ǰ�޸��ѱ�����
	m_blModified = false;
	
	return true;
}

/*	
��  �ã����������С���ı�ʱ���������ڵĿؼ�����һЩ����
��  ������
����ֵ����
*/
void form1_Resize()
{
	int iStatus = 0;	//״̬���߶�
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
��  �ã����������ʱ��һЩ��־λ�Լ�һЩ�ؼ�������ʼ��
��  ������
����ֵ����
*/
void form1_Load()
{
	m_fShowingStatus = true;	// Ĭ����ʾ״̬��
	m_blSetTop = false;			// Ĭ�ϲ��ö�����
	m_blSetClose = true;		// Ĭ��ʹ��ϵͳ�ر�
	m_blIsTransParent = false;	// Ĭ�����ô��岻͸��
	m_szFileName[0] = 0;		// ��ʼĬ���ļ���Ϊ�գ���ʾ��δָ���ļ���
	m_fEncInputOK = false;		// δȷ����������
	m_EncryptStr = NULL;		// δָ����������

	form1.Control(ID_LabStatus).TextSet(TEXT("��ӭʹ�ã�"));
	form1.Control(ID_LabStatus).FontSizeSet(10);
	form1.Control(ID_TextMain).FontSizeSet(14);
	form1.Control(ID_TextMain).DisableContextMenuSet(true);		// ���οؼ�Ĭ�ϵ����˵�
	form1.Control(ID_LabStatus).DisableContextMenuSet(true);	
	form1_Resize();

	HelpAbout_Init();
}

/*
��  �ã�������ر�ʱ�Դ����һЩ����
��  ������
����ֵ����
*/
void form1_Unload(int pbCancel)
{
	if(QuerySave()) // ����Ϊtrue, ��ʾȡ��������߱���ʧ�ܣ���ȡ���رմ���
		*(int *)pbCancel = 1;
}

/*	
��  �ã��������в˵��������ʱ���еĲ���
��  ����menuID���������Ĳ˵���ID
		bIsFromAcce���˵�ID�Ƿ�������Acce
		bIsFromSysMenu���˵�ID�Ƿ�������ϵͳ�˵�
����ֵ����
*/
void form1_MenuClick(int menuID, int bIsFromAcce, int bIsFromSysMenu)
{
	switch(menuID)	// ͨ��switch������ʵ��ĳһ���������ʱʵ�ְ�������
	{
	case ID_MenuFileExit:		// �ļ�-�˳������
		form1.UnLoad();
		break;
	
	case ID_MenuHelpAbout:		// ����-���ڱ����
		MsgBox(m_HelpContent,
			TEXT("���� �ҵļ��±�"), mb_OK, mb_IconNone);
		break;
	
	case ID_MenuViewStatus:		// �鿴-״̬�������
		m_fShowingStatus = !m_fShowingStatus;
		form1.Control(ID_LabStatus).VisibleSet(m_fShowingStatus);
		form1.Menu(ID_MenuViewStatus).CheckedSet(m_fShowingStatus);
		form1_Resize();
		break;
	
	case ID_MenuEditSelectAll:	// �༭-ȫѡ�����
		form1.Control(ID_TextMain).SelSet();
		break;
	
	case ID_MenuEditDate:		// �༭-����ʱ�䱻���
		form1.Control(ID_TextMain).SelTextSet(Now());
		break;

	case ID_MenuEditCut:		// �༭-���б����
		form1.Control(ID_TextMain).Cut();
		form1.Control(ID_TextMain).Refresh();	// ˢ���ļ�����ֹ���ֻ���
		break;

	case ID_MenuEditCopy:		// �༭-���Ʊ����
		form1.Control(ID_TextMain).Copy();
		break;

	case ID_MenuEditPaste:		// �༭-ճ�������
		form1.Control(ID_TextMain).Paste();
		break;

	case ID_MenuEditFontSong:	// �༭-����-���屻���
		form1.Control(ID_TextMain).FontNameSet(TEXT("����"));
		form1.Menu(ID_MenuEditFontSong).CheckedSet(true, false, 0, 0, true);	// �ڱ�ѡ�е�����ǰ�򹴣���ʱ���Է�Χ����
		form1.Menu(ID_MenuEditFontHan).CheckedSet(false, false, 0, 0, true);	// ��CheckedSet����õ�bRadio=falseʱ��
		form1.Menu(ID_MenuEditFontHei).CheckedSet(false, false, 0, 0, true);	// ��Ҫ������ѡ����и��£�ȥ�������Ĺ�ѡ
		break;

	case ID_MenuEditFontHan:	// �༭-����-���鱻���
		form1.Control(ID_TextMain).FontNameSet(TEXT("����"));
		form1.Menu(ID_MenuEditFontSong).CheckedSet(false, false, 0, 0, true);	// �ڱ�ѡ�е�����ǰ��
		form1.Menu(ID_MenuEditFontHan).CheckedSet(true, false, 0, 0, true);
		form1.Menu(ID_MenuEditFontHei).CheckedSet(false, false, 0, 0, true);
		break;

	case ID_MenuEditFontHei:	// �༭-����-���屻���
		form1.Control(ID_TextMain).FontNameSet(TEXT("����"));
		form1.Menu(ID_MenuEditFontSong).CheckedSet(false, false, 0, 0, true);
		form1.Menu(ID_MenuEditFontHan).CheckedSet(false, false, 0, 0, true);
		form1.Menu(ID_MenuEditFontHei).CheckedSet(true, false, 0, 0, true);
		break;

	case ID_MenuEditColorRed:	// �༭-��ɫ-��ɫ�����
		TextColorSet(ID_MenuEditColorRed);
		break;

	case ID_MenuEditColorGreen:	// �༭-��ɫ-��ɫ�����
		TextColorSet(ID_MenuEditColorGreen);
		break;

	case ID_MenuEditColorBlue:	// �༭-��ɫ-��ɫ�����
		TextColorSet(ID_MenuEditColorBlue);
		break;

	case ID_MenuEditColorBlack:	// �༭-��ɫ-��ɫ�����
		TextColorSet(ID_MenuEditColorBlack);
		break;

	case ID_MenuViewTop:		// �༭-�鿴-�����ö������
		m_blSetTop = !m_blSetTop;	// ��ת�ö�״̬
		if(m_blSetTop){
			SetWindowPos(form1.hWnd(),HWND_TOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
			form1.Menu(ID_MenuViewTop).TextSet(TEXT("ȡ���ö�(&T)"));
		}
		else{
			SetWindowPos(form1.hWnd(),HWND_NOTOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
			form1.Menu(ID_MenuViewTop).TextSet(TEXT("�����ö�(&T)"));
		}
		form1.Menu(ID_MenuViewTop).CheckedSet(m_blSetTop);
		break;

	case ID_MenuPopOpen:		// �����˵�-�򿪱����
		form1_MenuClick(ID_MenuFileOpen, bIsFromAcce, bIsFromSysMenu);
		break;

	case ID_MenuPopSave:		// �����˵�-���汻���
		form1_MenuClick(ID_MenuFileSave, bIsFromAcce, bIsFromSysMenu);
		break;

	case ID_MenuPopRefresh:		// �����˵�-ˢ�±����
		form1.Control(ID_TextMain).Refresh();
		break;

	case ID_MenuPopAbout:		// �����˵�-���ڱ����
		form1_MenuClick(ID_MenuHelpAbout, bIsFromAcce, bIsFromSysMenu);
		break;

	case ID_MenuFileConcentrate:// �ļ�-רע�����
		// �˹��ܹر�ϵͳ�رղ�����ʹ�༭���޷�ͨ�����÷�ʽ�ر��ı���
		m_blSetClose = !m_blSetClose;
		if(m_blSetClose)
			form1.Menu(ID_MenuFileConcentrate).TextSet(TEXT("רע(&C)"));
		else
			form1.Menu(ID_MenuFileConcentrate).TextSet(TEXT("ȡ��רע(&C)"));
		form1.Menu(SC_CLOSE).EnabledSet(m_blSetClose);
		form1.Menu(ID_MenuFileExit).EnabledSet(m_blSetClose);
		form1.Menu(ID_MenuFileConcentrate).CheckedSet(!m_blSetClose);
		break;

	case ID_MenuFileSave:		// �ļ�-���汻���
		if (0 == *m_szFileName)	// �жϵ�ǰ�ļ��Ƿ񱻱����
			// ��m_szFileNameΪ""����ǰ�ļ�δ����������򵯳����Ϊ�Ի��򣬽������Ϊ����
			form1_MenuClick(ID_MenuFileSaveas, bIsFromAcce, bIsFromSysMenu);
		else
			// ��m_szFileName��Ϊ""����ǰ�ļ�����������Ե�ǰ�ļ�·�������ļ�
			if(!SaveFile(m_szFileName, m_EncryptStr))
				MsgBox(TEXT("����ʧ�ܣ�"), TEXT("��ʾ"),mb_OK, mb_IconError);	// ����ʧ�ܣ�������ʾ
		break;

	case ID_MenuFileSaveas:		// �ļ�-���Ϊ�����
		{
			LPTSTR szFileSave;								// ������ʱ���������ļ���
			OsdSetFilter(TEXT("�ı��ļ�(*.txt)|*.txt"));	// �����ļ�������
			szFileSave = OsdSaveDlg(form1.hWnd());			// ���ļ����Ϊ����

			if(*szFileSave)		// ���û��������ļ����򱣴��ļ���������ȡ�����ļ���Ϊ��""���򲻱����ļ�
				if(InputEncrypt())
					SaveFile(szFileSave, m_EncryptStr);
			else
				MsgBox(TEXT("ȡ�����棡"), TEXT("��ʾ"), mb_OK, mb_IconInformation);
		}
		break;

	case ID_MenuFileOpen:		// �ļ�-�򿪱����
		{
			if(QuerySave()) return;	// ��ʾȡ���򿪻��ߵ�ǰ�ļ�����ʧ�ܣ��򷵻�

			LPTSTR szFileOpen;								// ������ʱ���������ļ���
			OsdSetFilter(TEXT("�ı��ļ�(*.txt)|*.txt"));	// �����ļ�������
			szFileOpen = OsdOpenDlg(form1.hWnd());			// ���ļ��򿪴���
			
			if(*szFileOpen)		// ���û�ѡ���˴򿪵��ļ�����ļ���������ȡ�����ļ���Ϊ��""���򲻴��ļ�
				if(InputEncrypt())
					OpenFile(szFileOpen, m_EncryptStr);
			else
				MsgBox(TEXT("ȡ���򿪣�"), TEXT("��ʾ"), mb_OK, mb_IconInformation);
		}
		break;

	case ID_MenuFileNew:		// �ļ�-�½������
		if(QuerySave()) return;	// �жϵ�ǰ�ļ��Ƿ���Ҫ����
		
		// �½��ļ�
		form1.Control(ID_TextMain).TextSet(TEXT(""));	// ��յ�ǰ�ı����е�����
		form1.TextSet(TEXT("�ҵļ��±�"));				// ��մ���������е��ļ���
		form1.Control(ID_LabStatus).TextSet(TEXT(""));	// ���״̬���е�����

		*m_szFileName = 0;		// ����ָ����ļ�������ʾ��δָ���ļ���
		m_blModified = false;	// �½���ɺ�δ�޸Ĺ�����
		m_fEncInputOK = false;	// ��δȷ����������
		m_EncryptStr = NULL;	// ��ʾ��ǰû�м�������
		break;

	case ID_MenuViewTransParent:// �鿴-͸�������ñ����
		m_blIsTransParent = !m_blIsTransParent;	// ÿ�ε��͸��״̬��ȡ��
		if(m_blIsTransParent)
			form1.OpacitySet(128);
		else
			form1.OpacitySet(255);
		form1.Menu(ID_MenuViewTransParent).CheckedSet(m_blIsTransParent);	// ÿ�ε�����л���״̬
	} // end of switch(menuID)
}

/*	
��  �ã������ָ��ĳһλ��ʱ����״̬������ʾ��ǰ���ָ���Ĳ˵�����Ϣ
��  ����ptrCBMenuItem��һ��ָ��˵��������ָ��
����ֵ����
*/
void form1_MenuSelect(int ptrCBMenuItem)
{
	CBMenuItem * pMenu = (CBMenuItem *)ptrCBMenuItem;
	tstring sPrompt(TEXT(""));	// ������Ҫ��״̬������ʾ���ַ�
	if (ptrCBMenuItem)			// ��ptrCBMenuItem��Ϊ�գ���ʾ���ָ��λ�ڲ˵�����
	{
		if (pMenu->IsSeparator())	// ָ��ָ��ָ���
			sPrompt = TEXT("���ָ���˲˵��ָ���");
		else	// ���ָ��Ƿָ��ߵĲ���
		{
			sPrompt = TEXT("����ǰ��ָ��˵�Ϊ��");	sPrompt += pMenu->Text();
			sPrompt += TEXT("  �˵�λ���ǣ�");		sPrompt += Str(pMenu->Position());
			if(pMenu->hSubMenu())	// ���ָ������һ���˵��е��Ӳ˵�
			{
				sPrompt += TEXT("  �������Ӳ˵����Ϊ��");
				sPrompt += Str((unsigned int)pMenu->hSubMenu());
			}
			else	// ���û��ָ��˵��е��Ӳ˵�
			{
				sPrompt += TEXT("  ��ԴID�ǣ�");
				sPrompt += Str(pMenu->IDRes());
			} // end if if(pMenu->hSubMenu())
		} // end if if (pMenu->IsSeparator())	//ָ��ָ��ָ���
	} // end if if (ptrCBMenuItem)	// ��ptrCBMenuItem��Ϊ�գ���ʾ���ָ��λ�ڲ˵�����
	// ���ָ�벻�ڲ˵��ϣ�����sPrompΪ��
	form1.Control(ID_LabStatus).TextSet(sPrompt);
}

/*	
��  �ã���Կ���봰�ڹر�ʱ���еĲ���
��  ������
����ֵ����
*/
void formInputEnc_Unload()
{
	// ����Կ���봰�ڴ�ʱ���ж��������Ƿ��ö������������ö������ȡ�������ڵ��ö�
	// ������Կ���봰�ڹر�ʱ��Ҫ�жϲ��ָ�������ԭ����״̬
	if(m_blSetTop)
		SetWindowPos(form1.hWnd(),HWND_TOPMOST,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOSIZE);
}

/*	
��  �ã���TextMain�ؼ��а�����갴��ʱ���еĲ���
��  ����button��1��2��4��ʾ������ҡ����Ƿ񱻰���
		shift��1��2��4��ʾShift��Ctrl��Alt�Ƿ񱻰���
		x��y����갴�°���ʱ���ָ��������λ�ò���
����ֵ����
*/
void TextMain_MouseDown(int button, int shift, int x, int y)
{
	if (2==button)	// buttonΪ2��ʾ��������Ҽ�
	{	// ��x,yת��Ϊ��������ϵ(�������Ͻ�Ϊ(0��0))
		x = form1.Control(ID_TextMain).Left() + x;
		y = form1.Control(ID_TextMain).Top() + y;
		form1.PopupMenu(ID_PopMenu, x, y);
	}
}

/*	
��  �ã���TextMain�ؼ��а�����갴��ʱ���еĲ���
��  ����button��1��2��4��ʾ������ҡ����Ƿ񱻰���
		shift��1��2��4��ʾShift��Ctrl��Alt�Ƿ񱻰���
		x��y����갴�°���ʱ���ָ��������λ�ò���
����ֵ����
*/
void LabStatus_MouseDown(int button, int shift, int x, int y)
{
	if (2==button)
	{	// ��x,yת��Ϊ��������ϵ(�������Ͻ�Ϊ(0��0))
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

	form1.Menu(4,0).RightJustifySet(true);		// �������˵��ƶ��������

	// Ϊ���˵�������λͼ��ʶ
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
	
	form1.SetAccelerator(ID_Accelerator);		// �������ټ���Դ��ʹ���ټ���Ч
	form1.IconSet(ID_Icon1);
	form1.Show();
	return 0;
}