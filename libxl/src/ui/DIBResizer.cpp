/**
 * Below code are copied (with minor modification) from FreeImage
 * (Filters.h, Resize.h, Resize.cpp and Rescale.cpp)
 */
#include <math.h>
#include "../../include/ui/DIBResizer.h"

XL_BEGIN
UI_BEGIN

template <class T> T MAX(T a, T b) {
	return (a > b) ? a: b;
}
template <class T> T MIN(T a, T b) {
	return (a < b) ? a: b;
}


//////////////////////////////////////////////////////////////////////////
// Weight Table
class CWeightsTable
{
	typedef struct {
		double *Weights;
		int Left, Right;   
	} Contribution;  

private:
	Contribution *m_WeightTable;
	uint m_WindowSize;
	uint m_LineLength;

public:
	CWeightsTable(CGenericFilter *pFilter, uint uDstSize, uint uSrcSize) {
		uint u;
		double dWidth;
		double dFScale = 1.0;
		double dFilterWidth = pFilter->GetWidth();

		double dScale = double(uDstSize) / double(uSrcSize);

		if(dScale < 1.0) {
			dWidth = dFilterWidth / dScale; 
			dFScale = dScale; 
		} else {
			dWidth= dFilterWidth; 
		}

		m_WindowSize = 2 * (int)ceil(dWidth) + 1; 
		m_LineLength = uDstSize; 
		m_WeightTable = (Contribution*)malloc(m_LineLength * sizeof(Contribution));
		for(u = 0 ; u < m_LineLength ; u++) {
			m_WeightTable[u].Weights = (double*)malloc(m_WindowSize * sizeof(double));
		}

		double dOffset = (0.5 / dScale) - 0.5;

		for(u = 0; u < m_LineLength; u++) {
			double dCenter = (double)u / dScale + dOffset;   // reverse mapping
			int iLeft = MAX (0, (int)floor (dCenter - dWidth)); 
			int iRight = MIN ((int)ceil (dCenter + dWidth), int(uSrcSize) - 1); 

			if((iRight - iLeft + 1) > int(m_WindowSize)) {
				if(iLeft < (int(uSrcSize) - 1 / 2)) {
					iLeft++; 
				} else {
					iRight--; 
				}
			}

			m_WeightTable[u].Left = iLeft; 
			m_WeightTable[u].Right = iRight;

			int iSrc = 0;
			double dTotalWeight = 0;
			for(iSrc = iLeft; iSrc <= iRight; iSrc++) {
				double weight = dFScale * pFilter->Filter(dFScale * (dCenter - (double)iSrc));
				m_WeightTable[u].Weights[iSrc-iLeft] = weight;
				dTotalWeight += weight;
			}
			if((dTotalWeight > 0) && (dTotalWeight != 1)) {
				for(iSrc = iLeft; iSrc <= iRight; iSrc++) {
					m_WeightTable[u].Weights[iSrc-iLeft] /= dTotalWeight; 
				}
				iSrc = iRight - iLeft;
				while(m_WeightTable[u].Weights[iSrc] == 0){
					m_WeightTable[u].Right--;
					iSrc--;
					if(m_WeightTable[u].Right == m_WeightTable[u].Left)
						break;
				}

			}
		} 
	}

	~CWeightsTable() {
		for(uint u = 0; u < m_LineLength; u++) {
			free(m_WeightTable[u].Weights);
		}
		free(m_WeightTable);
	}

