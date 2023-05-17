
// nvs_viewerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "nvs_viewer.h"
#include "nvs_viewerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CAMERA_IP	"192.168.255.108"
#define CAMERA_PORT	8002

#define SHM_DISPL_VALUE	0x1F0
#define	MEM_READ	0x30
#define	MEM_READ_REPLY	(0x30+0x80)

#define LIBAVFORMAT_INTERRUPT_OPEN_TIMEOUT_MS 30
#define LIBAVFORMAT_INTERRUPT_READ_TIMEOUT_MS 30

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
HWND hCommWnd;

PROTOCOL1 Protocol;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CnvsviewerDlg 대화 상자



CnvsviewerDlg::CnvsviewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NVS_VIEWER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iStopBit = 0;
	m_iSerialPort = 2;
	m_iParity = 0;
	m_iDataBit = 3;
	m_iBaudRate = 1;

}

void CnvsviewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CnvsviewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_COMM_READ, &CnvsviewerDlg::OnReceive)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CnvsviewerDlg 메시지 처리기

BOOL CnvsviewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// 폴더 경로 확인
	///////////////////////////////////////////////////////////////////////////////
	GetModuleFileName(NULL, chFilePath, 256);
	strFolderPath = chFilePath;

	strFolderPath = strFolderPath.Left(strFolderPath.ReverseFind('\\'));
	strFolderPath = strFolderPath.Left(strFolderPath.ReverseFind('\\'));
	save_folder = strFolderPath + _T("\\Data");
	///////////////////////////////////////////////////////////////////////////////

	// BASE 프로그램 HIED
	SetWindowPos(&wndTop, 0, 0, 10, 10, SWP_HIDEWINDOW);
	// BASE DIALOG 숨기기
	PostMessage(WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM);

	// 모니터 갯수 확인
	///////////////////////////////////////////////////////////////////////////////
	MonitorCount = GetSystemMetrics(SM_CMONITORS);

	DISPLAY_DEVICE dd;
	DEVMODE dm;

	MonitorRect = NULL;
	MonitorRect = new CRect[MonitorCount];

	for (int i = 0, j = 0; j < MonitorCount; i++)
	{
		memset(&dd, 0, sizeof(DISPLAY_DEVICE));
		dd.cb = sizeof(DISPLAY_DEVICE);

		EnumDisplayDevices(NULL, i, &dd, 0);
		memset(&dm, 0, sizeof(DEVMODE));

		if (EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm))
		{
			MonitorRect[j].left = dm.dmPosition.x;
			MonitorRect[j].top = dm.dmPosition.y;
			MonitorRect[j].right = dm.dmPosition.x + dm.dmPelsWidth;
			MonitorRect[j].bottom = dm.dmPosition.y + dm.dmPelsHeight;
			j++;
		}
	}

	volume_folder = strFolderPath.Left(strFolderPath.Find('\\'));

	str_zoom_value = AfxGetApp()->GetProfileStringW(_T("CAMERA"), _T("ZOOM_POSITION"));

	// zoom 값을 못 읽어왔을 경우
	// 기본 zoom 값은 1로 설정
	if (str_zoom_value == _T(""))
		zoom_value = 1;

	else
		zoom_value = _ttoi(str_zoom_value);

	// monitor가 2개로 인식이 된 경우
	if (MonitorCount == 2)
	{
		//좌측 모니터가 더 작고, 우측 모니터가 더 크다. 
		// 좌측 모니터에 eo, 우측 모니터에 ir 영상 display
		if (MonitorRect[0].right > MonitorRect[1].right)
		{
			eo_x = MonitorRect[0].left;
			eo_y = MonitorRect[0].top;
			eo_width = MonitorRect[0].Width();
			eo_height = MonitorRect[0].bottom;

			ir_x = MonitorRect[1].left;
			ir_y = MonitorRect[1].top;
			ir_width = MonitorRect[1].Width();
			ir_height = MonitorRect[1].Height();
		}

		else if (MonitorRect[1].right > MonitorRect[0].right)
		{
			eo_x = MonitorRect[1].left;
			eo_y = MonitorRect[1].top;
			eo_width = MonitorRect[1].Width();
			eo_height = MonitorRect[1].Height();

			ir_x = MonitorRect[0].left;
			ir_y = MonitorRect[0].top;
			ir_width = MonitorRect[0].Width();
			ir_height = MonitorRect[0].Height();
		}

		dlg_viewer_ir = new viewer_ir;
		dlg_viewer_ir->window_width = ir_width;
		dlg_viewer_ir->window_height = ir_height;
		dlg_viewer_ir->Create(IDD_VIEWER_IR, this);
		dlg_viewer_ir->MoveWindow(ir_x, ir_y, ir_width, ir_height);
		dlg_viewer_ir->ShowWindow(SW_SHOW);
	}

	else if (MonitorCount == 1)
	{
		eo_x = MonitorRect[0].left;
		eo_y = MonitorRect[0].top;
		eo_width = MonitorRect[0].Width();
		eo_height = MonitorRect[0].Height();

		dlg_viewer_ir = new viewer_ir;
		dlg_viewer_ir->window_width = eo_width;
		dlg_viewer_ir->window_height = eo_height;
		dlg_viewer_ir->Create(IDD_VIEWER_IR, this);
		dlg_viewer_ir->MoveWindow(eo_x, eo_y, eo_width, eo_height);
		dlg_viewer_ir->ShowWindow(SW_HIDE);
	}

	dlg_viewer_eo = new viewer_eo;
	dlg_viewer_eo->str_path = volume_folder;
	dlg_viewer_eo->window_width = eo_width;
	dlg_viewer_eo->window_height = eo_height;
	dlg_viewer_eo->Create(IDD_VIEWER_EO, this);
	dlg_viewer_eo->MoveWindow(eo_x, eo_y, eo_width, eo_height);
	dlg_viewer_eo->ShowWindow(SW_SHOW);
	///////////////////////////////////////////////////////////////////////////////

	// 232 통신 
	m_comm = new CCommThread;

	// 이미지 설정
	///////////////////////////////////////////////////////////////////////////////
	m_pBitmapInfo_eo = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO)];
	m_pBitmapInfo_ir = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFO)];

	EO_hdc = dlg_viewer_eo->m_pic_eo.GetDC()->m_hDC;
	IR_hdc = dlg_viewer_ir->m_pic_ir.GetDC()->m_hDC;

	SetStretchBltMode(EO_hdc, COLORONCOLOR);
	SetStretchBltMode(IR_hdc, COLORONCOLOR);
	///////////////////////////////////////////////////////////////////////////////


	// 카메라 RTSP 접속
	///////////////////////////////////////////////////////////////////////////////
	//	EO_VideoStream = "rtsp://admin:goend@223.171.56.72:80/cam/re34almonitor?channel=1&subtype=0";
	EO_VideoStream = "rtsp://admin:goend@192.168.255.108:554/cam/realmonitor?channel=1&subtype=0";
	IR_VideoStream = "rtsp://admin:admin@192.168.1.168:554/profile2/mjpeg";

	EO_VideoCap = new VideoCapture();
	IR_VideoCap = new VideoCapture();

	EO_VideoCap = new VideoCapture(EO_VideoStream);
	IR_VideoCap = new VideoCapture(IR_VideoStream);
	///////////////////////////////////////////////////////////////////////////////

	// 카메라 설정
	///////////////////////////////////////////////////////////////////////////////
	// MOG2 
	pMog2 = createBackgroundSubtractorMOG2();	// 그림자 제거 (FALSE)

	// IR 이미지 DISPLAY 이후 활성화
	// IR EVENT THREAD 진행 시 필요
	chk_ir = false;
	chk_eo = false;

	// 설정
	if (EO_VideoCap->isOpened())
		ConnectEO();

	if (IR_VideoCap->isOpened())
		ConnectIR();

	setBGImg();
	//ConnectEO();
	//ConnectIR();
	//ConnectSerial();

	event_count = 0;

	// 스레드 START
	///////////////////////////////////////////////////////////////////////////////
	EO = AfxBeginThread(EO_Thread, this);
	IR = AfxBeginThread(IR_Thread, this);
	IR_EVENT = AfxBeginThread(IR_EVENT_Thread, this);
	EO_EVENT = AfxBeginThread(EO_EVENT_Thread, this);

	pen_roi.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));

	SetTimer(1, 1, NULL);

	//
	eo_first_point.x = 100;
	eo_first_point.y = 100;

	eo_second_point.x = 200;
	eo_second_point.y = 200;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CnvsviewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CnvsviewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CnvsviewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CnvsviewerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	hCommWnd = this->m_hWnd;

	return 0;
}

void CnvsviewerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	//SetFocus();

	if (nIDEvent == 1)
	{
		if (chk_ir == true)
		{
			//imshow("back", back);
			//imshow("diff", sub_frame);
			//imshow("gray", gray);
			//imshow("mog2", MaskMog2);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CnvsviewerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN) // ENTER키 눌릴 시
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE) // ESC키 눌릴 시
			return TRUE;

		if (pMsg->wParam == VK_F2 && dlg_viewer_ir->SAVE_IR == false)
		{
			Catch_event(false, 0);
			dlg_viewer_ir->SAVE_IR = TRUE;
			dlg_viewer_eo->dlg_alarm->display_record(true);
		}

		else if (pMsg->wParam == VK_F2 && dlg_viewer_ir->SAVE_IR == true)
		{
			dlg_viewer_ir->SAVE_IR = FALSE;
			dlg_viewer_eo->dlg_alarm->display_record(false);
			IR_VideoWrite.release();
		}

	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CnvsviewerDlg::ConnectEO()
{
	if (!EO_VideoCap->read(EO_Image))
	{

	}

	else
	{
		EO_FPS = EO_VideoCap->get(cv::CAP_PROP_FPS);
		EO_VideoWidth = EO_VideoCap->get(cv::CAP_PROP_FRAME_WIDTH);
		EO_VideoHeight = EO_VideoCap->get(cv::CAP_PROP_FRAME_HEIGHT);

		m_pBitmapInfo_eo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_pBitmapInfo_eo->bmiHeader.biPlanes = 1;
		m_pBitmapInfo_eo->bmiHeader.biBitCount = 24;
		m_pBitmapInfo_eo->bmiHeader.biCompression = BI_RGB;
		m_pBitmapInfo_eo->bmiHeader.biSizeImage = 0;
		m_pBitmapInfo_eo->bmiHeader.biXPelsPerMeter = 0;
		m_pBitmapInfo_eo->bmiHeader.biYPelsPerMeter = 0;
		m_pBitmapInfo_eo->bmiHeader.biClrUsed = 0;
		m_pBitmapInfo_eo->bmiHeader.biClrImportant = 0;
		m_pBitmapInfo_eo->bmiHeader.biWidth = EO_Image.cols;
		m_pBitmapInfo_eo->bmiHeader.biHeight = -EO_Image.rows;
	}
}

void CnvsviewerDlg::ConnectIR()
{
	if (!IR_VideoCap->read(IR_Image))
	{

	}

	else
	{
		// 영상 저장 설정 값
		IR_FPS = IR_VideoCap->get(cv::CAP_PROP_FPS);
		IR_VideoWidth = IR_VideoCap->get(cv::CAP_PROP_FRAME_WIDTH);
		IR_VideoHeight = IR_VideoCap->get(cv::CAP_PROP_FRAME_HEIGHT);

		// 영상 출력 설정 값
		m_pBitmapInfo_ir->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_pBitmapInfo_ir->bmiHeader.biPlanes = 1;
		m_pBitmapInfo_ir->bmiHeader.biBitCount = 24;
		m_pBitmapInfo_ir->bmiHeader.biCompression = BI_RGB;
		m_pBitmapInfo_ir->bmiHeader.biSizeImage = 0;
		m_pBitmapInfo_ir->bmiHeader.biXPelsPerMeter = 0;
		m_pBitmapInfo_ir->bmiHeader.biYPelsPerMeter = 0;
		m_pBitmapInfo_ir->bmiHeader.biClrUsed = 0;
		m_pBitmapInfo_ir->bmiHeader.biClrImportant = 0;
		m_pBitmapInfo_ir->bmiHeader.biWidth = IR_Image.cols;
		m_pBitmapInfo_ir->bmiHeader.biHeight = -IR_Image.rows;
	}
}

void CnvsviewerDlg::ConnectSerial()
{
	if (m_comPort.m_bConnected == FALSE)//포트가 닫혀 있을 경우에만 포트를 열기 위해
	{
		if (m_comPort.OpenPort(byIndexComPort(m_iSerialPort), byIndexBaud(m_iBaudRate), byIndexData(m_iDataBit), byIndexStop(m_iStopBit), byIndexParity(m_iParity)) == TRUE)
		{
			ThreadWatchComm(m_comm);
		}
	}
	else
	{
		AfxMessageBox(_T("Already Port open"));
	}
}

CString CnvsviewerDlg::byIndexComPort(int xPort)
{
	CString PortName = _T("");
	switch (xPort)
	{
	case 0:		PortName = "COM1"; 			break;

	case 1:		PortName = "COM2";			break;

	case 2:		PortName = "COM3"; 			break;

	case 3:		PortName = "COM4";			break;

	case 4:		PortName = "\\\\.\\COM5"; 	break;

	case 5:		PortName = "\\\\.\\COM6";	break;

	case 6:		PortName = "\\\\.\\COM7"; 	break;

	case 7:		PortName = "\\\\.\\COM8";	break;

	case 8:		PortName = "\\\\.\\COM9"; 	break;

	case 9:		PortName = "\\\\.\\COM10";	break;
	}

	return PortName;
}

DWORD CnvsviewerDlg::byIndexBaud(int xBaud)
{
	DWORD dwBaud = 0;
	switch (xBaud)
	{
	case 0:		dwBaud = CBR_4800;		break;

	case 1:		dwBaud = CBR_9600;		break;

	case 2:		dwBaud = CBR_14400;		break;

	case 3:		dwBaud = CBR_19200;		break;

	case 4:		dwBaud = CBR_38400;		break;

	case 5:		dwBaud = CBR_56000;		break;

	case 6:		dwBaud = CBR_57600;		break;

	case 7:		dwBaud = CBR_115200;	break;
	}

	return dwBaud;
}

BYTE CnvsviewerDlg::byIndexData(int xData)
{
	BYTE byData = 0;
	switch (xData)
	{
	case 0:	byData = 5;			break;

	case 1:	byData = 6;			break;

	case 2:	byData = 7;			break;

	case 3:	byData = 8;			break;
	}
	return byData;
}

BYTE CnvsviewerDlg::byIndexStop(int xStop)
{
	BYTE byStop = 0;
	if (xStop == 0)
	{
		byStop = ONESTOPBIT;
	}
	else
	{
		byStop = TWOSTOPBITS;
	}
	return byStop;
}

BYTE CnvsviewerDlg::byIndexParity(int xParity)
{
	BYTE byParity = 0;
	switch (xParity)
	{
	case 0:	byParity = NOPARITY;	break;

	case 1:	byParity = ODDPARITY;	break;

	case 2:	byParity = EVENPARITY;	break;
	}

	return byParity;
}

UINT CnvsviewerDlg::EO_Thread(LPVOID pParam)
{
	CnvsviewerDlg* dlg_eo = (CnvsviewerDlg*)pParam;

	while (1)
	{
		// 열려 있고
		if (dlg_eo->EO_VideoCap->isOpened())
		{
			// 이미지가 load 된 상태에서
			if (!dlg_eo->EO_VideoCap->read(dlg_eo->EO_Image))
			{

			}

			else
			{
				dlg_eo->chk_eo = true;

				// 이미지는 항상 Display 하고
				// 검출에 대한 박스는 치지 않는다.
				StretchDIBits(dlg_eo->EO_hdc, dlg_eo->MonitorRect[0].left, dlg_eo->MonitorRect[0].top, dlg_eo->MonitorRect[0].right, dlg_eo->MonitorRect[0].bottom - 100, 0, 0, dlg_eo->EO_Image.cols, dlg_eo->EO_Image.rows,
					dlg_eo->EO_Image.data, dlg_eo->m_pBitmapInfo_eo, DIB_RGB_COLORS, SRCCOPY);
			}
		}
	}

	return 0;
}

UINT CnvsviewerDlg::IR_Thread(LPVOID pParam)
{
	CnvsviewerDlg* dlg_ir = (CnvsviewerDlg*)pParam;

	while (1)
	{
		// 카메라 연결 되어 있고
		if (dlg_ir->IR_VideoCap->isOpened())
		{
			if (!dlg_ir->IR_VideoCap->read(dlg_ir->IR_Image))
			{
			}

			// 이미지가 읽혔을 때
			else
			{
				dlg_ir->chk_ir = true;
				// 이미지 저장 기능 활성화
				if (dlg_ir->dlg_viewer_ir->SAVE_IR == true)
				{
					dlg_ir->IR_VideoWrite << dlg_ir->IR_Image;
					dlg_ir->dlg_viewer_eo->dlg_alarm->display_record(true);
				}

				// 이미지 저장 기능 비 활성화
				else if (dlg_ir->dlg_viewer_ir->SAVE_IR == false)
					dlg_ir->dlg_viewer_eo->dlg_alarm->display_record(false);

				// roi 영역 그리기
				// line 활성화 된 경우에만 그려라.
				// 설정된 값에 따라

				// 이미지 그리기
				//if (dlg_ir->dlg_viewer_eo->dlg_alarm->m_ico_event.GetIcon() == dlg_ir->dlg_viewer_eo->dlg_alarm->m_icon_event_true)
				//	rectangle(dlg_ir->IR_Image, dlg_ir->rect, cv::Scalar(0, 0, 255), 1);

				//pDC_rectangle->Rectangle(dlg_ir->rect.x, dlg_ir->rect.y, dlg_ir->rect.width, dlg_ir->rect.height);
			}
		}
	}

	return 0;
}

UINT CnvsviewerDlg::IR_EVENT_Thread(LPVOID pParam)
{
	CnvsviewerDlg* dlg_ir = (CnvsviewerDlg*)pParam;

	while (1)
	{
		if (dlg_ir->chk_ir == true)
		{
			int count = 0;

			dlg_ir->pMog2->apply(dlg_ir->IR_Image, dlg_ir->MaskMog2);
			dlg_ir->pMog2->getBackgroundImage(dlg_ir->back);
			//dlg_ir->back = imread("bg.bmp");

			absdiff(dlg_ir->IR_Image, dlg_ir->back, dlg_ir->sub_frame);
			cvtColor(dlg_ir->sub_frame, dlg_ir->gray, COLOR_RGB2GRAY);
			threshold(dlg_ir->gray, dlg_ir->gray, 100, 255, THRESH_BINARY);

			//threshold(dlg_ir->MaskMog2, dlg_ir->MaskMog2, 50, 255, THRESH_BINARY);
			//erode(dlg_ir->MaskMog2, dlg_ir->MaskMog2, cv::Mat()); cv::dilate(dlg_ir->MaskMog2, dlg_ir->MaskMog2, cv::Mat());

			findContours(dlg_ir->gray, dlg_ir->contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);// 윤각선 찾기
		//	drawContours(dlg_ir->MaskMog2, dlg_ir->contours, -1, cv::Scalar(0, 0, 255)); // 윤각선을 원본에 그리기

			vector<vector<Point>>contours_poly(dlg_ir->contours.size()); // bound box 계산 벡터 생성
			vector<Rect>boundRect(dlg_ir->contours.size()); // boundbox 좌표 저장 공간

			// 검출된 contour 확인
			// 크기로 확인
			for (int i = 0; i < dlg_ir->contours.size(); i++)
			{
				approxPolyDP(cv::Mat(dlg_ir->contours[i]), contours_poly[i], 1, true);

				// 만약 contour size 가 100보다 크면
				if (fabs(contourArea(Mat(contours_poly[i]))) > 10)
				{
					boundRect[i] = boundingRect(cv::Mat(contours_poly[i]));
					count++;
				}
			}

			if (count > 0)
			{
				// box draw
				// 검출된 갯수만큼 반복
				for (int i = 0; i < count; i++)
				{
					// 가로 size가 10일 때
					//if (boundRect[i].width > 10)
				//	{
					dlg_ir->dlg_viewer_eo->dlg_alarm->display_event(true);
					rectangle(dlg_ir->IR_Image, boundRect[i], cv::Scalar(0, 0, 255), 1);

					// 검출 되면 true
					// 처음 시작할 때는 event_count = 0;
					// 이전에 검출된 상황이면 event_count = 1;
					dlg_ir->Catch_event(true, dlg_ir->event_count);
					//	}
				}
			}

			// 하나도 검출이 된 게 없다.
			else if (count == 0)
				dlg_ir->dlg_viewer_eo->dlg_alarm->display_event(false);

			// 검출이 안된 경우 (contours.size = 0)
			// 이전 이벤트에서의 경과 시간을 확인 (event_count = 1)
			if (dlg_ir->contours.size() == 0 && dlg_ir->event_count == 1)
				dlg_ir->Catch_event(false, dlg_ir->event_count);

			StretchDIBits(dlg_ir->IR_hdc, dlg_ir->MonitorRect[0].left, dlg_ir->MonitorRect[0].top, dlg_ir->MonitorRect[0].right, dlg_ir->MonitorRect[0].bottom - 100, 0, 0, dlg_ir->IR_Image.cols, dlg_ir->IR_Image.rows,
				dlg_ir->IR_Image.data, dlg_ir->m_pBitmapInfo_ir, DIB_RGB_COLORS, SRCCOPY);

			CDC* pDC_rectangle;
			pDC_rectangle = dlg_ir->dlg_viewer_ir->m_pic_ir.GetDC();
			pDC_rectangle->SelectObject(dlg_ir->pen_roi);
			pDC_rectangle->SelectStockObject(NULL_BRUSH);

			if (dlg_ir->dlg_viewer_eo->dlg_alarm->m_ico_line.GetIcon() == dlg_ir->dlg_viewer_eo->dlg_alarm->m_icon_line_true)
			{
				pDC_rectangle->Rectangle(dlg_ir->eo_first_point.x, dlg_ir->eo_first_point.y,
					dlg_ir->eo_second_point.x, dlg_ir->eo_second_point.y);
			}
		}
	}
	return 0;
}

UINT CnvsviewerDlg::EO_EVENT_Thread(LPVOID pParam)
{
	CnvsviewerDlg* dlg_eo = (CnvsviewerDlg*)pParam;

	while (1)
	{
		if (dlg_eo->chk_eo == true)
		{
			// IR 검출 유무에 따라
			// 저장
			if (dlg_eo->dlg_viewer_ir->SAVE_IR == true)
				dlg_eo->EO_VideoWrite << dlg_eo->EO_Image;

		}
	}

	return 0;
}

void CnvsviewerDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	str_zoom_value.Format(_T("%d"), zoom_value);
	AfxGetApp()->WriteProfileStringW(_T("CAMERA"), _T("ZOOM_POSITION"), str_zoom_value);

	delete dlg_viewer_eo;
	delete dlg_viewer_ir;

	EO_VideoCap->release();
	IR_VideoCap->release();

	EO->SuspendThread();
	IR->SuspendThread();
	IR_EVENT->SuspendThread();
	EO_EVENT->SuspendThread();

	DWORD dwResult;
	::GetExitCodeThread(EO->m_hThread, &dwResult);
	::GetExitCodeThread(IR->m_hThread, &dwResult);
	::GetExitCodeThread(IR_EVENT->m_hThread, &dwResult);
	::GetExitCodeThread(EO_EVENT->m_hThread, &dwResult);

	delete EO;
	delete IR;
	delete IR_EVENT;
	delete EO_EVENT;

	EO = NULL;
	IR = NULL;
	IR_EVENT = NULL;
	EO_EVENT = NULL;

	CDialogEx::OnClose();
}

