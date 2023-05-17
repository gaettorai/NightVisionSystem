#pragma once


// alarm 대화 상자

class alarm : public CDialogEx
{
	DECLARE_DYNAMIC(alarm)

public:
	alarm(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~alarm();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ALARM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnClose();
	afx_msg void OnStnClickedIcoLine();

	HICON m_icon_record_true;
	HICON m_icon_record_false;
	HICON m_icon_event_true;
	HICON m_icon_event_false;
	HICON m_icon_folder;
	HICON m_icon_folder_warning;
	HICON m_icon_line_true;
	HICON m_icon_line_false;

	CStatic m_ico_record;
	CStatic m_ico_event;
	CStatic m_ico_folder;
	CStatic m_ico_line;

	void display_record(bool nCase);
	void display_event(bool nCase);

	CString str_path;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
