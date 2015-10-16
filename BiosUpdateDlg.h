// BiosUpdateDlg.h : header file
//

#pragma once


// CBiosUpdateDlg dialog
class CBiosUpdateDlg : public CDialog
{
// Construction
public:
	CBiosUpdateDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_BIOSUPDATE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	BOOL  m_bExistKey;
	BOOL  m_bIsx64;
	UINT  m_nBiosSize;
	CString m_szPath,m_curPath,m_strSSN,m_strBSN;
	int   m_nTxe,m_nSN;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedUpdate();
	static UINT KeyThread(LPVOID lp);
	afx_msg void OnDestroy();
	void SnRefresh();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedSnchk();
};
