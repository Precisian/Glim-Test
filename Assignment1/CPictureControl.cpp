// CPictureControl.cpp: 구현 파일

#include "pch.h"
#include "Assignment1.h"
#include "CPictureControl.h"


IMPLEMENT_DYNAMIC(CPictureControl, CStatic)

CPictureControl::CPictureControl()
{
	this->m_bFlagMove = false;
	this->m_nStroke = 0;
	this->m_nRadius = 0;
	this->m_nNumIdx = 0;
}

CPictureControl::~CPictureControl()
{
	this->m_image.Destroy();
}


BEGIN_MESSAGE_MAP(CPictureControl, CStatic)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CPictureControl 메시지 처리기
void CPictureControl::OnPaint()
{
	CPaintDC dc(this); 

	// 이미지를 화면에 그리기
	CRect rect;
	GetClientRect(&rect);

	// 실행하려는 영역 설정
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&rect);
	dc.SelectClipRgn(&rgn);

	if (this->m_image) {
		this->m_image.Draw(dc, rect);
	}
}

// CStatic 컨트롤이 생성될 때 호출되는 메시지 핸들러입니다.
int CPictureControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


bool CPictureControl::GetPointInCircle(CPoint cPointCursor, CPoint cPointTarget) {
	// 입력한 점을 기준으로 반경 내부에 커서가 존재할 시 true 반환
	double dx = cPointCursor.x - cPointTarget.x;
	double dy = cPointCursor.y - cPointTarget.y;
	double dist = sqrt((dx * dx) + (dy * dy));

	if (dist <= this->m_nRadius) return true;
	else return false;
}

bool CPictureControl::GetPointNearCircle(CPoint cPointCursor, CPoint cPointTarget) {
	// 입력한 점을 기준으로 생성된 원의 반경범위에 존재할 시 true 반환
	double dx = cPointCursor.x - cPointTarget.x;
	double dy = cPointCursor.y - cPointTarget.y;
	double dist = sqrt((dx * dx) + (dy * dy));

	if (this->m_nRadius < dist && dist <= this->m_nRadius * 2) return true;
	else return false;
}

void CPictureControl::InitImage(int nRadius, int nStroke, bool bFlagInit) {
	this->m_nRadius = nRadius;
	this->m_nStroke = nStroke;

	// CImage가 존재 시 파괴
	if (this->m_image) this->m_image.Destroy();

	// 픽쳐컨트롤의 영역 확보
	CRect rect;
	this->GetWindowRect(&rect);
	GetParent()->ScreenToClient(&rect);

	// 컨트롤 크기 저장
	this->m_nControlSize_Width = rect.Width();
	this->m_nControlSize_Height = rect.Height();

	// CImage 객체 생성
	this->m_image.Create(rect.Width(), -rect.Height(), INIT_BPP);

	// 컬러 팔레트 설정
	RGBQUAD pPalette[256] = { 0 };						// 컬러팔레트 생성, 검은색으로 초기화
	pPalette[255] = { 255, 255, 255, 0 };				// 흰색
	this->m_image.SetColorTable(0, 256, pPalette);

	// 팔레트의 중간 값 설정(안티 앨리어싱용)
	for (int i = 1; i < 255; i++) {
		pPalette[i] = { (BYTE)i, (BYTE)i, (BYTE)i, 0 };
	}

	// 픽셀 초기화
	this->ClearImage(bFlagInit);

	this->Invalidate();
}

void CPictureControl::ClearImage(bool bFlagInit) {
	// 점 목록 초기화
	if(bFlagInit) this->m_vLocPoint.clear();

	// m_image를 흰색으로 초기화
	unsigned char* fm = (unsigned char*)this->m_image.GetBits();
	int nPitch = this->m_image.GetPitch();
	memset(fm, 0xff, nPitch * this->m_image.GetHeight());
}

void CPictureControl::DrawAll() {
	this->ClearImage();

	// 원 그리기
	for (CPoint point : this->m_vLocPoint) {
		this->DrawCircle(point);
	}

	if (this->m_vLocPoint.size() == MAX_CIRCLE_EXIST) {
		Draw3PointsCircle();
	}

	this->Invalidate();
}

