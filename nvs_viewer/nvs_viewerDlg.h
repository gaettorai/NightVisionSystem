
// nvs_viewerDlg.h: 헤더 파일
//

#pragma once

#include "stdafx.h"
#include "viewer_eo.h"
#include "viewer_ir.h"


#include "CommThread.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/video/background_segm.hpp"

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/features2d.hpp"
#include <iostream>
#include <string>


//#include "alarm_record.h"
//#include "alarm_event.h"

//#define OUTPUT_VIDEO_NAME "out.avi"

using namespace cv;
using namespace std;

typedef struct PROTOCOL1		// pelco & WIA
{
	unsigned char syncbyte;
	unsigned char address;
	unsigned char cmd1;
	unsigned char cmd2;
	unsigned char length;
	unsigned char data;
	unsigned char checksum;
	//	UCHAR SendData[7];
}PROTOCOL1;

// CnvsviewerDlg 대화 상자
class CnvsviewerDlg : public CDialogEx
{
// 생성입니다.
public:
	CnvsviewerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	viewer_eo* dlg_viewer_eo;
	viewer_ir* dlg_viewer_ir;

	CCommThread* m_comm;

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NVS_VIEWER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClose();
	afx_msg LRESULT OnReceive(WPARAM wParam, LPARAM IParam);

	void ConnectEO();
	void ConnectIR();
	void ConnectSerial();

	// monitor 갯수 확인
	int MonitorCount;
	CRect* MonitorRect;

	int eo_x, eo_y, eo_width, eo_height;	// x, y, width, height
	int ir_x, ir_y, ir_width, ir_height;

	TCHAR chFilePath[256] = { 0, };
	CString strFolderPath;

	CString volume_folder;
	CString save_folder;

	// 영상 출력
	string EO_VideoStream;
	string IR_VideoStream;

	VideoCapture *EO_VideoCap;
	VideoCapture *IR_VideoCap;

	// 영상 저장
	string EO_VideoSave;
	string IR_VideoSave;

	VideoWriter EO_VideoWrite;
	VideoWriter	IR_VideoWrite;

	// 영상 저장 관련 Parameter
	// FrameRate
	float EO_FPS;
	float IR_FPS;
	
	// EO Video Size
	int EO_VideoWidth;
	int EO_VideoHeight;

	// IR Video Size
	int IR_VideoWidth;
	int IR_VideoHeight;

	string EO_VideoName;
	string IR_VideoName;

	// Frame Image
	Mat EO_Image;
	Mat IR_Image;

	BITMAPINFO* m_pBitmapInfo_eo;
	BITMAPINFO* m_pBitmapInfo_ir;

	HDC EO_hdc;
	HDC IR_hdc;

	// 시리얼 포트 연결

	CCommThread m_comPort;

	int m_iSerialPort;
	int m_iBaudRate;
	int m_iDataBit;
	int m_iStopBit;
	int m_iParity;

	CString byIndexComPort(int xPort);// 포트이름 받기 
	DWORD byIndexBaud(int xBaud);// 전송률 받기
	BYTE byIndexData(int xData);//데이터 비트 받기
	BYTE byIndexStop(int xStop);// 스톱비트 받기 
	BYTE byIndexParity(int xParity);// 펠리티 받기

	static UINT IR_Thread(LPVOID pParam);
	CWinThread* IR = NULL;

	static UINT EO_Thread(LPVOID pParam);
	CWinThread* EO = NULL;

	static UINT IR_EVENT_Thread(LPVOID pParam);
	CWinThread* IR_EVENT = NULL;

	static UINT EO_EVENT_Thread(LPVOID pParam);
	CWinThread* EO_EVENT = NULL;

	Ptr<BackgroundSubtractorMOG2> pMog2;	// mog2 
	Mat MaskMog2;

	vector<vector<Point>>contours; // 윤각선 벡터 생성

	Mat back;
	Mat gray;
	Mat sub_frame;

	void Catch_event(BOOL event, int count);
	int event_count;

	CTime event_time;
	CTime real_time;
	CTimeSpan between_time;

	CString str_path = _T("");
	CString year = _T("");
	CString month = _T("");
	CString day = _T("");
	CString name = _T("");
	CString eo_file_name = _T("");
	CString ir_file_name = _T("");

	void ReceiveData();

	char syncbyte;
	char address;
	char cmd1;
	char cmd2;
	char data1;
	char data2;
	char checksum;

	//
	int ZoomControl(int nValue);	

	int zoom_value;
	CString str_zoom_value;

	CPen pen_roi;

	CPoint eo_first_point;
	CPoint eo_second_point;

	cv::Rect rect;

	BOOL chk_ir;
	BOOL chk_eo;

	void setBGImg();
};