void CnvsviewerDlg::Catch_event(BOOL event, int count)
{
	// 처음 이벤트가 발생하였을 때
	// 검출 시간 확인, 폴더 생성, count 증가

	// 이벤트가 검출되었다는
	if (event == true)
	{
		// count = 0 이면 처음 검출되었다는 의미
		if (count == 0)
		{
			// 처음 발생했을 때 시간 확인
			event_time = CTime::GetCurrentTime();

			// 경로 확인
			str_path = save_folder;
			year.Format(_T("\\%4d년도"), event_time.GetYear());
			month.Format(_T("\\%2d월"), event_time.GetMonth());
			day.Format(_T("\\%2d일"), event_time.GetDay());

			// 년도별 폴더 생성
			str_path = str_path + year;
			CreateDirectory(str_path, NULL);

			// 월별 폴더 생성
			str_path = str_path + month;
			CreateDirectory(str_path, NULL);

			// 일자별 폴더 생성
			str_path = str_path + day;
			CreateDirectory(str_path, NULL);

			// 시간별 폴더 생성
			//name.Format(_T("\\%02d%02d%02d"), event_time.GetHour());
			name.Format(_T("\\%02d%02d%02d"), event_time.GetHour(), event_time.GetMinute(), event_time.GetSecond());
			str_path = str_path + name;
			CreateDirectory(str_path, NULL);

			// 시간별 파일 생성
			eo_file_name = str_path + _T("\\[EO].avi");
			ir_file_name = str_path + _T("\\[IR].avi");

			EO_VideoName = std::string(CT2CA(eo_file_name));
			IR_VideoName = std::string(CT2CA(ir_file_name));

			IR_VideoWrite.open(IR_VideoName, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
				IR_FPS, cv::Size(IR_VideoWidth, IR_VideoHeight), true);

			EO_VideoWrite.open(EO_VideoName, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'),
				IR_FPS, cv::Size(EO_VideoWidth, EO_VideoHeight), true);

			dlg_viewer_ir->SAVE_IR = true;

			event_count = 1;
		}

		else if (count == 1)
		{
			event_time = CTime::GetCurrentTime();
		}
	}

	else if (event == false)
	{
		real_time = CTime::GetCurrentTime();
		between_time = real_time - event_time;

		// 시간을 확인
		// 이벤트 검출 유무를 확인해야 함.
		if (between_time < 5)
		{

		}

		else if (between_time > 5)
		{
			event_count = 0;
			dlg_viewer_ir->SAVE_IR = false;
		}
	}
}

