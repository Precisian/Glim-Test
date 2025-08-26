#pragma once
#include "afxdialogex.h"
#include <iostream>
#include <vector>
#include <random>

#define MAX_CIRCLE_EXIST 3		// 최대 생성 가능한 원의 개수
#define INIT_BPP 8
#define PADDING_PICTURE 2

using namespace std;

class CPictureControl : public CStatic
{
	DECLARE_DYNAMIC(CPictureControl)

public:
	CPictureControl();
	virtual ~CPictureControl();

	void InitImage(int nRadius, int nStroke, bool bFlagInit = false);
	void DrawAll();
	bool DrawRandom();

protected:
	DECLARE_MESSAGE_MAP()

private:
	// 이미지 그리기용
	CImage m_image;

	vector<CPoint> m_vLocPoint;		// 마우스로 클릭된 원의 좌표
	int m_nRadius;						// 클릭된 원의 반경
	int m_nStroke;						// 생성된 정원의 테두리

	// 컨트롤 크기
	int m_nControlSize_Width;
	int m_nControlSize_Height;

	bool m_bFlagMove;			// 작은 원 클릭 시 움직임 활성화 플래그
	int m_nNumIdx;			// 클릭된 원의 인덱스 확인

	// 메서드
	void ClearImage(bool bFlagInit = false);
	bool GetPointInCircle(CPoint cPointCursor, CPoint cPointTarget);		// 클릭한 좌표가 생성된 원 내부에 존재하는지 확인
	bool GetPointNearCircle(CPoint cPointCursor, CPoint cPointTarget);		// 클릭한 좌표가 생성된 원 주변에 존재하는지 확인

	void DrawCircle(CPoint cPointCursor, int nRadius = 0);					// 입력된 좌표를 중점으로 원 생성, 반경 값에 따라 솔리드인지, 엣지인지 구분하여 생성
	void Draw3PointsCircle();												// 3점에 대해서 중심 좌표 획득

public:
	// OnCreate 메시지 핸들러 추가
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
