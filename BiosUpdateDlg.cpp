// BiosUpdateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BiosUpdate.h"
#include "BiosUpdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CHECK_EXE
#define FPTW_EXE
#define IDRV_DLL
#define PMX_DLL
#define FPARTS_TXT
#define AFUWIN_EXE
#define AMIDEWIN_EXE
#define AMIFLDRV32_DLL
#define AMIFLDRV64_DLL

// CBiosUpdateDlg dialog




CBiosUpdateDlg::CBiosUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBiosUpdateDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBiosUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBiosUpdateDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_UPDATE, &CBiosUpdateDlg::OnBnClickedUpdate)
	ON_WM_DESTROY()
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_BROWSE, &CBiosUpdateDlg::OnBnClickedBrowse)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_SNCHK, &CBiosUpdateDlg::OnBnClickedSnchk)
END_MESSAGE_MAP()


// CBiosUpdateDlg message handlers

BOOL CBiosUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	DragAcceptFiles();

	GetCurrentDirectory(2048,m_curPath);
	typedef BOOL (WINAPI* ChangeWindowMessageFilterFn)( UINT, DWORD );
	HMODULE hUserMod = NULL;
	BOOL bResult = FALSE;
	hUserMod = LoadLibrary( "user32.dll" );
	if ( hUserMod )
	{
		ChangeWindowMessageFilterFn pfnChangeWindowMessageFilter = (ChangeWindowMessageFilterFn)GetProcAddress( hUserMod, "ChangeWindowMessageFilter" );
		if (pfnChangeWindowMessageFilter)
		{
			pfnChangeWindowMessageFilter(WM_DROPFILES, 1); // 1-MSGFLT_ADD, 2-MSGFLT_REMOVE
			pfnChangeWindowMessageFilter(0x0049, 1); // 1-MSGFLT_ADD, 2-MSGFLT_REMOVE
		}

		FreeLibrary( hUserMod );
	}
	m_nBiosSize = 0;
	m_bExistKey = FALSE;
	m_strSSN = "";
	m_strBSN = "";
	IsWow64Process(GetCurrentProcess(),&m_bIsx64);

	SnRefresh();

	strcpy(m_szTempDir,getenv("SystemRoot"));
	strcat(m_szTempDir,"\\Temp");
	DWORD dwLen;
	CFile fp;
	BOOL bRet;
	SetCurrentDirectory(m_szTempDir);
//---------------------------------------------------------------------
	//Microsoft tools
#ifdef CHECK_EXE
	HRSRC hSrc = FindResource(NULL,MAKEINTRESOURCE(IDR_CHECK),"DATA");
	HGLOBAL hGl = LoadResource(NULL,hSrc);
	dwLen = SizeofResource(NULL,hSrc);
	LPBYTE lpBuf = (LPBYTE)LockResource(hGl);
	bRet = fp.Open("Check.exe",CFile::modeCreate|CFile::modeReadWrite);
	fp.Write((LPBYTE)lpBuf,dwLen);
	fp.Close();
#endif
	//Intel tools
#ifdef FPTW_EXE
	hSrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FPTW),"DATA");
	hGl = LoadResource(NULL,hSrc);
	dwLen = SizeofResource(NULL,hSrc);
	lpBuf = (LPBYTE)LockResource(hGl);
	bRet = fp.Open("fptw.exe",CFile::modeCreate|CFile::modeReadWrite);
	fp.Write((LPBYTE)lpBuf,dwLen);
	fp.Close();
#endif
#ifdef IDRV_DLL
	hSrc = FindResource(NULL,MAKEINTRESOURCE(IDR_IDRVDLL),"DATA");
	hGl = LoadResource(NULL,hSrc);
	dwLen = SizeofResource(NULL,hSrc);
	lpBuf = (LPBYTE)LockResource(hGl);
	bRet = fp.Open("idrvdll.dll",CFile::modeCreate|CFile::modeReadWrite);
	fp.Write((LPBYTE)lpBuf,dwLen);
	fp.Close();
