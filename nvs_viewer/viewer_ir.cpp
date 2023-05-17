// viewer_ir.cpp: 구현 파일
//

#include "pch.h"
#include "nvs_viewer.h"
#include "viewer_ir.h"
#include "afxdialogex.h"

#include "nvs_viewerDlg.h"

// viewer_ir 대화 상자

IMPLEMENT_DYNAMIC(viewer_ir, CDialogEx)

viewer_ir::viewer_ir(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIEWER_IR, pParent)
{

}

viewer_ir::~viewer_ir()
{
}

void viewer_ir::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_IR, m_pic_ir);
}


BEGIN_MESSAGE_MAP(viewer_ir, CDialogEx)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// viewer_ir 메시지 처리기


int viewer_ir::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


HBRUSH viewer_ir::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


void viewer_ir::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnMouseMove(nFlags, point);
}


BOOL viewer_ir::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	GetDlgItem(IDC_PIC_IR)->MoveWindow(0, 0, window_width, window_height);

	SAVE_IR = FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL viewer_ir::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN) // ENTER키 눌릴 시
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE) // ESC키 눌릴 시
			return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void viewer_ir::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CnvsviewerDlg* pDlg = (CnvsviewerDlg*)AfxGetMainWnd();

	pDlg->DestroyWindow();

	CDialogEx::OnClose();
}
