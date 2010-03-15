/**
 * Below code are copied (with minor modification) from FreeImage
 * (Filters.h, Resize.h, Resize.cpp and Rescale.cpp)
 */
#include <math.h>
#include <emmintrin.h>
#include "../../include/utilities.h"
#include "../../include/ui/DIBResizer.h"

#define USE_SSE
// #define USE_SSE2
// #define USE_FLOAT

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
	// xl::CTimerLogger logger(_T("--- construct weight table (%d - %d) cost: "), uDstSize, uSrcSize);
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
		if (!verticalFilter(tmp.get(), dst, pCallback)) {
			assert(pCallback && pCallback->shouldStop());
			return false;
		}

	} else {
		CDIBSectionPtr tmp = CDIBSection::createDIBSection(src_width, dst_height, bitcount, false);
		if (!tmp) {
			return false;
		}
		if (!verticalFilter(src, tmp.get(), pCallback)) {
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
                                     CDIBSection *dst, uint dst_yoffset, uint dst_height,
                                     ILongTimeRunCallback *pCallback) {
	assert(src->getBitCounts() == dst->getBitCounts());
	int bitcount = src->getBitCounts();
	assert((int)src_height <= src->getHeight());
	assert(src_height >= dst_height);
	uint dst_ymax = dst_yoffset + dst_height;
	assert((int)dst_ymax <= dst->getHeight());
	uint src_width = src->getWidth();
	uint dst_width = dst->getWidth();

	if (dst_width == src_width) {

		uint8 *src_bits = src->getData();
		uint8 *dst_bits = dst->getLine(dst_yoffset);
		assert(src_bits && dst_bits);

		uint height = min(dst_height, src_height);
		memcpy(dst_bits, src_bits, height * dst->getStride());

	} else if (!m_pFilter) { // fast (COLORONCOLOR)
		double ratio_w = (double)src_width / (double)dst_width;
		uint bytespp = bitcount / 8;

		for (uint y = dst_yoffset, sy = 0; y < dst_ymax; ++ y, ++ sy) {
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

	} else { // use m_pFilter
		uint index; // pixel index
		CWeightsTable weightsTable(m_pFilter, dst_width, src_width);
#ifdef USE_SSE
		__m128i value, t;
		__m128 a, b, c, v05 = _mm_set_ps1(0.5);
#elif (defined(USE_SSE2))
		__m128i value, t;
		__m128d a, b, c, v05 = _mm_set1_pd(0.5);
#endif

		uint bytespp = src->getBitCounts() / 8;
		assert(bytespp == 3 || bytespp == 4);
		for (uint dsty = dst_yoffset, srcy = 0; dsty < dst_ymax; ++ dsty, ++ srcy) {
			// test for stop
			if (srcy % 32 == 0) {
				if (pCallback && pCallback->shouldStop()) {
					return false;
				}
			}

			uint8 *src_bits = src->getLine(srcy);
			uint8 *dst_bits = dst->getLine(dsty);

			for(uint x = 0; x < dst_width; ++ x) {
				int iLeft = weightsTable.getLeftBoundary(x);
				int iRight = weightsTable.getRightBoundary(x);
				index = iLeft * bytespp;
#ifdef USE_SSE
				__m128 v = _mm_set_ps1(0.0);
				_mm_prefetch((const char *)src_bits + index, _MM_HINT_T0);
#elif defined(USE_SSE2)
				__m128d v1 = _mm_set1_pd(0.0);
				__m128d v2 = _mm_set1_pd(0.0);
#elif defined(USE_FLOAT)
				float value[4] = {0, 0, 0, 0};
#else
				double value[4] = {0, 0, 0, 0}; // 4 = 32bpp max
#endif
				for(int i = iLeft; i <= iRight; ++ i) {
#ifdef USE_SSE
					float weight = (float)weightsTable.getWeight(x, i - iLeft);

					a = _mm_set_ps1(weight);
					if (bytespp == 3) {
						t = _mm_set_epi32(0, src_bits[index + 2], src_bits[index + 1], src_bits[index]);
					} else {
						t = _mm_set_epi32(src_bits[index + 3], src_bits[index + 2], src_bits[index + 1], src_bits[index]);
					}
					b = _mm_cvtepi32_ps(t);
					c = _mm_mul_ps(a, b);
					v = _mm_add_ps(v, c);
					index += bytespp;
#elif defined(USE_SSE2)
					double weight = weightsTable.getWeight(x, i-iLeft);

					a = _mm_set1_pd(weight);
					t = _mm_set_epi32(0, 0, src_bits[index + 1], src_bits[index]);
					b = _mm_cvtepi32_pd(t);
					c = _mm_mul_pd(a, b);
					v1 = _mm_add_pd(v1, c);

					t = _mm_set_epi32(0, 0, bytespp == 3 ? 0 : src_bits[index + 3], src_bits[index + 2]);
					b = _mm_cvtepi32_pd(t);
					c = _mm_mul_pd(a, b);
					v2 = _mm_add_pd(v2, c);
					index += bytespp;
#elif defined(USE_FLOAT)
					float weight = (float)weightsTable.getWeight(x, i-iLeft);

					for (uint j = 0; j < bytespp; ++ j) {
						value[j] += (weight * (float)src_bits[index ++]); 
					}
#else
					double weight = weightsTable.getWeight(x, i-iLeft);

					for (uint j = 0; j < bytespp; ++ j) {
						value[j] += (weight * (double)src_bits[index ++]); 
					}
#endif
				} 

#ifdef USE_SSE
				v = _mm_add_ps(v, v05);
				value = _mm_cvtps_epi32(v);
				dst_bits[0] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[0]), (int)255);
				dst_bits[1] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[1]), (int)255);
				dst_bits[2] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[2]), (int)255);
				if (bytespp == 4) {
					dst_bits[3] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[3]), (int)255);
				}