#endif
#ifdef PMX_DLL
	hSrc = FindResource(NULL,MAKEINTRESOURCE(IDR_PMXDLL),"DATA");
	hGl = LoadResource(NULL,hSrc);
	dwLen = SizeofResource(NULL,hSrc);
	lpBuf = (LPBYTE)LockResource(hGl);
	bRet = fp.Open("pmxdll.dll",CFile::modeCreate|CFile::modeReadWrite);
	fp.Write((LPBYTE)lpBuf,dwLen);
	fp.Close();
#endif
#ifdef FPARTS_TXT
	hSrc = FindResource(NULL,MAKEINTRESOURCE(IDR_FPARTS),"DATA");
	hGl = LoadResource(NULL,hSrc);
	dwLen = SizeofResource(NULL,hSrc);
	lpBuf = (LPBYTE)LockResource(hGl);
	bRet = fp.Open("fparts.txt",CFile::modeCreate|CFile::modeReadWrite);
	fp.Write((LPBYTE)lpBuf,dwLen);
	fp.Close();
#endif
	//AMI tools
#ifdef AFUWIN_EXE
	hSrc = FindResource(NULL,MAKEINTRESOURCE(IDR_AFUWIN),"DATA");
	hGl = LoadResource(NULL,hSrc);
	dwLen = SizeofResource(NULL,hSrc);
	lpBuf = (LPBYTE)LockResource(hGl);
	bRet = fp.Open("afuwin.exe",CFile::modeCreate|CFile::modeReadWrite);
	fp.Write((LPBYTE)lpBuf,dwLen);
	fp.Close();
#endif
#ifdef AMIDEWIN_EXE
	hSrc = FindResource(NULL,MAKEINTRESOURCE(IDR_AMIDEWIN),"DATA");
	hGl = LoadResource(NULL,hSrc);
	dwLen = SizeofResource(NULL,hSrc);
	lpBuf = (LPBYTE)LockResource(hGl);
	bRet = fp.Open("amidewin.exe",CFile::modeCreate|CFile::modeReadWrite);
	fp.Write((LPBYTE)lpBuf,dwLen);
	fp.Close();
#endif
#ifdef AMIFLDRV32_DLL
	hSrc = FindResource(NULL,MAKEINTRESOURCE(IDR_AMIFLDRV32),"DATA");
	hGl = LoadResource(NULL,hSrc);
	dwLen = SizeofResource(NULL,hSrc);
	lpBuf = (LPBYTE)LockResource(hGl);
	bRet = fp.Open("amifldrv32.sys",CFile::modeCreate|CFile::modeReadWrite);
	fp.Write((LPBYTE)lpBuf,dwLen);
	fp.Close();
#endif
#ifdef AMIFLDRV64_DLL
	hSrc = FindResource(NULL,MAKEINTRESOURCE(IDR_AMIFLDRV64),"DATA");
	hGl = LoadResource(NULL,hSrc);
	dwLen = SizeofResource(NULL,hSrc);
	lpBuf = (LPBYTE)LockResource(hGl);
	bRet = fp.Open("amifldrv64.sys",CFile::modeCreate|CFile::modeReadWrite);
	fp.Write((LPBYTE)lpBuf,dwLen);
	fp.Close();
