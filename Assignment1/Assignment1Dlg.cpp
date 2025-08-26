
// Assignment1Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Assignment1.h"
#include "Assignment1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    

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


// CAssignment1Dlg 대화 상자

CAssignment1Dlg::CAssignment1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ASSIGNMENT1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAssignment1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// DDX 바인딩 코드 추가
	DDX_Text(pDX, IDC_EDIT_RADIUS, this->m_nRadius);
	DDX_Text(pDX, IDC_EDIT_STROKE, this->m_nStroke);
	DDX_Control(pDX, IDC_STATIC_PICTURE, this->m_cPicturControl);
}

BEGIN_MESSAGE_MAP(CAssignment1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BN_INIT, &CAssignment1Dlg::OnBnClickedBN_INIT)
	ON_BN_CLICKED(IDC_BN_MODIFY, &CAssignment1Dlg::OnBnClickedBnModify)
	ON_WM_GETMINMAXINFO()
	ON_WM_EXITSIZEMOVE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BN_RANDOM, &CAssignment1Dlg::OnBnClickedBnRandom)
END_MESSAGE_MAP()


// CAssignment1Dlg 메시지 처리기

BOOL CAssignment1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	// 컨트롤 생성 완료 후 실행
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// 초기 설정값 입력
	this->m_nRadius = INIT_RADIUS;
	this->m_nStroke = INIT_STROKE;

	// 현재 생성된 창 크기를 최소크기로 지정
	CRect rect;
	this->GetWindowRect(&rect);
	this->m_nDialogMinSize_Width = rect.Width();
	this->m_nDialogMinSize_Height = rect.Height();

	// 픽쳐컨트롤 인스턴스 생성
	this->m_cPicturControl.InitImage(this->m_nRadius, this->m_nStroke);

	// 초기값을 컨트롤에 적용
	UpdateData(false);	

	return TRUE;  
}

void CAssignment1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAssignment1Dlg::OnPaint()
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
// 이 함수를 호출합니다.
HCURSOR CAssignment1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAssignment1Dlg::OnBnClickedBN_INIT()
{
	// 이미지 초기화
	this->m_cPicturControl.InitImage(this->m_nRadius, this->m_nStroke, true);
}

void CAssignment1Dlg::OnBnClickedBnModify()
{
	// 반경 크기, 테두리 굵기에 대하여 Image 재생성
	// UI의 컨트롤에서 값을 수신
	UpdateData();
	this->m_cPicturControl.InitImage(this->m_nRadius, this->m_nStroke);
	this->m_cPicturControl.DrawAll();
}

void CAssignment1Dlg::OnExitSizeMove()
{
	// 이미지 재생성
	this->m_cPicturControl.InitImage(this->m_nRadius, this->m_nStroke);
	this->m_cPicturControl.DrawAll();

	CDialogEx::OnExitSizeMove();
}

void CAssignment1Dlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// 크기 변경시 작동, 최소 크기 제한
	lpMMI->ptMinTrackSize.x = this->m_nDialogMinSize_Width;
	lpMMI->ptMinTrackSize.y = this->m_nDialogMinSize_Height;

	// 창의 크기가 변화하는 동안 실시간으로 반영
	if (this->m_cPicturControl && this->m_iThreadWork == THREAD_STOP) {
		this->m_cPicturControl.InitImage(this->m_nRadius, this->m_nStroke);
		this->m_cPicturControl.DrawAll();
	}	
	
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

void CAssignment1Dlg::OnBnClickedBnRandom()
{
	// 반복 메서드가 동작하지 않는 경우에만 활성화
	if (this->m_iThreadWork == THREAD_STOP) {
		// 별도 스레드에서 구동, 랜덤하게 UI 변경됨
		this->m_pThread = AfxBeginThread(this->ThreadRepeat, this);
		this->m_iThreadWork = THREAD_RUNNING;		// 스레드의 동작 상태 갱신
	}
}

UINT CAssignment1Dlg::ThreadRepeat(LPVOID LpData)
{
	CAssignment1Dlg* pDlg = (CAssignment1Dlg*)(LpData);

	// 계수용 변수 선언
	int cnt = 0;
	// 랜덤 횟수만큼만 반복
	while (cnt < RANDOM_MAX)
	{
		// 랜덤으로 구동한게 수행했다면, cnt 증가와 딜레이 실행
		if (pDlg->m_cPicturControl.DrawRandom()) {
			cnt++;
		}
		// 실행이 진행되지 않으면 종료
		else break;
		Sleep(RANDOM_INTERVAL);
	}

	// 스레드의 상태 갱신
	pDlg->m_iThreadWork = THREAD_STOP;
	return 0;
}
