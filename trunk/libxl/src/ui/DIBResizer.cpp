/**
 * Below code are copied (with minor modification) from FreeImage
 * (Filters.h, Resize.h, Resize.cpp and Rescale.cpp)
 */
#include <math.h>
#include "../../include/utilities.h"
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
CWeightsTable::CWeightsTable(CGenericFilter *pFilter, uint uDstSize, uint uSrcSize) {
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
	// xl::trace(_T("scale: %.4f, width: %.4f\n"), dScale, dWidth);

	m_WindowSize = 2 * (int)ceil(dWidth) + 1; 
	m_LineLength = uDstSize; 
	m_WeightTable = (Contribution *)malloc(m_LineLength * sizeof(Contribution));
	double *weights = (double *)malloc(m_WindowSize * m_LineLength * sizeof(double)); // continuous memory maybe cache friendly
	for(u = 0; u < m_LineLength; ++ u) {
		m_WeightTable[u].Weights = weights + m_WindowSize * u;
	}

	double dOffset = (0.5 / dScale) - 0.5;

	for(u = 0; u < m_LineLength; ++ u) {
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
		for(iSrc = iLeft; iSrc <= iRight; ++ iSrc) {
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

CWeightsTable::~CWeightsTable() {
	free(m_WeightTable[0].Weights);
	free(m_WeightTable);
}



//////////////////////////////////////////////////////////////////////////
// Resize Engine
#if 0
CDIBSectionPtr CResizeEngine::scale(CDIBSection *src, uint dst_width, uint dst_height) {
	assert(src != NULL);
	assert(dst_width > 0 && dst_height > 0);
	uint src_width  = (uint)src->getWidth();
	uint src_height = (uint)src->getHeight();
	int bitcount = src->getBitCounts();
	assert(bitcount == 24 || bitcount == 32);
	CDIBSectionPtr dib = CDIBSection::createDIBSection(dst_width, dst_height, bitcount, false);
	if (!dib) {
		return dib;
	}

	scale(src, dib.get());

	return dib;
}
#endif

bool CResizeEngine::scale (CDIBSection *src, CDIBSection *dst, ILongTimeRunCallback *pCallback) {
	assert(src != NULL && dst != NULL);
	uint src_width  = (uint)src->getWidth();
	uint src_height = (uint)src->getHeight();
	uint dst_width = (uint)dst->getWidth();
	uint dst_height = (uint)dst->getHeight();
	int bitcount = src->getBitCounts();
	assert(dst_width > 0 && dst_height > 0);
	assert(bitcount == 24 || bitcount == 32);

	if (m_pFilter == NULL) {
		_FastScale(src, dst);
		return true;
	}

	if(dst_width * src_height <= dst_height * src_width) {
		CDIBSectionPtr tmp = CDIBSection::createDIBSection(dst_width, src_height, bitcount, false);
		if (!tmp) {
			return false;
		}

		if (!horizontalFilter(src, src_height, tmp.get(), 0, src_height, pCallback)) {
			assert(pCallback && pCallback->shouldStop());
			return false;
		}
		if (!verticalFilter(tmp.get(), dst_width, src_height, dst, dst_width, dst_height, pCallback)) {
			assert(pCallback && pCallback->shouldStop());
			return false;
		}

	} else {
		CDIBSectionPtr tmp = CDIBSection::createDIBSection(src_width, dst_height, bitcount, false);
		if (!tmp) {
			return false;
		}
		if (!verticalFilter(src, src_width, src_height, tmp.get(), src_width, dst_height, pCallback)) {
			assert(pCallback && pCallback->shouldStop());
			return false;
		}
		if (!horizontalFilter(tmp.get(), dst_height, dst, 0, dst_height, pCallback)) {
			assert(pCallback && pCallback->shouldStop());
			return false;
		}
	}

	return true;
}

bool CResizeEngine::horizontalFilter(CDIBSection *src, uint src_height,
                                      CDIBSection *dst, uint dst_offset_y, uint dst_height,
                                      ILongTimeRunCallback *pCallback) {
	assert(m_pFilter != NULL);
	assert(src->getBitCounts() == dst->getBitCounts());
	assert((int)src_height <= src->_GetHeightNoLock());
	uint dst_ymax = dst_offset_y + dst_height;
	assert((int)dst_ymax <= dst->getHeight());
	uint src_width = src->_GetWidthNoLock();
	uint dst_width = dst->getWidth();
	if (dst_width == src_width) {
		unsigned char *src_bits = (unsigned char *)src->_GetDataNoLock();
		unsigned char *dst_bits = (unsigned char *)dst->getLine(dst_offset_y);
		assert(src_bits && dst_bits);

		uint height = min(dst_height, src_height);
		memcpy(dst_bits, src_bits, height * dst->getStride());
	} else {
		uint index; // pixel index
		CWeightsTable weightsTable(m_pFilter, dst_width, src_width);

		uint bytespp = src->getBitCounts() / 8;
		assert(bytespp == 3 || bytespp == 4);
		for (uint dsty = dst_offset_y, srcy = 0; dsty < dst_ymax; ++ dsty, ++ srcy) {
			// test for stop
			if (srcy % 32 == 0) {
				if (pCallback && pCallback->shouldStop()) {
					return false;
				}
			}

			unsigned char *src_bits = (unsigned char *)src->_GetLineNoLock(srcy);
			unsigned char *dst_bits = (unsigned char *)dst->_GetLineNoLock(dsty);

			for(uint x = 0; x < dst_width; ++ x) {
				double value[4] = {0, 0, 0, 0}; // 4 = 32bpp max
				int iLeft = weightsTable.getLeftBoundary(x);
				int iRight = weightsTable.getRightBoundary(x);

				for(int i = iLeft; i <= iRight; ++ i) {
					double weight = weightsTable.getWeight(x, i-iLeft);

					index = i * bytespp;
					for (uint j = 0; j < bytespp; ++ j) {
						value[j] += (weight * (double)src_bits[index++]); 
					}
				} 

				for (uint j = 0; j < bytespp; ++ j) {
					dst_bits[j] = (unsigned char)MIN(MAX((int)0, (int)(value[j] + 0.5)), (int)255);
				}

				dst_bits += bytespp;
			}
		}
	}
	return true;
}

bool CResizeEngine::verticalFilter(CDIBSection *src, uint src_width, uint src_height,
                                    CDIBSection *dst, uint dst_width, uint dst_height,
                                    ILongTimeRunCallback *pCallback) {
	assert(m_pFilter != NULL);
	assert(src->getBitCounts() == dst->getBitCounts());
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

		for(uint x = 0; x < dst_width; ++ x) {
			// test for stop
			if (x % 16 == 0) {
				if (pCallback && pCallback->shouldStop()) {
					return false;
				}
			}
			index = x * bytespp;

			unsigned char *dst_bits = (unsigned char *)dst->getData();
			dst_bits += index;

			for(uint y = 0; y < dst_height; ++ y) {
				double value[4] = {0, 0, 0, 0}; // 4 = 32bpp max
				int iLeft = weightsTable.getLeftBoundary(y);
				int iRight = weightsTable.getRightBoundary(y);

				unsigned char *src_bits = (unsigned char *)src->_GetLineNoLock(iLeft);
				src_bits += index;

				for(int i = iLeft; i <= iRight; ++ i) {
					double weight = weightsTable.getWeight(y, i - iLeft);							
					for (uint j = 0; j < bytespp; ++ j) {
						value[j] += (weight * (double)src_bits[j]);
					}

					src_bits += src_pitch;
				}

				// clamp and place result in destination pixel
				for (unsigned j = 0; j < bytespp; ++ j) {
					dst_bits[j] = (unsigned char)MIN(MAX((int)0, (int)(value[j] + 0.5)), (int)255);
				}

				dst_bits += dst_pitch;
			}
		}
	}
	return true;
}

void CResizeEngine::_FastScale (CDIBSection *src, CDIBSection *dst) {
	assert(src != NULL && dst != NULL);
	assert(src->getBitCounts() == dst->getBitCounts());
	uint bitcount = src->getBitCounts();
	assert(bitcount == 24 || bitcount == 32);
	uint src_width = src->getWidth();
	uint src_height = src->getHeight();
	uint dst_width = dst->getWidth();
	uint dst_height = dst->getHeight();
	double ratio_w = (double)src_width / (double)dst_width;
	double ratio_h = (double)src_height / (double)dst_height;

	uint bytespp = bitcount / 8;
	uint src_stride = src->getStride();
	uint dst_stride = dst->getStride();

	for (uint y = 0; y < dst_height; ++ y) {
		uint sy = (uint)(y * ratio_h + 0.5);
		if (sy >= src_height) {
			sy = src_height - 1;
		}
		uint8 *dst_data = (uint8 *)dst->getLine(y);
		uint8 *src_line = (uint8 *)src->getLine(sy);

		for (uint x = 0; x < dst_width; ++ x) {
			uint sx = (uint)(x * ratio_w + 0.5);
			if (sx >= src_width) {
				sx = src_width - 1;
			}

			uint8 *src_data = src_line + sx * bytespp;
			for (uint i = 0; i < bytespp; ++ i) {
				*dst_data ++ = *src_data ++;
			}
		}
	}
}

UI_END
XL_END
