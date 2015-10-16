// BiosUpdateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BiosUpdate.h"
#include "BiosUpdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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

	char curPath[2048]={0};
	GetCurrentDirectory(2048,curPath);
	m_curPath=curPath;
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
	GUID CORM  = { 0x0ff0a55a, 0x0003, 0x0204, { 0x06, 0x02, 0x10, 0x15, 0x20, 0x01, 0x21, 0x00 } };
	GUID BYTCR = { 0x0ff0a55a, 0x0003, 0x0204, { 0x06, 0x02, 0x10, 0x0b, 0x20, 0x00, 0x21, 0x00 } };

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
	if (memcmp(buff,(char*)&BYTCR,16) && memcmp(buff,(char*)&CORM,16))
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
	CString fw=p->m_curPath,cmd;
	retval=CreateProcess(NULL,"cmd.exe /c fptw.exe -i",&sa,&sa,TRUE,0,NULL,p->m_curPath,&si,&pi);
	if(retval)
	{
		WaitForSingleObject(pi.hThread,INFINITE);//等待命令行执行完毕
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

	retval=CreateProcess(NULL,"cmd.exe /c fptw.exe -dumplock",&sa,&sa,TRUE,0,NULL,p->m_curPath,&si,&pi);
	if(retval)
	{
		WaitForSingleObject(pi.hThread,INFINITE);//等待命令行执行完毕
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
	p->SetDlgItemText(IDC_STATUS,"Saving product key");
	retval=CreateProcess(NULL,"powershell.exe (get-wmiobject softwarelicensingservice).OA3xOriginalProductKey",&sa,&sa,TRUE,0,NULL,p->m_curPath,&si,&pi);
	if(retval)
	{
		WaitForSingleObject(pi.hThread,INFINITE);//等待命令行执行完毕
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		dwLen=GetFileSize(hReadPipe,NULL);
		CString szCmdLine(' ',dwLen);
		retval=ReadFile(hReadPipe,(LPSTR)(LPCTSTR)szCmdLine,dwLen,&dwRead,NULL);
		if (szCmdLine.GetLength() == 31)
		{
			char szDPK[20]={0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0x1d,0x00,0x00,0x00};
			fp.Open("key.bin",CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
			fp.Write(szDPK,20);
			fp.Write((LPSTR)(LPCTSTR)szCmdLine,29);
			fp.Close();
			p->m_bExistKey = TRUE;
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
	fw += "\\fw.bin";
	if (!fp2.Open(fw,CFile::modeCreate|CFile::modeReadWrite))
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
	retval=CreateProcess(NULL,(LPSTR)(LPCSTR)cmd,&sa,&sa,0,0,NULL,p->m_curPath,&si,&pi);
	WaitForSingleObject(pi.hThread,INFINITE);
	GetExitCodeProcess(pi.hProcess,&retCode1);
	DeleteFile(fw);
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
			retval=CreateProcess(NULL,buff,&sa,&sa,0,0,NULL,p->m_curPath,&si,&pi);
			WaitForSingleObject(pi.hThread,INFINITE);
		}

		if (p->m_strBSN.GetLength())
		{
			strcpy(buff,"cmd.exe /c amidewin.exe /bs \"");
			strcat(buff,(LPSTR)(LPCSTR)p->m_strBSN);
			strcat(buff,"\"");
			retval=CreateProcess(NULL,buff,&sa,&sa,0,0,NULL,p->m_curPath,&si,&pi);
			WaitForSingleObject(pi.hThread,INFINITE);
		}
	}

	if (p->m_bExistKey)
	{
		if (fp.Open("key.bin",CFile::modeRead))
		{
			fp.Close();
			CreateProcess(NULL,"cmd.exe /c afuwin.exe /oad",&sa,&sa,0,0,NULL,p->m_curPath,&si,&pi);
			WaitForSingleObject(pi.hThread,INFINITE);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			CreateProcess(NULL,"cmd.exe /c afuwin.exe /akey.bin",&sa,&sa,0,0,NULL,p->m_curPath,&si,&pi);
			WaitForSingleObject(pi.hThread,INFINITE);
			GetExitCodeProcess(pi.hProcess,&retCode2);
			if (retCode2 == 0)
			{
				p->SetDlgItemText(IDC_STATUS,"Write product key successfully");
			}
			else
			{
				p->SetDlgItemText(IDC_STATUS,"Write product key failed");
			}
		}
	}
end:
	DeleteFile("key.bin");
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

	retval=CreateProcess(NULL,"cmd.exe /c amidewin.exe /ss",&sa,&sa,TRUE,0,NULL,m_curPath,&si,&pi);
	if(retval)
	{
		WaitForSingleObject(pi.hThread,INFINITE);//等待命令行执行完毕
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

	retval=CreateProcess(NULL,"cmd.exe /c amidewin.exe /bs",&sa,&sa,TRUE,0,NULL,m_curPath,&si,&pi);
	if(retval)
	{
		WaitForSingleObject(pi.hThread,INFINITE);//等待命令行执行完毕
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
	char szcurPath[2048]={0};
	GetCurrentDirectory(2048,szcurPath);
	CFileDialog dlg(TRUE,0,0,6,"BIOS Firmware|*.bin||",this);
	dlg.m_ofn.lpstrInitialDir = szcurPath;
	if (dlg.DoModal() != IDOK)
	{
		m_szPath="";
		return;
	}
	m_szPath = dlg.GetPathName();
	SetDlgItemText(IDC_PATH,m_szPath);
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
			SetDlgItemText(IDC_PATH,"");
			m_szPath="";
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
	}
	CDialog::OnDropFiles(hDropInfo);
}

void CBiosUpdateDlg::OnBnClickedSnchk()
{
	// TODO: Add your control notification handler code here
	m_nSN = ((CButton*)GetDlgItem(IDC_SNCHK))->GetCheck();
}