	double getWeight(int dst_pos, int src_pos) {
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
CDIBSectionPtr CResizeEngine::scale(CDIBSection *src, uint dst_width, uint dst_height) {
	uint src_width  = (uint)src->getWidth();
	uint src_height = (uint)src->getHeight();
	int bitcount = src->getBitCounts();
	assert(bitcount == 24 || bitcount == 32);
	CDIBSectionPtr dib = CDIBSection::createDIBSection(dst_width, dst_height, bitcount, false);
	if (!dib) {
		return dib;
	}

	if(dst_width * src_height <= dst_height * src_width) {
		CDIBSectionPtr tmp = CDIBSection::createDIBSection(dst_width, src_height, bitcount, false);
		if (!tmp) {
			dib.reset();
			return CDIBSectionPtr();
		}

		horizontalFilter(src, src_width, src_height, tmp.get(), dst_width, src_height);
		verticalFilter(tmp.get(), dst_width, src_height, dib.get(), dst_width, dst_height);

	} else {
		CDIBSectionPtr tmp = CDIBSection::createDIBSection(src_width, dst_height, bitcount, false);
		if (!tmp) {
			dib.reset();
			return CDIBSectionPtr();
		}
		verticalFilter(src, src_width, src_height, tmp.get(), src_width, dst_height);
		horizontalFilter(tmp.get(), src_width, dst_height, dib.get(), dst_width, dst_height);
	}

	return dib;
}

void CResizeEngine::horizontalFilter(CDIBSection *src, uint src_width, uint src_height,
                                     CDIBSection *dst, uint dst_width, uint dst_height) {
	if (dst_width == src_width) {
		unsigned char *src_bits = (unsigned char *)src->getData();
		unsigned char *dst_bits = (unsigned char *)dst->getData();
		assert(src_bits && dst_bits);

		memcpy(dst_bits, src_bits, dst_height * dst->getStride());
	} else {
		uint index; // pixel index
		CWeightsTable weightsTable(m_pFilter, dst_width, src_width);

		uint bytespp = src->getBitCounts() / 8;
		assert(bytespp == 3 || bytespp == 4);
		for (uint y = 0; y < dst_height; ++ y) {
			unsigned char *src_bits = (unsigned char *)src->getLine(y);
			unsigned char *dst_bits = (unsigned char *)dst->getLine(y);

			for(uint x = 0; x < dst_width; x++) {
				double value[4] = {0, 0, 0, 0}; // 4 = 32bpp max
				int iLeft = weightsTable.getLeftBoundary(x);
				int iRight = weightsTable.getRightBoundary(x);

				for(int i = iLeft; i <= iRight; i++) {
					double weight = weightsTable.getWeight(x, i-iLeft);

					index = i * bytespp;
					for (unsigned j = 0; j < bytespp; j++) {
						value[j] += (weight * (double)src_bits[index++]); 
					}
				} 

				for (unsigned j = 0; j < bytespp; j++) {
					dst_bits[j] = (unsigned char)MIN(MAX((int)0, (int)(value[j] + 0.5)), (int)255);
				}

				dst_bits += bytespp;
			}
		}
	}
}

void CResizeEngine::verticalFilter(CDIBSection *src, uint src_width, uint src_height,
				   CDIBSection *dst, uint dst_width, uint dst_height) {
	if (src_height == dst_height) {
		unsigned char *src_bits = (unsigned char *)src->getData();
		unsigned char *dst_bits = (unsigned char *)dst->getData();
		assert(src_bits && dst_bits);

		memcpy(dst_bits, src_bits, dst_height * dst->getStride());
	} else {
		uint index; // pixel index
		CWeightsTable weightsTable(m_pFilter, dst_height, src_height);

		uint bytespp = src->getBitCounts() / 8;
		assert(bytespp == 3 || bytespp == 4);

		unsigned src_pitch = src->getStride();
		unsigned dst_pitch = dst->getStride();

		for(uint x = 0; x < dst_width; x++) {
			index = x * bytespp;

			unsigned char *dst_bits = (unsigned char *)dst->getData();
			dst_bits += index;

			for(uint y = 0; y < dst_height; y++) {
				double value[4] = {0, 0, 0, 0}; // 4 = 32bpp max
				int iLeft = weightsTable.getLeftBoundary(y);
				int iRight = weightsTable.getRightBoundary(y);

				unsigned char *src_bits = (unsigned char *)src->getLine(iLeft);
				src_bits += index;

				for(int i = iLeft; i <= iRight; i++) {
					double weight = weightsTable.getWeight(y, i-iLeft);							
					for (unsigned j = 0; j < bytespp; j++) {
						value[j] += (weight * (double)src_bits[j]);
					}

					src_bits += src_pitch;
				}

				// clamp and place result in destination pixel
				for (unsigned j = 0; j < bytespp; j++) {
					dst_bits[j] = (BYTE)MIN(MAX((int)0, (int)(value[j] + 0.5)), (int)255);
				}

				dst_bits += dst_pitch;
			}
		}
	}
}

UI_END
XL_END
