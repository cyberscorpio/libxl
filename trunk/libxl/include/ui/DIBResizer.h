#ifndef XL_UI_DIBRESIZER_H
#define XL_UI_DIBRESIZER_H
#include "../interfaces.h"
#include "DIBResizerFilter.h"
#include "DIBSection.h"

XL_BEGIN
UI_BEGIN

//////////////////////////////////////////////////////////////////////////
// Weight Table
class CWeightsTable
{
	typedef struct {
		double *Weights;
		int Left, Right;   
	} Contribution;  

protected:
	Contribution *m_WeightTable;
	uint m_WindowSize;
	uint m_LineLength;

public:
	CWeightsTable(CGenericFilter *pFilter, uint uDstSize, uint uSrcSize);
	~CWeightsTable();

	inline double getWeight(int dst_pos, int src_pos) {
		return m_WeightTable[dst_pos].Weights[src_pos];
	}

	int getLeftBoundary(int dst_pos) {
		return m_WeightTable[dst_pos].Left;
	}

	int getRightBoundary(int dst_pos) {
		return m_WeightTable[dst_pos].Right;
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
	// CDIBSectionPtr scale(CDIBSection *src, uint dst_width, uint dst_height);
	bool scale(CDIBSection *src, CDIBSection *dst, ILongTimeRunCallback *pCallback = NULL);

	bool horizontalFilter(CDIBSection *src, uint src_height,
		CDIBSection *dst, uint dst_offset, uint dst_height,
		ILongTimeRunCallback *pCallback);
	bool verticalFilter(CDIBSection *src, CDIBSection *dst, ILongTimeRunCallback *pCallback);

protected:
	void _FastScale (CDIBSection *src, CDIBSection *dst);
};


UI_END
XL_END
#endif