bool CPictureControl::DrawRandom() {
	// 3점이 존재할 때만 사용, 실행되지 않으면 false로 반환하기
	if (this->m_vLocPoint.size() >= MAX_CIRCLE_EXIST) {
		// 현재 화면에 출력중인 3점 벡터 초기화
		this->m_vLocPoint.clear();

		random_device rd;
		mt19937 gen(rd());

		// 랜덤 좌표 생성
		for (int i = 0; i < MAX_CIRCLE_EXIST; i++) {
			CPoint point;

			// 난수 생성
			uniform_int_distribution<int> dist_x(0, this->m_nControlSize_Width);
			uniform_int_distribution<int> dist_y(0, this->m_nControlSize_Height);

			// 난수를 좌표로 입력
			point.x = dist_x(gen);
			point.y = dist_y(gen);

			// 벡터에 넣기
			this->m_vLocPoint.push_back(point);
		}

		// 랜덤하게 생성된 3점 좌표로 전체 그리기(3개의 작은 원, 정원)
		this->DrawAll();
		return true;
	}
	return false;
}

void CPictureControl::DrawCircle(CPoint cPointCursor, int nRadius) {
	// 정원을 그릴지, 작은 원을 그릴지 결정
	bool bFlagCenter = false;
	double offset = 0;
	if (nRadius == 0) {
		// 작은 원을 그리기 위한 전역 반지름 입력
		nRadius = this->m_nRadius;
		offset = 0.5;
	}
	else {
		// 정원 그리는 플래그 활성화
		bFlagCenter = true;
	}

	const int nCenterX = cPointCursor.x;
	const int nCenterY = cPointCursor.y;
	const int nRadiusSq = pow(nRadius, 2);

	// 이미지 너비, 높이 확보
	const int nWidth = this->m_image.GetWidth();
	const int nHeight = this->m_image.GetHeight();

	unsigned char* fm = (unsigned char*)this->m_image.GetBits();
	int nPitch = this->m_image.GetPitch();

	// 시작, 종료 시점 생성
	const int nStartX = nCenterX - nRadius - (this->m_nStroke / 2);
	const int nStartY = nCenterY - nRadius - (this->m_nStroke / 2);
	const int nEndX = nCenterX + nRadius + (this->m_nStroke / 2);
	const int nEndY = nCenterY + nRadius + (this->m_nStroke / 2);

	//  원형을 그리기 위한 반복문 순환
	for (int j = nStartY; j <= nEndY; j++) {
		// 이미지 객체 범위 이내로만 작동되게 함
		if (j >= nHeight || j < 0) continue;
		for (int i = nStartX; i <= nEndX; i++) {
			// 이미지 객체 범위 이내로만 작동되게 함
			if (i >= nWidth || i < 0) continue;

			// 안티앨리어싱을 위하여 픽셀 중앙으로 오프셋
			double dx = nCenterX - i - offset;
			double dy = nCenterY - j - offset;
			double distSq = (dx * dx) + (dy * dy);
			double dist = sqrt(distSq);

			// 플래그로 구분, 솔리드, 엣지로만 생성
			if (bFlagCenter) {
				// nStroke의 절반을 안쪽, 나머지 절반을 바깥쪽에 할당
				const int innerRadius = static_cast<int>(nRadius - this->m_nStroke * 0.5);
				const int outerRadius = static_cast<int>(nRadius + this->m_nStroke * 0.5);

				// 안쪽과 바깥쪽 사이의 거리일 때만 검정색
				if (dist >= innerRadius && dist <= outerRadius) {
					fm[j * nPitch + i] = 0;
				}
			}
			else {
				// 솔리드 생성
				if (dist <= nRadius) {
					fm[j * nPitch + i] = 0;
				}
			}
		}
	}
}