#endif
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBiosUpdateDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CRect rc(5,12,245,32);
		CFont font;
		LOGFONT logFont;
		GetObject(GetStockObject(DEFAULT_GUI_FONT),sizeof(LOGFONT),&logFont);
		logFont.lfHeight=-16;
		logFont.lfItalic=1;
		logFont.lfWeight=600;
		font.CreateFontIndirectA(&logFont);
		CPaintDC dc(this);
		int odc=dc.SaveDC();
		CFont* oldFont=dc.SelectObject(&font);
		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(RGB(128,128,200));
		dc.DrawText("For CherryTrail x86 only v2.1",&rc,DT_LEFT);
		dc.SelectObject(oldFont);
		font.DeleteObject();
		dc.RestoreDC(odc);
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBiosUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CBiosUpdateDlg::OnBnClickedUpdate()
{
	// TODO: Add your control notification handler code here
	//GUID CORM  = { 0x0ff0a55a, 0x0003, 0x0204, { 0x06, 0x02, 0x10, 0x15, 0x20, 0x01, 0x21, 0x00 } };
	//GUID BYTCR = { 0x0ff0a55a, 0x0003, 0x0204, { 0x06, 0x02, 0x10, 0x0b, 0x20, 0x00, 0x21, 0x00 } };
	GUID CHT3  = { 0x0ff0a55a, 0x0003, 0x0204, { 0x06, 0x02, 0x10, 0x0e, 0x20, 0x00, 0x21, 0x00 } };

	char buff[16]={0};

	m_bExistKey = FALSE;
	CString szBios;
	EnableMenuItem(::GetSystemMenu(m_hWnd,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_DISABLED);
	GetDlgItem(IDC_SNCHK)->EnableWindow(0);
	GetDlgItem(IDC_UPDATE)->EnableWindow(0);
	GetDlgItem(IDC_BROWSE)->EnableWindow(0);
	SnRefresh();
	if (m_szPath.GetLength()<=0) 
	{
		MessageBox("No bios firmware found!","Error",MB_ICONSTOP);
		EnableMenuItem(::GetSystemMenu(m_hWnd,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_ENABLED);
		GetDlgItem(IDC_SNCHK)->EnableWindow();
		GetDlgItem(IDC_UPDATE)->EnableWindow();
		GetDlgItem(IDC_BROWSE)->EnableWindow();
		return;
	}
	CFile fp;
	if (!fp.Open(m_szPath,CFile::modeRead|CFile::typeBinary))
	{
		MessageBox("Bios file not found!","Error",MB_ICONERROR);
		EnableMenuItem(::GetSystemMenu(m_hWnd,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_ENABLED);
		GetDlgItem(IDC_SNCHK)->EnableWindow();
		GetDlgItem(IDC_UPDATE)->EnableWindow();
		GetDlgItem(IDC_BROWSE)->EnableWindow();
		return;
	}
	ULONG len=(ULONG)fp.GetLength();
	if (len != 8388608)
	{
		MessageBox("Bios file is invalid!","Error",MB_ICONERROR);
		EnableMenuItem(::GetSystemMenu(m_hWnd,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_ENABLED);
		GetDlgItem(IDC_SNCHK)->EnableWindow();
		GetDlgItem(IDC_UPDATE)->EnableWindow();
		GetDlgItem(IDC_BROWSE)->EnableWindow();
		fp.Close();
		return;
	}
	fp.Seek(0x10,SEEK_SET);
	fp.Read(buff,16);
	if (memcmp(buff,(char*)&CHT3,16))
	{
		MessageBox("Bios file is invalid!","Error",MB_ICONERROR);
		EnableMenuItem(::GetSystemMenu(m_hWnd,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_ENABLED);
		GetDlgItem(IDC_SNCHK)->EnableWindow();
		GetDlgItem(IDC_UPDATE)->EnableWindow();
		GetDlgItem(IDC_BROWSE)->EnableWindow();
		fp.Close();
		return;
	}
	fp.Seek(0x42,SEEK_SET);
	fp.Read(&m_nBiosSize,4);
	m_nBiosSize >>= 16;
	m_nBiosSize <<= 12;
	m_nBiosSize = 0x800000 - m_nBiosSize;
	fp.Close();
	SetCurrentDirectory(m_szTempDir);
	CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)KeyThread,this,0,NULL));
}


UINT CBiosUpdateDlg::KeyThread(LPVOID lp)
{
	CBiosUpdateDlg* p = (CBiosUpdateDlg*)lp;

	BOOL retval;
	PROCESS_INFORMATION pi={0};
	STARTUPINFO si={0};
	SECURITY_ATTRIBUTES sa={0};
	HANDLE hReadPipe,hWritePipe;
	sa.bInheritHandle=TRUE;
	sa.nLength=sizeof SECURITY_ATTRIBUTES;
	sa.lpSecurityDescriptor=NULL;
	retval=CreatePipe(&hReadPipe,&hWritePipe,&sa,0);
	si.cb=sizeof STARTUPINFO;
	si.wShowWindow=SW_HIDE;
	si.dwFlags=STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.hStdOutput=si.hStdError=hWritePipe;
	DWORD dwLen,dwRead,retCode1=-1,retCode2=-1,retDet=-1;
	UINT nActualBiosSize=0,nLock=0;
	char buff[256] = {0},szErrMsg[256]={0};
	CFile fp;
	CFile fp1,fp2;
	DWORD fLen;
	BYTE* fBuff;
	CString cmd;
	SetCurrentDirectory(p->m_szTempDir);
	retval=CreateProcess(NULL,"cmd.exe /c fptw.exe -i",&sa,&sa,TRUE,0,NULL,NULL,&si,&pi);
	if(retval)
	{
		WaitForSingleObject(pi.hThread,INFINITE);//�ȴ�������ִ�����
		GetExitCodeProcess(pi.hProcess,&retDet);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		if (retDet)
		{
			strcpy(szErrMsg,"fptw.exe not found or not support current platform!");
			goto end;
		}
		dwLen=GetFileSize(hReadPipe,NULL);
		char* szRegion = new char[dwLen];
		ReadFile(hReadPipe,szRegion,dwLen,&dwRead,NULL);
		if (dwRead)
		{
			char* szToken=strstr(szRegion,"BIOS       - Base: ");
			char szLen[9]={0};
			if (szToken)
			{
				strncpy(szLen,szToken+19,8);
				sscanf(szLen,"%x",&nActualBiosSize);
				nActualBiosSize = 0x800000 - nActualBiosSize;
			}
		}
		delete szRegion;
	}
	if (p->m_nBiosSize != nActualBiosSize)
	{
		sprintf(szErrMsg,"Current bios rom layout did not match, old size=0x%X, new size=0x%X!",nActualBiosSize,p->m_nBiosSize);
		goto end;
	}

	retval=CreateProcess(NULL,"cmd.exe /c fptw.exe -dumplock",&sa,&sa,TRUE,0,NULL,NULL,&si,&pi);
	if(retval)
	{
		WaitForSingleObject(pi.hThread,INFINITE);//�ȴ�������ִ�����
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		dwLen=GetFileSize(hReadPipe,NULL);
		char* szRegion = new char[dwLen];
		ReadFile(hReadPipe,szRegion,dwLen,&dwRead,NULL);
		if (dwRead)
		{
			char* szToken=strstr(szRegion,"Host CPU master:    ");
			int a,b,c,d,nCPU=0,nTXE=0;
			if (szToken)
			{
				szToken += 20;
				if (4 == sscanf(szToken,"%x %x %x %x",&a,&b,&c,&d))
				{
					nCPU = (a<<24) + (b<<16) + (c<<8) + d;
				}
			}
			szToken=strstr(szRegion,"TXE region master:  ");
			if (szToken)
			{
				szToken += 20;
				if (4 == sscanf(szToken,"%x %x %x %x",&a,&b,&c,&d))
				{
					nTXE = (a<<24) + (b<<16) + (c<<8) + d;
				}
			}
			if (nCPU == 0x0B0A && nTXE == 0x0D0C)
			{
				nLock = 1;
			}
		}
		p->SetDlgItemText(IDC_STATUS,nLock?"TXE/ME is locked":"TXE/ME is unlocked");
		delete szRegion;
	}
	Sleep(2000);
	//////////////////////////////////////////////////////////////////////
	retval=CreateProcess(NULL,"cmd.exe /c Check.exe",&sa,&sa,TRUE,0,NULL,NULL,&si,&pi);
	if(retval)
	{
		WaitForSingleObject(pi.hThread,INFINITE);//�ȴ�������ִ�����
		GetExitCodeProcess(pi.hProcess,&retCode1);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		if (retCode1 != 0)
		{
			p->m_bExistKey = FALSE;
			p->SetDlgItemText(IDC_STATUS,"Product key not found!");
		}
		else
		{
			p->SetDlgItemText(IDC_STATUS,"Found product key, saving...");
			dwLen=GetFileSize(hReadPipe,NULL);
			char *buff=new char [dwLen+1];
			char dpk[30]={0};
			char* vptr,*token="Product key:       ";
			memset(buff,0,dwLen+1);
			retval=ReadFile(hReadPipe,buff,dwLen,&dwRead,NULL);
			vptr=strstr(buff+700,token);
			if (vptr)
			{
				vptr +=strlen(token);
				strncpy(dpk,vptr,29);
				char szDPK[20]={0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								0x1d,0x00,0x00,0x00};
				fp.Open("key.bin",CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
				fp.Write(szDPK,20);
				fp.Write(dpk,29);
				fp.Close();
				p->m_bExistKey = TRUE;
			}
			delete buff;
		}
	}

	if (!fp1.Open(p->m_szPath,CFile::modeRead|CFile::typeBinary))
	{
		strcpy(szErrMsg,"Can't find the firmware, please check whether is exist!");
		goto end;
	}
	fLen=(DWORD)fp1.GetLength();
	fBuff = new BYTE[fLen];
	fp1.Read(fBuff,fLen);
	fp1.Close();
	if (!fp2.Open("fw.bin",CFile::modeCreate|CFile::modeReadWrite))
	{
		delete fBuff;
		strcpy(szErrMsg,"Open firmware file failed!");
		goto end;
	}
	fp2.Write(fBuff,fLen);
	fp2.Close();
	delete fBuff;

	if (nLock == 0)
	{
		cmd="cmd.exe /c fptw.exe -f fw.bin";
	}
	else
	{
		cmd="cmd.exe /c fptw.exe -f fw.bin -bios";
	}
	p->SetDlgItemText(IDC_STATUS,"Programming flash");
	sa.bInheritHandle=0;
	si.wShowWindow=SW_SHOW;
	retval=CreateProcess(NULL,(LPSTR)(LPCSTR)cmd,&sa,&sa,0,0,NULL,NULL,&si,&pi);
	WaitForSingleObject(pi.hThread,INFINITE);
	GetExitCodeProcess(pi.hProcess,&retCode1);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	DeleteFile("fw.bin");
	if (retCode1 == 0)
	{
		p->SetDlgItemText(IDC_STATUS,"Update successfully");
		Sleep(1000);
	}
	else
	{
		p->SetDlgItemText(IDC_STATUS,"Update failed");
		strcpy(szErrMsg,"update BIOS firmware failed!");
		goto end;
	}

	si.wShowWindow=SW_HIDE;
	if (p->m_nSN == BST_CHECKED)
	{
		if (p->m_strSSN.GetLength())
		{
			strcpy(buff,"cmd.exe /c amidewin.exe /ss \"");
			strcat(buff,(LPSTR)(LPCSTR)p->m_strSSN);
			strcat(buff,"\"");
			retval=CreateProcess(NULL,buff,&sa,&sa,0,0,NULL,NULL,&si,&pi);
			WaitForSingleObject(pi.hThread,INFINITE);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}

		if (p->m_strBSN.GetLength())
		{
			strcpy(buff,"cmd.exe /c amidewin.exe /bs \"");
			strcat(buff,(LPSTR)(LPCSTR)p->m_strBSN);
			strcat(buff,"\"");
			retval=CreateProcess(NULL,buff,&sa,&sa,0,0,NULL,NULL,&si,&pi);
			WaitForSingleObject(pi.hThread,INFINITE);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
	}

	if (p->m_bExistKey)
	{
		if (fp.Open("key.bin",CFile::modeRead))
		{
			fp.Close();
			CreateProcess(NULL,"cmd.exe /c afuwin.exe /oad",&sa,&sa,0,0,NULL,NULL,&si,&pi);
			WaitForSingleObject(pi.hThread,INFINITE);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			CreateProcess(NULL,"cmd.exe /c afuwin.exe /akey.bin",&sa,&sa,0,0,NULL,NULL,&si,&pi);
			WaitForSingleObject(pi.hThread,INFINITE);
			GetExitCodeProcess(pi.hProcess,&retCode2);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			if (retCode2 == 0)
			{
				p->SetDlgItemText(IDC_STATUS,"Write product key successfully");
			}
			else
			{
				p->SetDlgItemText(IDC_STATUS,"Write product key failed");
			}
			DeleteFile("key.bin");
		}
	}
end:
	CloseHandle(hWritePipe);
	CloseHandle(hReadPipe);
	EnableMenuItem(::GetSystemMenu(p->m_hWnd,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_ENABLED);
	p->GetDlgItem(IDC_SNCHK)->EnableWindow();
	p->GetDlgItem(IDC_UPDATE)->EnableWindow();
	p->GetDlgItem(IDC_BROWSE)->EnableWindow();
/*
	if (retCode2 != 0 && retCode2 != -1)
	{
		p->MessageBox("Inject DPK into BIOS failed","DPK Error",MB_ICONERROR);
	}
	else if (!p->m_bExistKey)
	{
		p->MessageBox("DPK not found in BIOS","DPK Error",MB_ICONERROR);
	}
*/
	if (retCode1 == 0)
	{
		HANDLE hToken;
		TOKEN_PRIVILEGES tkp;
		//p->MessageBox("New BIOS require restart to take effect ","Update",MB_ICONINFORMATION);
		//SetCurrentDirectory(m_szTempDir);
		//Microsoft tools
#ifdef CHECK_EXE
		DeleteFile("Check.exe");
#endif
		//Intel tools
#ifdef FPTW_EXE
		DeleteFile("fptw.exe");
#endif
#ifdef IDRV_DLL
		DeleteFile("idrvdll.dll");
#endif
#ifdef PMX_DLL
		DeleteFile("pmxdll.dll");
#endif
#ifdef FPARTS_TXT
		DeleteFile("fparts.txt");
#endif
		//AMI tools
#ifdef AFUWIN_EXE
		DeleteFile("afuwin.exe");
#endif
#ifdef AMIDEWIN_EXE
		DeleteFile("amidewin.exe");
#endif
#ifdef AMIFLDRV32_DLL
		DeleteFile("amifldrv32.sys");
#endif
#ifdef AMIFLDRV64_DLL
		DeleteFile("amifldrv64.sys");
#endif
		Sleep(1000);
		OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken);
		LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken,FALSE,&tkp,sizeof(TOKEN_PRIVILEGES),NULL,0);
		ExitWindowsEx(EWX_FORCE|EWX_REBOOT,0);
		CloseHandle(hToken);
	}
	else
	{
		p->MessageBox(szErrMsg,"Update Error",MB_ICONERROR);
	}

	return 0;
}
void CBiosUpdateDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
//---------------------------------------------------------------------
	SetCurrentDirectory(m_szTempDir);
	//Microsoft tools
#ifdef CHECK_EXE
	DeleteFile("Check.exe");
#endif
	//Intel tools
#ifdef FPTW_EXE
	DeleteFile("fptw.exe");
#endif
#ifdef IDRV_DLL
	DeleteFile("idrvdll.dll");
#endif
#ifdef PMX_DLL
	DeleteFile("pmxdll.dll");
#endif
#ifdef FPARTS_TXT
	DeleteFile("fparts.txt");
#endif
	//AMI tools
#ifdef AFUWIN_EXE
	DeleteFile("afuwin.exe");
#endif
#ifdef AMIDEWIN_EXE
	DeleteFile("amidewin.exe");
#endif
#ifdef AMIFLDRV32_DLL
	DeleteFile("amifldrv32.sys");
#endif
#ifdef AMIFLDRV64_DLL
	DeleteFile("amifldrv64.sys");
#endif
}

void CBiosUpdateDlg::SnRefresh()
{
	// TODO: Add your control notification handler code here
	BOOL retval;
	PROCESS_INFORMATION pi={0};
	STARTUPINFO si={0};
	SECURITY_ATTRIBUTES sa={0};
	HANDLE hReadPipe,hWritePipe;
	sa.bInheritHandle=TRUE;
	sa.nLength=sizeof SECURITY_ATTRIBUTES;
	sa.lpSecurityDescriptor=NULL;
	retval=CreatePipe(&hReadPipe,&hWritePipe,&sa,0);
	si.cb=sizeof STARTUPINFO;
	si.wShowWindow=SW_HIDE;
	si.dwFlags=STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.hStdOutput=si.hStdError=hWritePipe;
	DWORD dwLen,dwRead;

	retval=CreateProcess(NULL,"cmd.exe /c amidewin.exe /ss",&sa,&sa,TRUE,0,NULL,m_szTempDir,&si,&pi);
	if(retval)
	{
		WaitForSingleObject(pi.hThread,INFINITE);//�ȴ�������ִ�����
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		dwLen=GetFileSize(hReadPipe,NULL);
		char *buf=new char[dwLen+1];
		retval=ReadFile(hReadPipe,buf,dwLen,&dwRead,NULL);
		if (strlen(buf))
		{
			char* p=strchr(buf,'"');
			if (p)
			{
				p++;
				char* p2=strchr(p,'"');
				if (p2)
				{
					*p2 = 0;
					m_strSSN=p;
				}
			}
		}
		delete buf;
	}

	retval=CreateProcess(NULL,"cmd.exe /c amidewin.exe /bs",&sa,&sa,TRUE,0,NULL,m_szTempDir,&si,&pi);
	if(retval)
	{
		WaitForSingleObject(pi.hThread,INFINITE);//�ȴ�������ִ�����
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		dwLen=GetFileSize(hReadPipe,NULL);
		char *buf=new char[dwLen+1];
		retval=ReadFile(hReadPipe,buf,dwLen,&dwRead,NULL);
		if (strlen(buf))
		{
			char* p=strchr(buf,'"');
			if (p)
			{
				p++;
				char* p2=strchr(p,'"');
				if (p2)
				{
					*p2 = 0;
					m_strBSN=p;
				}
			}
		}
		delete buf;
	}
}

BOOL CBiosUpdateDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CBiosUpdateDlg::OnBnClickedBrowse()
{
	// TODO: Add your control notification handler code here
	SetCurrentDirectory(m_curPath);
	CFileDialog dlg(TRUE,0,0,6,"BIOS Firmware|*.bin||",this);
	dlg.m_ofn.lpstrInitialDir = m_curPath;
	if (dlg.DoModal() != IDOK)
	{
		return;
	}
	m_szPath = dlg.GetPathName();
	SetDlgItemText(IDC_PATH,m_szPath);
	GetCurrentDirectory(2048,m_curPath);
}

void CBiosUpdateDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	char szPath[2048]={0};
	int len = DragQueryFile(hDropInfo, 0, szPath, sizeof(szPath));
	if (len)
	{
		WIN32_FIND_DATA wfd;
		BOOL bValue = FALSE;
		HANDLE hFind = FindFirstFile(szPath, &wfd);
		if (hFind != INVALID_HANDLE_VALUE && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			//SetDlgItemText(IDC_PATH,"");
			//m_szPath="";
		}
		else if (hFind != INVALID_HANDLE_VALUE)
		{
			m_szPath=szPath;
			SetDlgItemText(IDC_PATH,m_szPath);
		}
		FindClose(hFind);
	}
	else
	{
		SetDlgItemText(IDC_PATH,"");
		m_szPath="";
	}
	CDialog::OnDropFiles(hDropInfo);
}

void CBiosUpdateDlg::OnBnClickedSnchk()
{
	// TODO: Add your control notification handler code here
	m_nSN = ((CButton*)GetDlgItem(IDC_SNCHK))->GetCheck();
}
