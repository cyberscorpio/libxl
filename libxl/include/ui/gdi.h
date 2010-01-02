#ifndef XL_UI_GDI_H
#define XL_UI_GDI_H
#include <atlbase.h>
#include <atltypes.h>
#include <atlapp.h>
#include <atlgdi.h>

namespace xl {
	namespace ui {

template <bool t_bManaged>
class CDCPlusT
	: public WTL::CDCT<t_bManaged>
{
public:
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
};

typedef CDCPlusT<true> CDC;
typedef CDCPlusT<false> CDCHandle;


//////////////////////////////////////////////////////////////////////////
// blow copied from WTL::atlgdi.h
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
	CBitmap m_bmp;
	HBITMAP m_hBmpOld;

	// Constructor/destructor
	CMemoryDC(HDC hDC, RECT& rcPaint) : m_hDCOriginal(hDC), m_hBmpOld(NULL)
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
		::BitBlt(m_hDCOriginal, m_rcPaint.left, m_rcPaint.top, m_rcPaint.right - m_rcPaint.left, m_rcPaint.bottom - m_rcPaint.top, m_hDC, m_rcPaint.left, m_rcPaint.top, SRCCOPY);
		SelectBitmap(m_hBmpOld);
	}
};
// copy end
//////////////////////////////////////////////////////////////////////////




	} // namespace ui
} // namespace xl

#endif
