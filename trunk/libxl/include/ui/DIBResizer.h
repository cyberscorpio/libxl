#ifndef XL_UI_DIBRESIZER_H
#define XL_UI_DIBRESIZER_H
#include "DIBSection.h"

XL_BEGIN
UI_BEGIN

//////////////////////////////////////////////////////////////////////////
// filters

class CGenericFilter {
protected:

#define FILTER_PI  double (3.1415926535897932384626433832795)
#define FILTER_2PI double (2.0 * 3.1415926535897932384626433832795)
#define FILTER_4PI double (4.0 * 3.1415926535897932384626433832795)

	double  m_dWidth;

public:
	CGenericFilter (double dWidth) : m_dWidth (dWidth) {}
	virtual ~CGenericFilter() {}

	double GetWidth() { return m_dWidth; }
	void SetWidth (double dWidth) { m_dWidth = dWidth; }

	/// Returns F(dVal) where F is the filter's impulse response
	virtual double Filter (double dVal) = 0;
};

// Box filter
class CBoxFilter : public CGenericFilter {
public:
	CBoxFilter() : CGenericFilter(0.5) {}
	virtual ~CBoxFilter() {}

	double Filter (double dVal) { return (fabs(dVal) <= m_dWidth ? 1.0 : 0.0); }
};

// Mitchell & Netravali's two-param cubic filter
class CBicubicFilter : public CGenericFilter {
protected:
	double p0, p2, p3;
	double q0, q1, q2, q3;

public:
	CBicubicFilter (double b = (1/(double)3), double c = (1/(double)3)) 
		: CGenericFilter(2)
	{
		p0 = (6 - 2*b) / 6;
		p2 = (-18 + 12*b + 6*c) / 6;
		p3 = (12 - 9*b - 6*c) / 6;
		q0 = (8*b + 24*c) / 6;
		q1 = (-12*b - 48*c) / 6;
		q2 = (6*b + 30*c) / 6;
		q3 = (-b - 6*c) / 6;
	}
	virtual ~CBicubicFilter() {}

	double Filter(double dVal) { 
		dVal = fabs(dVal);
		if(dVal < 1) {
			return (p0 + dVal*dVal*(p2 + dVal*p3));
		} else if(dVal < 2) {
			return (q0 + dVal*(q1 + dVal*(q2 + dVal*q3)));
		}
		return 0;
	}
};

//////////////////////////////////////////////////////////////////////////
// Resize Engine
class CResizeEngine
{
private:
	CGenericFilter* m_pFilter;

public:
	CResizeEngine(CGenericFilter* filter) : m_pFilter(filter) {}
	virtual ~CResizeEngine() {}

	/** Scale an image to the desired dimensions
	 * @param src Pointer to the source image
	 * @param dst_width Destination image width
	 * @param dst_height Destination image height
	 * @return Returns the scaled image if successful, returns NULL otherwise
	*/
	CDIBSectionPtr scale(CDIBSection *src, uint dst_width, uint dst_height);

private:
	void horizontalFilter(CDIBSection *src, uint src_width, uint src_height, CDIBSection *dst, uint dst_width, uint dst_height);

	void verticalFilter(CDIBSection *src, uint src_width, uint src_height, CDIBSection *dst, uint dst_width, uint dst_height);
};


UI_END
XL_END
#endif