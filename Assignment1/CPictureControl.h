#pragma once
#include "afxdialogex.h"
#include <iostream>
#include <vector>
#include <random>

#define MAX_CIRCLE_EXIST 3		// 최대 생성 가능한 원의 개수
#define INIT_BPP 8
#define PADDING_PICTURE 2

using namespace std;

// CStatic을 상속받도록 변경
class CPictureControl : public CStatic
{
	// CStatic에 맞게 매크로 변경
	DECLARE_DYNAMIC(CPictureControl)

public:
	// 생성자 변경: CStatic은 매개변수 없는 기본 생성자를 사용합니다.
	CPictureControl();
	virtual ~CPictureControl();

	void initImage(int nRadius, int nStroke, bool flag_init = false);
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

	bool flag_move;			// 작은 원 클릭 시 움직임 활성화 플래그
	int m_nNumIdx;			// 클릭된 원의 인덱스 확인

	// 메서드
	void clearImage(bool flag_init = false);
	bool GetPointInCircle(CPoint cursor, CPoint point);		// 클릭한 좌표가 생성된 원 내부에 존재하는지 확인
	bool GetPointNearCircle(CPoint cursor, CPoint point);		// 클릭한 좌표가 생성된 원 주변에 존재하는지 확인

	void drawCircle(CPoint point, int nRadius = 0);
	void draw3PointsCircle();

public:
	// OnCreate 메시지 핸들러 추가
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
