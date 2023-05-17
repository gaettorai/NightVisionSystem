#pragma once


// viewer_ir 대화 상자

class viewer_ir : public CDialogEx
{
	DECLARE_DYNAMIC(viewer_ir)

public:
	viewer_ir(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~viewer_ir();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIEWER_IR };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	CStatic m_pic_ir;

	BOOL SAVE_IR;

	int window_width;
	int window_height;
};
