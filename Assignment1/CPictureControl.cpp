// CPictureControl.cpp: 구현 파일

#include "pch.h"
#include "Assignment1.h"
#include "CPictureControl.h"


// CPictureControl
// CDialogEx 대신 CStatic을 상속받습니다.
IMPLEMENT_DYNAMIC(CPictureControl, CStatic)

CPictureControl::CPictureControl()
{
	this->flag_move = false;
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
	CPaintDC dc(this); // device context for painting

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


bool CPictureControl::GetPointInCircle(CPoint cursor, CPoint point) {
	// 입력한 점을 기준으로 반경 내부에 커서가 존재할 시 true 반환
	double dx = cursor.x - point.x;
	double dy = cursor.y - point.y;
	double dist = sqrt((dx * dx) + (dy * dy));

	if (dist <= this->m_nRadius) return true;
	else return false;
}

bool CPictureControl::GetPointNearCircle(CPoint cursor, CPoint point) {
	// 입력한 점을 기준으로 생성된 원의 반경범위에 존재할 시 true 반환
	double dx = cursor.x - point.x;
	double dy = cursor.y - point.y;
	double dist = sqrt((dx * dx) + (dy * dy));

	if (this->m_nRadius < dist && dist <= this->m_nRadius * 2) return true;
	else return false;
}

void CPictureControl::initImage(int nRadius, int nStroke, bool flag_init) {
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
	RGBQUAD pPalette[256];
	pPalette[0] = { 0, 0, 0, 0 };
	pPalette[255] = { 255, 255, 255, 0 }; // 흰색
	this->m_image.SetColorTable(0, 256, pPalette);

	// 회색 톤 팔레트 생성 (안티 앨리어싱용)
	for (int i = 1; i < 255; i++) {
		pPalette[i] = { (BYTE)i, (BYTE)i, (BYTE)i, 0 };
	}

	// 픽셀 초기화
	this->clearImage(flag_init);

	this->Invalidate();
}

void CPictureControl::clearImage(bool flag_init) {
	// 점 목록 초기화
	if(flag_init) this->m_vLocPoint.clear();

	// m_image를 다시 흰색으로 초기화
	unsigned char* fm = (unsigned char*)this->m_image.GetBits();
	int nPitch = this->m_image.GetPitch();
	memset(fm, 0xff, nPitch * this->m_image.GetHeight());
}

void CPictureControl::DrawAll() {
	this->clearImage();

	// 원 그리기
	for (CPoint point : this->m_vLocPoint) {
		this->drawCircle(point);
	}

	if (this->m_vLocPoint.size() == MAX_CIRCLE_EXIST) {
		draw3PointsCircle();
	}

	this->Invalidate();
}

bool CPictureControl::DrawRandom() {
	// 3점이 존재할 때만 사용, 실행되지 않으면 false로 반환하기
	if (this->m_vLocPoint.size() >= MAX_CIRCLE_EXIST) {
		//this->m_vLocPoint.clear();		// 3점벡터 초기화
		this->initImage(this->m_nRadius, this->m_nStroke, true);

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

		this->DrawAll();
		return true;
	}
	return false;
}

void CPictureControl::drawCircle(CPoint point, int nRadius) {
	// 정원을 그릴지, 작은 원을 그릴지 결정
	bool flag_center = false;
	double offset = 0;
	if (nRadius == 0) {
		// 작은 원을 그리기 위한 전역 반지름 입력
		nRadius = this->m_nRadius;
		offset = 0.5;
	}
	else {
		// 정원 그리는 플래그 활성화
		flag_center = true;
	}

	const int nCenterX = point.x;
	const int nCenterY = point.y;
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
		// 다른 주소 참조를 막기 위한 예외처리
		if (j >= nHeight || j < 0) continue;
		for (int i = nStartX; i <= nEndX; i++) {
			// 다른 주소 참조를 막기 위한 예외처리
			if (i >= nWidth || i < 0) continue;

			// 안티앨리어싱을 위하여 픽셀 중앙으로 오프셋
			double dx = nCenterX - i - offset;
			double dy = nCenterY - j - offset;
			double distSq = (dx * dx) + (dy * dy);
			double dist = sqrt(distSq);

			// 플래그로 구분, 솔리드, 엣지로만 생성
			if (flag_center) {
				// 엣지 생성
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
void CPictureControl::draw3PointsCircle() {
	// 중심점 변수 선언
	const vector<CPoint> points = this->m_vLocPoint;
	CPoint pntCenter;

	// A - B
	double a1 = points[1].x - points[0].x;
	double b1 = points[1].y - points[0].y;
	double c1 = ((pow(points[0].x, 2) - pow(points[1].x, 2)) + (pow(points[0].y, 2) - pow(points[1].y, 2))) / 2.0;

	// B - C
	double a2 = points[2].x - points[1].x;
	double b2 = points[2].y - points[1].y;
	double c2 = ((pow(points[1].x, 2) - pow(points[2].x, 2)) + (pow(points[1].y, 2) - pow(points[2].y, 2))) / 2.0;

	// 해가 존재하는지 확인, double의 불확실성에 의한 오차범위 고려(0.000001)
	double det = a1 * b2 - a2 * b1;
	if (abs(det) < 1e-6) {
		cout << "3점이 한 선에 위치합니다. 정원을 그릴 수 없습니다." << endl;
		return;
	}

	// 중심점 구하기
	pntCenter.x = -(c1 * b2 - c2 * b1) / det;
	pntCenter.y = -(a1 * c2 - a2 * c1) / det;

	// 반지름 구하기 
	double r1 = sqrt(pow(points[0].x - pntCenter.x, 2) + pow(points[0].y - pntCenter.y, 2));
	double r2 = sqrt(pow(points[1].x - pntCenter.x, 2) + pow(points[1].y - pntCenter.y, 2));
	double r3 = sqrt(pow(points[2].x - pntCenter.x, 2) + pow(points[2].y - pntCenter.y, 2));
	int nRadius = static_cast<int>(round((r1 + r2 + r3) / 3.0)); // 평균값 반올림하여 정수형으로 변환

	this->drawCircle(pntCenter, nRadius);
}

void CPictureControl::OnMouseMove(UINT nFlags, CPoint point)
{
	// 이동 가능한 상태라면 해당 점을 이동
	if (this->flag_move) {
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
		bool flag_enable = true;
		for (int i = 0; i < this->m_vLocPoint.size(); i++) {
			//현재 생성된 작은 원과의 겹치지 않게 범위 확인 및 패스
			if (this->GetPointNearCircle(pos, m_vLocPoint[i])) {
				flag_enable = false;
				break;
			}
			//만약 현재 클릭한 커서가 작은 원 안에 있는 경우 이동 플래그 활성화
			if (this->GetPointInCircle(pos, m_vLocPoint[i])) {
				flag_enable = false;
				this->flag_move = true;
				this->m_nNumIdx = i;
				break;
			}
		}

		//cout << "클릭된 좌표 : " << pos.x << ", " << pos.y << endl;

		// 벡터에 현재 포인트 입력
		if (flag_enable && this->m_vLocPoint.size() < MAX_CIRCLE_EXIST) {
			this->m_vLocPoint.push_back(pos);
			this->DrawAll();
		}
	}
	CStatic::OnLButtonDown(nFlags, point);
}

void CPictureControl::OnLButtonUp(UINT nFlags, CPoint point)
{
	this->flag_move = false;

	CStatic::OnLButtonUp(nFlags, point);
}
