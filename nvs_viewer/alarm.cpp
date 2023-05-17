// alarm.cpp: 구현 파일
//

#include "pch.h"
#include "nvs_viewer.h"
#include "alarm.h"
#include "afxdialogex.h"

#include "nvs_viewerDlg.h"

// alarm 대화 상자

IMPLEMENT_DYNAMIC(alarm, CDialogEx)

alarm::alarm(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ALARM, pParent)
{

}

alarm::~alarm()
{
}

void alarm::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ICO_RECORD, m_ico_record);
	DDX_Control(pDX, IDC_ICO_EVENT, m_ico_event);
	DDX_Control(pDX, IDC_ICO_FOLDER, m_ico_folder);
	DDX_Control(pDX, IDC_ICO_LINE, m_ico_line);
}


BEGIN_MESSAGE_MAP(alarm, CDialogEx)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR()
	ON_WM_CLOSE()
	ON_STN_CLICKED(IDC_ICO_LINE, &alarm::OnStnClickedIcoLine)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// alarm 메시지 처리기


int alarm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}


HBRUSH alarm::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
	{
		/*if (pWnd->GetDlgCtrlID() == IDC_STATIC_RECORD)
		{
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}*/
	}
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}


BOOL alarm::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//GetDlgItem(IDC_ICON_RECORD)->MoveWindow(10, 10, 20, 20);

	GetDlgItem(IDC_ICO_RECORD)->MoveWindow(10, 10, 40, 40);
	GetDlgItem(IDC_ICO_EVENT)->MoveWindow(50, 10, 20, 20);
	GetDlgItem(IDC_ICO_FOLDER)->MoveWindow(90, 10, 40, 30);
	GetDlgItem(IDC_ICO_LINE)->MoveWindow(130, 10, 40, 30);

	m_icon_record_true = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_REC_TRUE));
	m_icon_record_false = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_REC_FALSE));
	m_icon_event_true = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_EVENT_TRUE));
	m_icon_event_false = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_EVENT_FALSE));
	m_icon_folder = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_FOLDER));
	m_icon_folder_warning = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_FOLDER_WARNING));
	m_icon_line_true = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_LINE_TRUE));
	m_icon_line_false = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_LINE_FALSE));

	m_ico_record.SetIcon(m_icon_record_false);
	m_ico_event.SetIcon(m_icon_event_false);
	m_ico_folder.SetIcon(m_icon_folder);
	m_ico_line.SetIcon(m_icon_line_false);

	SetTimer(1, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL alarm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialogEx::PreTranslateMessage(pMsg);
}


void alarm::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CnvsviewerDlg* pDlg = (CnvsviewerDlg*)AfxGetMainWnd();

	pDlg->DestroyWindow();


	CDialogEx::OnClose();
}


void alarm::OnStnClickedIcoLine()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_ico_line.GetIcon() == m_icon_line_false)
	{
		int nn = 0;
		m_ico_line.SetIcon(m_icon_line_true);
	}

	else if (m_ico_line.GetIcon() == m_icon_line_true)
	{
		int n = 0;
		m_ico_line.SetIcon(m_icon_line_false);
	}
}


void alarm::display_record(bool nCase)
{
	if (nCase == false)
		m_ico_record.SetIcon(m_icon_record_false);

	else if (nCase == true)
		m_ico_record.SetIcon(m_icon_record_true);
}

void alarm::display_event(bool nCase)
{
	if (nCase == false)
		m_ico_event.SetIcon(m_icon_event_false);

	else if (nCase == true)
		m_ico_event.SetIcon(m_icon_event_true);
}

void alarm::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ULARGE_INTEGER avail, total, free;

	CString strVolDrive;
	GetDiskFreeSpaceEx(str_path, &avail, &total, &free);

	//MBytes로 처리
	double disk_size = (int)(total.QuadPart >> 30);

	//GBytes total.QuadPart>>30 , KBytes : total.QuadPart>>10
	//disk_free_size;
	double disk_free_size = (int)(free.QuadPart >> 30);

	if (((disk_size - disk_free_size) / disk_size) * 100 > 80)
		m_ico_folder.SetIcon(m_icon_folder_warning);

	CDialogEx::OnTimer(nIDEvent);
}
