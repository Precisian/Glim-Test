
// Assignment1Dlg.h: 헤더 파일
//

#pragma once

#define INIT_RADIUS 10
#define INIT_STROKE 2
#define RANDOM_MAX 10
#define RANDOM_INTERVAL 500

// CAssignment1Dlg 대화 상자
class CAssignment1Dlg : public CDialogEx
{
// 생성입니다.
public:
	CAssignment1Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	// 스레드 상태 열거형 선언
	enum ThreadWorkingType
	{
		THREAD_STOP = 0,
		THREAD_RUNNING = 1,
		THREAD_PAUSE = 2
	};


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ASSIGNMENT1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

private:
	// 설정값
	int m_nRadius;						// 클릭된 원의 반경
	int m_nStroke;						// 생성된 정원의 테두리

	// 창 최소 크기
	int m_nDialogMinSize_Width;
	int m_nDialogMinSize_Height;

	// 픽쳐컨트롤 인스턴스
	CPictureControl cPicturControl;

	// 스레드
	CWinThread* m_pThread;
	ThreadWorkingType m_pThreadWork;



protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBN_INIT();
	afx_msg void OnBnClickedBnModify();

	afx_msg void OnExitSizeMove();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedBnRandom();

	static UINT ThreadRepeat(LPVOID LpData);
};
