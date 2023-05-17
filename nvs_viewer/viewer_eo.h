#pragma once

#include "stdafx.h"
#include "alarm.h"



// viewer_eo 대화 상자

class viewer_eo : public CDialogEx
{
	DECLARE_DYNAMIC(viewer_eo)

public:
	viewer_eo(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~viewer_eo();
	
	alarm* dlg_alarm;


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIEWER_EO };
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
	CStatic m_pic_eo;
	CString str_path;

	int window_width;
	int window_height;
};
