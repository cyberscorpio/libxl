#ifndef XL_UI_GDI_H
#define XL_UI_GDI_H
#include <assert.h>
#include <atlbase.h>
#include <atltypes.h>
#include <atlapp.h>
#include <atlgdi.h>
#include "../common.h"
#include "ResMgr.h"
#include "DIBSection.h"
XL_BEGIN
UI_BEGIN

template <bool t_bManaged>
class CDCPlusT
	: public WTL::CDCT<t_bManaged>
{
public:
	CDCPlusT (HDC hDC = NULL) : WTL::CDCT<t_bManaged>(hDC)
	{
	}

	BOOL drawLine (int x1, int y1, int x2, int y2) {
		POINT pos;
		if (MoveTo(x1, y1, &pos)) {
			BOOL ret = LineTo(x2, y2);
			MoveTo(pos);
			return ret;
		} else {
			return FALSE;
		}
	}

	BOOL drawLine (POINT p1, POINT p2) {
		return drawLine(p1.x, p1.y, p2.x, p2.y);
	}

	int drawTransparentText (LPCTSTR lpstrText, int cchText, LPRECT lpRect, UINT uFormat) {
		int oldMode = SetBkMode(TRANSPARENT);
		int ret = DrawText(lpstrText, cchText, lpRect, uFormat);
		SetBkMode(oldMode);
		return ret;
	}

	int drawTransparentTextWithDefaultFont (LPCTSTR lpstrText, int cchText, LPRECT lpRect, UINT uFormat) {
		CResMgr *pResMgr = CResMgr::getInstance();
		HFONT font = pResMgr->getSysFont();
		HFONT oldFont = SelectFont(font);
		int ret = drawTransparentText(lpstrText, cchText, lpRect, uFormat);
		SelectFont(oldFont);
		return ret;
	}

	void drawRectangle (LPRECT lpRect, int width, COLORREF color, int style = PS_SOLID | PS_INSIDEFRAME) {
		assert(lpRect != NULL && width > 0);
		HPEN pen = ::CreatePen(style, width, color);
		HPEN oldPen = SelectPen(pen);

		HBRUSH emptyBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
		HBRUSH oldBrush = SelectBrush(emptyBrush);

		Rectangle(lpRect);

		SelectBrush(oldBrush);
		::DeleteObject(emptyBrush);

		SelectPen(oldPen);
		::DeleteObject(pen);
	}
};

typedef CDCPlusT<true> CDC;
typedef CDCPlusT<false> CDCHandle;


//////////////////////////////////////////////////////////////////////////
// below are copied from WTL::atlgdi.h
class CPaintDC : public CDC
{
public:
	// Data members
	HWND m_hWnd;
	PAINTSTRUCT m_ps;

	// Constructor/destructor
	CPaintDC(HWND hWnd)
	{
		ATLASSERT(::IsWindow(hWnd));
		m_hWnd = hWnd;
		m_hDC = ::BeginPaint(hWnd, &m_ps);
	}

	~CPaintDC()
	{
		ATLASSERT(m_hDC != NULL);
		ATLASSERT(::IsWindow(m_hWnd));
		::EndPaint(m_hWnd, &m_ps);
		Detach();
	}
};

class CClientDC : public CDC
{
public:
	// Data members
	HWND m_hWnd;

	// Constructor/destructor
	CClientDC(HWND hWnd)
	{
		ATLASSERT(hWnd == NULL || ::IsWindow(hWnd));
		m_hWnd = hWnd;
		m_hDC = ::GetDC(hWnd);
	}

	~CClientDC()
	{
		ATLASSERT(m_hDC != NULL);
		::ReleaseDC(m_hWnd, Detach());
	}
};

class CWindowDC : public CDC
{
public:
	// Data members
	HWND m_hWnd;

	// Constructor/destructor
	CWindowDC(HWND hWnd)
	{
		ATLASSERT(hWnd == NULL || ::IsWindow(hWnd));
		m_hWnd = hWnd;
		m_hDC = ::GetWindowDC(hWnd);
	}

	~CWindowDC()
	{
		ATLASSERT(m_hDC != NULL);
		::ReleaseDC(m_hWnd, Detach());
	}
};

class CMemoryDC : public CDC
{
public:
	// Data members
	HDC m_hDCOriginal;
	RECT m_rcPaint;
	WTL::CBitmap m_bmp;
	HBITMAP m_hBmpOld;
	bool m_paintWhenDestroy;

	// Constructor/destructor
	CMemoryDC(HDC hDC, RECT& rcPaint, bool paintWhenDestroy = true)
		: m_hDCOriginal(hDC)
		, m_hBmpOld(NULL)
		, m_paintWhenDestroy(paintWhenDestroy)
	{
		m_rcPaint = rcPaint;
		CreateCompatibleDC(m_hDCOriginal);
		ATLASSERT(m_hDC != NULL);
		m_bmp.CreateCompatibleBitmap(m_hDCOriginal, m_rcPaint.right - m_rcPaint.left, m_rcPaint.bottom - m_rcPaint.top);
		ATLASSERT(m_bmp.m_hBitmap != NULL);
		m_hBmpOld = SelectBitmap(m_bmp);
		SetViewportOrg(-m_rcPaint.left, -m_rcPaint.top);
	}

	~CMemoryDC()
	{
		if (m_paintWhenDestroy)
		{
			::BitBlt(m_hDCOriginal, m_rcPaint.left, m_rcPaint.top, m_rcPaint.right - m_rcPaint.left, m_rcPaint.bottom - m_rcPaint.top, m_hDC, m_rcPaint.left, m_rcPaint.top, SRCCOPY);
		}
		SelectBitmap(m_hBmpOld);
	}
};
// copy end
//////////////////////////////////////////////////////////////////////////




UI_END
XL_END
#endif