// 3점에 대해 존재하는 원을 작성
void CPictureControl::Draw3PointsCircle() {
	// 중심점 변수 선언
	const vector<CPoint> vPoints = this->m_vLocPoint;
	CPoint cPointCenter;

	// 임의의 2점에 대하여 원에 대한 연립방정식 원소 대체 : points[0] - points[1]
	double a1 = vPoints[1].x - vPoints[0].x;
	double b1 = vPoints[1].y - vPoints[0].y;
	double c1 = ((pow(vPoints[0].x, 2) - pow(vPoints[1].x, 2)) + (pow(vPoints[0].y, 2) - pow(vPoints[1].y, 2))) / 2.0;

	// 임의의 2점에 대하여 원에 대한 연립방정식 원소 대체 : points[1] - points[2]
	double a2 = vPoints[2].x - vPoints[1].x;
	double b2 = vPoints[2].y - vPoints[1].y;
	double c2 = ((pow(vPoints[1].x, 2) - pow(vPoints[2].x, 2)) + (pow(vPoints[1].y, 2) - pow(vPoints[2].y, 2))) / 2.0;

	// 해가 존재하는지 확인, double의 불확실성에 의한 오차범위 고려(0.000001)
	double det = a1 * b2 - a2 * b1;
	if (abs(det) < 1e-6) {
		cout << "3점이 한 선에 위치합니다. 정원을 그릴 수 없습니다." << endl;
		return;
	}

	// 중심점 구하기
	cPointCenter.x = -(c1 * b2 - c2 * b1) / det;
	cPointCenter.y = -(a1 * c2 - a2 * c1) / det;

	// 반지름 구하기 
	double r1 = sqrt(pow(vPoints[0].x - cPointCenter.x, 2) + pow(vPoints[0].y - cPointCenter.y, 2));
	double r2 = sqrt(pow(vPoints[1].x - cPointCenter.x, 2) + pow(vPoints[1].y - cPointCenter.y, 2));
	double r3 = sqrt(pow(vPoints[2].x - cPointCenter.x, 2) + pow(vPoints[2].y - cPointCenter.y, 2));
	int nRadius = static_cast<int>(round((r1 + r2 + r3) / 3.0)); // 평균값 반올림하여 정수형으로 변환

	this->DrawCircle(cPointCenter, nRadius);
}

void CPictureControl::OnMouseMove(UINT nFlags, CPoint point)
{
	// 이동 가능한 상태라면 해당 점을 이동
	if (this->m_bFlagMove) {
		GetCursorPos(&point);
		ScreenToClient(&point);
		// 클릭된 원의 인덱스 위치의 좌표 변경
		this->m_vLocPoint[this->m_nNumIdx] = point;

		this->DrawAll();
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CPictureControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// 픽쳐컨트롤의 영역 확보
	CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(&rect);

	// 현재 클릭한 좌표를 픽쳐컨트롤 기준으로 확보
	CPoint pos;
	GetCursorPos(&pos);
	ScreenToClient(&pos);

	// 픽쳐컨트롤 영역 내에 마우스 좌표가 존재 시
	if (rect.PtInRect(pos)) {
		bool bFlagEnable = true;
		for (int i = 0; i < this->m_vLocPoint.size(); i++) {
			//현재 생성된 작은 원과의 겹치지 않게 범위 확인 및 패스
			if (this->GetPointNearCircle(pos, m_vLocPoint[i])) {
				bFlagEnable = false;
				break;
			}
			//만약 현재 클릭한 커서가 작은 원 안에 있는 경우 이동 플래그 활성화
			if (this->GetPointInCircle(pos, m_vLocPoint[i])) {
				bFlagEnable = false;
				this->m_bFlagMove = true;
				this->m_nNumIdx = i;
				break;
			}
		}

		// 벡터에 현재 포인트 입력
		if (bFlagEnable && this->m_vLocPoint.size() < MAX_CIRCLE_EXIST) {
			this->m_vLocPoint.push_back(pos);
			this->DrawAll();
		}
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void CPictureControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	this->m_bFlagMove = false;

	CStatic::OnLButtonUp(nFlags, point);
}