#elif defined (USE_SSE2)
				v1 = _mm_add_pd(v1, v05);
				v2 = _mm_add_pd(v2, v05);
				value = _mm_cvtpd_epi32(v1);
				dst_bits[0] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[0]), (int)255);
				dst_bits[1] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[1]), (int)255);
				value = _mm_cvtpd_epi32(v2);
				dst_bits[2] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[0]), (int)255);
				if (bytespp == 4) {
					dst_bits[3] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[1]), (int)255);
				}
#else
				for (uint j = 0; j < bytespp; ++ j) {
					dst_bits[j] = (unsigned char)MIN(MAX((int)0, (int)(value[j] + 0.5)), (int)255);
				}
#endif

				dst_bits += bytespp;
			}
		}
	}
	return true;
}

bool CResizeEngine::verticalFilter(CDIBSection *src, CDIBSection *dst, ILongTimeRunCallback *pCallback) {
	assert(src->getBitCounts() == dst->getBitCounts());
	int bitcount = src->getBitCounts();
	uint src_width = src->getWidth();
	uint src_height = src->getHeight();
	uint dst_width = dst->getWidth();
	uint dst_height = dst->getHeight();
	assert(src_width == dst_width);
	src_width = src_width;
	if (src_height == dst_height) {

		unsigned char *src_bits = (unsigned char *)src->getData();
		unsigned char *dst_bits = (unsigned char *)dst->getData();
		assert(src_bits && dst_bits);

		memcpy(dst_bits, src_bits, dst_height * dst->getStride());

	} else if (!m_pFilter) { // fast (COLOR ON COLOR)

		double ratio_h = (double)src_height / (double)dst_height;

		uint bytespp = bitcount / 8;
		for (uint y = 0; y < dst_height; ++ y) {
			uint sy = (uint)(y * ratio_h + 0.5);
			if (sy >= src_height) {
				sy = src_height - 1;
			}
			uint8 *dst_data = (uint8 *)dst->getLine(y);
			uint8 *src_line = (uint8 *)src->getLine(sy);

			for (uint x = 0; x < dst_width; ++ x) {
				uint8 *src_data = src_line + x * bytespp;
				for (uint i = 0; i < bytespp; ++ i) {
					*dst_data ++ = *src_data ++;
				}
			}
		}

	} else {
#ifdef USE_SSE
		__m128i value, t;
		__m128 a, b, c, v05 = _mm_set_ps1(0.5);
#elif (defined(USE_SSE2))
		__m128i value, t;
		__m128d a, b, c, v05 = _mm_set1_pd(0.5);
#endif
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
#ifdef USE_SSE
				__m128 v = _mm_set_ps1(0.0);
#elif defined (USE_SSE2)
				__m128d v1 = _mm_set1_pd(0.0);
				__m128d v2 = _mm_set1_pd(0.0);
#elif defined (USE_FLOAT)
				float value[4] = {0, 0, 0, 0};
#else
				double value[4] = {0, 0, 0, 0}; // 4 = 32bpp max
#endif
				int iLeft = weightsTable.getLeftBoundary(y);
				int iRight = weightsTable.getRightBoundary(y);

				uint8 *src_bits = src->getLine(iLeft);
				src_bits += index;

				for(int i = iLeft; i <= iRight; ++ i) {
#ifdef USE_SSE
					float weight = (float)weightsTable.getWeight(y, i - iLeft);
					a = _mm_set_ps1(weight);
					if (bytespp == 3) {
						t = _mm_set_epi32(0, src_bits[2], src_bits[1], src_bits[0]);
					} else {
						t = _mm_set_epi32(src_bits[3], src_bits[2], src_bits[1], src_bits[0]);
					}

					b = _mm_cvtepi32_ps(t);
					c = _mm_mul_ps(a, b);
					v = _mm_add_ps(v, c);
#elif defined(USE_SSE2)
					double weight = weightsTable.getWeight(y, i - iLeft);

					a = _mm_set1_pd(weight);
					t = _mm_set_epi32(0, 0, src_bits[1], src_bits[0]);
					b = _mm_cvtepi32_pd(t);
					c = _mm_mul_pd(a, b);
					v1 = _mm_add_pd(v1, c);

					t = _mm_set_epi32(0, 0, bytespp == 3 ? 0 : src_bits[3], src_bits[2]);
					b = _mm_cvtepi32_pd(t);
					c = _mm_mul_pd(a, b);
					v2 = _mm_add_pd(v2, c);
#elif defined (USE_FLOAT)
					float weight = (float)weightsTable.getWeight(y, i - iLeft);							
					for (uint j = 0; j < bytespp; ++ j) {
						value[j] += (weight * (float)src_bits[j]);
					}
#else
					double weight = weightsTable.getWeight(y, i - iLeft);							
					for (uint j = 0; j < bytespp; ++ j) {
						value[j] += (weight * (double)src_bits[j]);
					}
#endif

					src_bits += src_pitch;
				}

				// clamp and place result in destination pixel
#ifdef USE_SSE
				v = _mm_add_ps(v, v05);
				value = _mm_cvtps_epi32(v);
// 				__m128i flag = _mm_cmpgt_epi32(value, _mm_set1_epi32(0));
// 				value = _mm_and_si128(value, flag);
// 				dst_bits[0] = (unsigned char)MIN(255, value.m128i_i32[0]);
// 				dst_bits[1] = (unsigned char)MIN(255, value.m128i_i32[1]);
// 				dst_bits[2] = (unsigned char)MIN(255, value.m128i_i32[2]);
// 				if (bytespp == 4) {
// 					dst_bits[3] = (unsigned char)MIN(255, value.m128i_i32[3]);
// 				}
				dst_bits[0] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[0]), (int)255);
				dst_bits[1] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[1]), (int)255);
				dst_bits[2] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[2]), (int)255);
				if (bytespp == 4) {
					dst_bits[3] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[3]), (int)255);
				}
#elif defined (USE_SSE2)
				v1 = _mm_add_pd(v1, v05);
				v2 = _mm_add_pd(v2, v05);
				value = _mm_cvtpd_epi32(v1);
 				dst_bits[0] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[0]), (int)255);
				dst_bits[1] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[1]), (int)255);
				value = _mm_cvtpd_epi32(v2);
				dst_bits[2] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[0]), (int)255);
				if (bytespp == 4) {
					dst_bits[3] = (unsigned char)MIN(MAX((int)0, value.m128i_i32[1]), (int)255);
				}
#else
				for (unsigned j = 0; j < bytespp; ++ j) {
					dst_bits[j] = (unsigned char)MIN(MAX((int)0, (int)(value[j] + 0.5)), (int)255);
				}
#endif

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