LRESULT CnvsviewerDlg::OnReceive(WPARAM wParam, LPARAM IParam)
{
	BYTE buffer;
	CString message;
	int length = -1;

	length = this->m_comPort.m_QueueRead.GetSize();

	Protocol.syncbyte = 0xFF;	// 고정
	Protocol.address = 0x02;	// 고정
	Protocol.cmd1 = 0x01;		// 고정

	Protocol.cmd2 = 0x00;
	Protocol.length = 0x00;
	Protocol.data = 0x00;
	Protocol.checksum = 0x00;

	// Zoom Control
	if (length == 6)
	{
		for (int i = 0; i < length; i++)
		{
			this->m_comPort.m_QueueRead.GetByte(&buffer);

			if (i == 4)
				Protocol.length = buffer;

			else if (i == 5)
				Protocol.data = buffer;
		}
	}

	// focus, day&night, lv filter
	else if (length == 5)
	{
		for (int i = 0; i < length; i++)
		{
			this->m_comPort.m_QueueRead.GetByte(&buffer);

			if (i == 3)
				Protocol.cmd1 = buffer;

		}
	}

	ReceiveData();

	return 0;
}

void CnvsviewerDlg::ReceiveData()
{
	if (Protocol.syncbyte == 0xFF && Protocol.address == 0x02 && Protocol.cmd1 == 0x01 && Protocol.cmd2 == 0x04 && Protocol.length == 0x01)
	{
		if (Protocol.length == 0x00)
			ZoomControl(0);

		else if (Protocol.length == 0x01)
			ZoomControl(1);

		else if (Protocol.length == 0x02)
			ZoomControl(2);

		else if (Protocol.length == 0x03)
			ZoomControl(3);

		else if (Protocol.length == 0x04)
			ZoomControl(4);

		else if (Protocol.length == 0x05)
			ZoomControl(5);

		else if (Protocol.length == 0x06)
			ZoomControl(6);

		else if (Protocol.length == 0x07)
			ZoomControl(7);

		else if (Protocol.length == 0x08)
			ZoomControl(8);

		else if (Protocol.length == 0x09)
			ZoomControl(9);

		else if (Protocol.length == 0x0A)
			ZoomControl(10);
	}
}

int CnvsviewerDlg::ZoomControl(int nValue)
{
	if (nValue)
	{
		eo_first_point.x = 100;
		eo_first_point.y = 100;

		eo_second_point.x = 200;
		eo_second_point.y = 200;
	}

	return zoom_value = nValue;
}

void CnvsviewerDlg::setBGImg()
{
	//imwrite("bg.bmp", IR_Image);
}