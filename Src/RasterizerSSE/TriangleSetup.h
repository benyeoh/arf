//================================================================================
//
//		TriangleSetup.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/25/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

template <uint numInterpolants, uint topTileSize, uint midTileSize>
boolean TriangleSetup2Layer(const gmtl::VecA4f& v1, const gmtl::VecA4f& v2, const gmtl::VecA4f& v3,
	const float* pInterpolantsV1, const float* pInterpolantsV2, const float* pInterpolantsV3, 
	uint bufferWidth, uint bufferHeight, void* pRasterInfo)
{
	RasterInfo<numInterpolants, 2>& raster = *((RasterInfo<numInterpolants, 2>*) pRasterInfo);

	// Compute inverse of 3x3

	__m128 col1 = _mm_load_ps((float*) &v1);
	__m128 col2 = _mm_load_ps((float*) &v2);
	__m128 col3 = _mm_load_ps((float*) &v3);

	// Get the 9 matrix values
	__m128 _33_33_33_33 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(3, 3, 3, 3));

	//__m128 _11_21_12_22	= _mm_shuffle_ps(col1, col2, _MM_SHUFFLE(1, 0, 1, 0));
	//__m128 _11_12_21_22	= _mm_shuffle_ps(_11_21_12_22, _11_21_12_22, _MM_SHUFFLE(3, 1, 2, 0));

	__m128 _11_12_21_22	= _mm_unpacklo_ps(col1, col2);

	__m128 _31_31_32_32 = _mm_shuffle_ps(col1, col2, _MM_SHUFFLE(3, 3, 3, 3));
	__m128 _32_31_32_31 = _mm_shuffle_ps(_31_31_32_32, _31_31_32_32, _MM_SHUFFLE(0, 3, 0, 3));

	__m128 _23_23_13_13 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(0, 0, 1, 1));

	__m128 _22_21_12_11 = _mm_shuffle_ps(_11_12_21_22, _11_12_21_22, _MM_SHUFFLE(0, 1, 2, 3));

	__m128 _3311_3312_3321_3322 = _mm_mul_ps(_33_33_33_33, _11_12_21_22);
	__m128 _3223_3123_3213_3113 = _mm_mul_ps(_32_31_32_31, _23_23_13_13);
	__m128 _3211_3112_3221_3122 = _mm_mul_ps(_32_31_32_31, _11_12_21_22);
	__m128 _2311_2312_2113_2213 = _mm_mul_ps(_23_23_13_13, _11_12_21_22);
	__m128 _2211_2112_2112_2211 = _mm_mul_ps(_22_21_12_11, _11_12_21_22);

	__m128 _3221_3112_2312_2113 = _mm_shuffle_ps(_3211_3112_3221_3122, _2311_2312_2113_2213, _MM_SHUFFLE(2, 1, 1, 2));
	__m128 _3122_3211_2213_2311 = _mm_shuffle_ps(_3211_3112_3221_3122, _2311_2312_2113_2213, _MM_SHUFFLE(0, 3, 0, 3));
	__m128 _3221s3122_3112s3211_2312s2213_2113s2311 = _mm_sub_ps(_3221_3112_2312_2113, _3122_3211_2213_2311);

	__m128 _3322_3311_3123_3213 = _mm_shuffle_ps(_3311_3312_3321_3322, _3223_3123_3213_3113, _MM_SHUFFLE(2, 1, 0, 3));
	__m128 _3223_3113_3321_3312 = _mm_shuffle_ps(_3223_3123_3213_3113, _3311_3312_3321_3322, _MM_SHUFFLE(1, 2, 3, 0));
	__m128 _3322s3223_3311s3113_3123s3321_3213s3312 = _mm_sub_ps(_3322_3311_3123_3213, _3223_3113_3321_3312);

	__m128 _2112_2112_2211_2211 = _mm_unpackhi_ps(_2211_2112_2112_2211, _2211_2112_2112_2211);
	__m128 _2211s2112 = _mm_sub_ss(_2211_2112_2112_2211, _2112_2112_2211_2211);

	// Shuffle to rows
	// This is the 3x3 adjoint matrix
	__m128 _3322s3223_3213s3312_2312s2213_2312s2213 = _mm_shuffle_ps(_3322s3223_3311s3113_3123s3321_3213s3312, _3221s3122_3112s3211_2312s2213_2113s2311, _MM_SHUFFLE(2, 2, 3, 0));
	__m128 _3123s3321_3311s3113_2113s2311_2113s2311 = _mm_shuffle_ps(_3322s3223_3311s3113_3123s3321_3213s3312, _3221s3122_3112s3211_2312s2213_2113s2311, _MM_SHUFFLE(3, 3, 1, 2));
	__m128 _3221s3122_3112s3211_2211s2112_2211s2112 = _mm_shuffle_ps(_3221s3122_3112s3211_2312s2213_2113s2311, _2211s2112, _MM_SHUFFLE(0, 0, 1, 0));

	// Compute determinant
	__m128 determinant		= _mm_mul_ps(col1, _3322s3223_3213s3312_2312s2213_2312s2213);
	__m128 col1y_3213s3312	= _mm_shuffle_ps(determinant, determinant, _MM_SHUFFLE(1, 1, 1, 1));
	__m128 col1w_2312s2213	= _mm_shuffle_ps(determinant, determinant, _MM_SHUFFLE(3, 3, 3, 3));

	determinant = _mm_add_ss(determinant, col1y_3213s3312);
	determinant = _mm_add_ss(determinant, col1w_2312s2213);

	// Only accept front facing triangles for now
	__m128 detRes = _mm_cmpgt_ss(determinant, _mm_setzero_ps());
	int detMask = _mm_movemask_ps(detRes);
	//float det;
	//_mm_store_ss(&det, determinant);
	if(detMask > 0)
	{
		// Compute the edge functions
		// Make parameters relative to viewport space
		const __m128 SCREEN_BUFFER_SCALE = _mm_set_ps(1.0f, 1.0f, -2.0f / bufferHeight, 2.0f / bufferWidth);

		// Row 1
		__m128 _B = _mm_shuffle_ps(_3322s3223_3213s3312_2312s2213_2312s2213, _3322s3223_3213s3312_2312s2213_2312s2213, _MM_SHUFFLE(1, 1, 1, 1));
		__m128 _C = _mm_movehl_ps(_3322s3223_3213s3312_2312s2213_2312s2213, _3322s3223_3213s3312_2312s2213_2312s2213);
		__m128 _CaBsA = _mm_add_ss(_C, _B);
		_CaBsA = _mm_sub_ss(_CaBsA, _3322s3223_3213s3312_2312s2213_2312s2213);
		__m128 _3322s3223_3213s3312_CaBsA_3213s3312 = _mm_movelh_ps(_3322s3223_3213s3312_2312s2213_2312s2213, _CaBsA);
		__m128 row1_ABC = _mm_mul_ps(_3322s3223_3213s3312_CaBsA_3213s3312, SCREEN_BUFFER_SCALE);

		// Row 2
		_B = _mm_shuffle_ps(_3123s3321_3311s3113_2113s2311_2113s2311, _3123s3321_3311s3113_2113s2311_2113s2311, _MM_SHUFFLE(1, 1, 1, 1));
		_C = _mm_movehl_ps(_3123s3321_3311s3113_2113s2311_2113s2311, _3123s3321_3311s3113_2113s2311_2113s2311);
		_CaBsA = _mm_add_ss(_C, _B);
		_CaBsA = _mm_sub_ss(_CaBsA, _3123s3321_3311s3113_2113s2311_2113s2311);
		__m128 _3123s3321_3311s3113_CaBsA_3311s3113 = _mm_movelh_ps(_3123s3321_3311s3113_2113s2311_2113s2311, _CaBsA);
		__m128 row2_ABC = _mm_mul_ps(_3123s3321_3311s3113_CaBsA_3311s3113, SCREEN_BUFFER_SCALE);

		// Row 3
		_B = _mm_shuffle_ps(_3221s3122_3112s3211_2211s2112_2211s2112, _3221s3122_3112s3211_2211s2112_2211s2112, _MM_SHUFFLE(1, 1, 1, 1));
		_C = _mm_movehl_ps(_3221s3122_3112s3211_2211s2112_2211s2112, _3221s3122_3112s3211_2211s2112_2211s2112);
		_CaBsA = _mm_add_ss(_C, _B);
		_CaBsA = _mm_sub_ss(_CaBsA, _3221s3122_3112s3211_2211s2112_2211s2112);
		__m128 _3221s3122_3112s3211_CaBsA_3112s3211 = _mm_movelh_ps(_3221s3122_3112s3211_2211s2112_2211s2112, _CaBsA);
		__m128 row3_ABC = _mm_mul_ps(_3221s3122_3112s3211_CaBsA_3112s3211, SCREEN_BUFFER_SCALE);

		// Normalize to A and B
		// Since we only care about the half space aspect of the edge functions,
		// we can normalize with respect to screen/pixel space to reduce floating point
		// precision issues with very large triangles or very thin ones
		__m128 _B2 = _mm_shuffle_ps(row1_ABC, row1_ABC, _MM_SHUFFLE(1, 1, 1, 1));
		_B2 = _mm_mul_ss(_B2, _B2);
		__m128 _A2 = _mm_mul_ss(row1_ABC, row1_ABC);
		__m128 rcpSqrtAB = _mm_add_ss(_B2, _A2);
		rcpSqrtAB = _mm_rsqrt_ss(rcpSqrtAB);
		rcpSqrtAB = _mm_shuffle_ps(rcpSqrtAB, rcpSqrtAB, _MM_SHUFFLE(0, 0, 0, 0));
		row1_ABC = _mm_mul_ps(row1_ABC, rcpSqrtAB);

		_B2 = _mm_shuffle_ps(row2_ABC, row2_ABC, _MM_SHUFFLE(1, 1, 1, 1));
		_B2 = _mm_mul_ss(_B2, _B2);
		_A2 = _mm_mul_ss(row2_ABC, row2_ABC);
		rcpSqrtAB = _mm_add_ss(_B2, _A2);
		rcpSqrtAB = _mm_rsqrt_ss(rcpSqrtAB);
		rcpSqrtAB = _mm_shuffle_ps(rcpSqrtAB, rcpSqrtAB, _MM_SHUFFLE(0, 0, 0, 0));
		row2_ABC = _mm_mul_ps(row2_ABC, rcpSqrtAB);

		_B2 = _mm_shuffle_ps(row3_ABC, row3_ABC, _MM_SHUFFLE(1, 1, 1, 1));
		_B2 = _mm_mul_ss(_B2, _B2);
		_A2 = _mm_mul_ss(row3_ABC, row3_ABC);
		rcpSqrtAB = _mm_add_ss(_B2, _A2);
		rcpSqrtAB = _mm_rsqrt_ss(rcpSqrtAB);
		rcpSqrtAB = _mm_shuffle_ps(rcpSqrtAB, rcpSqrtAB, _MM_SHUFFLE(0, 0, 0, 0));
		row3_ABC = _mm_mul_ps(row3_ABC, rcpSqrtAB);

		const static __m128 NEG = _mm_set_ps1(-1.0f);
		const static __m128 ZERO = _mm_set_ps1(0.0f);
		static const __m128 ONE = _mm_set_ps1(1.0f);

		// Edge 1 -> (0, 0, 1) -> row 3
		// Edge 2 -> (1, 0, 0) -> row 1
		// Edge 3 -> (0, 1, 0) -> row 2
		__m128 edge1 = row3_ABC;
		__m128 edge2 = row1_ABC;
		__m128 edge3 = row2_ABC;

		raster.e1aaaa = _mm_shuffle_ps(edge1, edge1, _MM_SHUFFLE(0, 0, 0, 0));
		raster.e1bbbb = _mm_shuffle_ps(edge1, edge1, _MM_SHUFFLE(1, 1, 1, 1));
		raster.e1cccc = _mm_shuffle_ps(edge1, edge1, _MM_SHUFFLE(2, 2, 2, 2));

		raster.e2aaaa = _mm_shuffle_ps(edge2, edge2, _MM_SHUFFLE(0, 0, 0, 0));
		raster.e2bbbb = _mm_shuffle_ps(edge2, edge2, _MM_SHUFFLE(1, 1, 1, 1));
		raster.e2cccc = _mm_shuffle_ps(edge2, edge2, _MM_SHUFFLE(2, 2, 2, 2));

		raster.e3aaaa = _mm_shuffle_ps(edge3, edge3, _MM_SHUFFLE(0, 0, 0, 0));
		raster.e3bbbb = _mm_shuffle_ps(edge3, edge3, _MM_SHUFFLE(1, 1, 1, 1));
		raster.e3cccc = _mm_shuffle_ps(edge3, edge3, _MM_SHUFFLE(2, 2, 2, 2));	

		// Top tile
		const static _ALIGN(16) float TOP_TILE_OFFSET[] = { topTileSize * 2.0f, topTileSize * 2.0f, topTileSize * 2.0f, topTileSize * 2.0f };

		__m128 topTileAddFactor = _mm_load_ps(TOP_TILE_OFFSET);
		raster.e1XLayerAdd[0] = _mm_mul_ps(raster.e1aaaa, topTileAddFactor);
		raster.e2XLayerAdd[0] = _mm_mul_ps(raster.e2aaaa, topTileAddFactor);
		raster.e3XLayerAdd[0] = _mm_mul_ps(raster.e3aaaa, topTileAddFactor);

		raster.e1YLayerAdd[0] = _mm_mul_ps(raster.e1bbbb, topTileAddFactor);
		raster.e2YLayerAdd[0] = _mm_mul_ps(raster.e2bbbb, topTileAddFactor);
		raster.e3YLayerAdd[0] = _mm_mul_ps(raster.e3bbbb, topTileAddFactor);

		// E1
		// Reject
		const static _ALIGN(16) float TOP_QUAD_SIZE[] = { (float)(topTileSize-1), (float)(topTileSize-1), (float)(topTileSize-1), (float)(topTileSize-1) };

		__m128 e1aaaaCmpGt = _mm_cmpgt_ps(raster.e1aaaa, ZERO);
		__m128 e1aaaaCmpGtFactor = _mm_and_ps(e1aaaaCmpGt, raster.e1aaaa);
		__m128 topQuadSize = _mm_load_ps(TOP_QUAD_SIZE);
		__m128 e1aaaaCmpGtQuadOffset = _mm_mul_ps(e1aaaaCmpGtFactor, topQuadSize);		// A * quadSize
		raster.e1RejectLayerCCCC[0] = _mm_add_ps(raster.e1cccc, e1aaaaCmpGtQuadOffset);		// C + A * quadSize	

		__m128 e1bbbbCmpGt = _mm_cmpgt_ps(raster.e1bbbb, ZERO);
		__m128 e1bbbbCmpGtFactor = _mm_and_ps(e1bbbbCmpGt, raster.e1bbbb);
		__m128 e1bbbbCmpGtQuadOffset = _mm_mul_ps(e1bbbbCmpGtFactor, topQuadSize);			// B * quadSize
		raster.e1RejectLayerCCCC[0] = _mm_add_ps(raster.e1RejectLayerCCCC[0], e1bbbbCmpGtQuadOffset);	// C + A * quadSize + B * quadSize

		// Accept
		raster.e1AcceptLayerOffset[0] = _mm_sub_ps(ZERO, e1aaaaCmpGtQuadOffset);						// A > 0 ? -A * quadSize : 0
		__m128 e1aaaaCmpLt = _mm_cmplt_ps(raster.e1aaaa, ZERO);
		__m128 e1aaaaCmpLtFactor = _mm_and_ps(e1aaaaCmpLt, raster.e1aaaa);
		__m128 e1aaaaCmpLtQuadOffset = _mm_mul_ps(e1aaaaCmpLtFactor, topQuadSize);
		raster.e1AcceptLayerOffset[0] = _mm_add_ps(raster.e1AcceptLayerOffset[0], e1aaaaCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0)

		raster.e1AcceptLayerOffset[0] = _mm_sub_ps(raster.e1AcceptLayerOffset[0], e1bbbbCmpGtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) - (B > 0 ? B * quadSize : 0)
		__m128 e1bbbbCmpLt = _mm_cmplt_ps(raster.e1bbbb, ZERO);
		__m128 e1bbbbCmpLtFactor = _mm_and_ps(e1bbbbCmpLt, raster.e1bbbb);
		__m128 e1bbbbCmpLtQuadOffset = _mm_mul_ps(e1bbbbCmpLtFactor, topQuadSize);
		raster.e1AcceptLayerOffset[0] = _mm_add_ps(raster.e1AcceptLayerOffset[0], e1bbbbCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) + (B > 0 ? -B * quadSize : 0) + (B < 0 ? B * quadSize : 0)
			
		// E2
		// Reject
		__m128 e2aaaaCmpGt = _mm_cmpgt_ps(raster.e2aaaa, ZERO);
		__m128 e2aaaaCmpGtFactor = _mm_and_ps(e2aaaaCmpGt, raster.e2aaaa);
		__m128 e2aaaaCmpGtQuadOffset = _mm_mul_ps(e2aaaaCmpGtFactor, topQuadSize);		// A * quadSize
		raster.e2RejectLayerCCCC[0] = _mm_add_ps(raster.e2cccc, e2aaaaCmpGtQuadOffset);		// C + A * quadSize	

		__m128 e2bbbbCmpGt = _mm_cmpgt_ps(raster.e2bbbb, ZERO);
		__m128 e2bbbbCmpGtFactor = _mm_and_ps(e2bbbbCmpGt, raster.e2bbbb);
		__m128 e2bbbbCmpGtQuadOffset = _mm_mul_ps(e2bbbbCmpGtFactor, topQuadSize);			// B * quadSize
		raster.e2RejectLayerCCCC[0] = _mm_add_ps(raster.e2RejectLayerCCCC[0], e2bbbbCmpGtQuadOffset);	// C + A * quadSize + B * quadSize

		// Accept
		raster.e2AcceptLayerOffset[0] = _mm_sub_ps(ZERO, e2aaaaCmpGtQuadOffset);						// A > 0 ? -A * quadSize : 0
		__m128 e2aaaaCmpLt = _mm_cmplt_ps(raster.e2aaaa, ZERO);
		__m128 e2aaaaCmpLtFactor = _mm_and_ps(e2aaaaCmpLt, raster.e2aaaa);
		__m128 e2aaaaCmpLtQuadOffset = _mm_mul_ps(e2aaaaCmpLtFactor, topQuadSize);
		raster.e2AcceptLayerOffset[0] = _mm_add_ps(raster.e2AcceptLayerOffset[0], e2aaaaCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0)

		raster.e2AcceptLayerOffset[0] = _mm_sub_ps(raster.e2AcceptLayerOffset[0], e2bbbbCmpGtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) - (B > 0 ? B * quadSize : 0)
		__m128 e2bbbbCmpLt = _mm_cmplt_ps(raster.e2bbbb, ZERO);
		__m128 e2bbbbCmpLtFactor = _mm_and_ps(e2bbbbCmpLt, raster.e2bbbb);
		__m128 e2bbbbCmpLtQuadOffset = _mm_mul_ps(e2bbbbCmpLtFactor, topQuadSize);
		raster.e2AcceptLayerOffset[0] = _mm_add_ps(raster.e2AcceptLayerOffset[0], e2bbbbCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) + (B > 0 ? -B * quadSize : 0) + (B < 0 ? B * quadSize : 0)

		// E3
		// Reject
		__m128 e3aaaaCmpGt = _mm_cmpgt_ps(raster.e3aaaa, ZERO);
		__m128 e3aaaaCmpGtFactor = _mm_and_ps(e3aaaaCmpGt, raster.e3aaaa);
		__m128 e3aaaaCmpGtQuadOffset = _mm_mul_ps(e3aaaaCmpGtFactor, topQuadSize);		// A * quadSize
		raster.e3RejectLayerCCCC[0] = _mm_add_ps(raster.e3cccc, e3aaaaCmpGtQuadOffset);		// C + A * quadSize	

		__m128 e3bbbbCmpGt = _mm_cmpgt_ps(raster.e3bbbb, ZERO);
		__m128 e3bbbbCmpGtFactor = _mm_and_ps(e3bbbbCmpGt, raster.e3bbbb);
		__m128 e3bbbbCmpGtQuadOffset = _mm_mul_ps(e3bbbbCmpGtFactor, topQuadSize);			// B * quadSize
		raster.e3RejectLayerCCCC[0] = _mm_add_ps(raster.e3RejectLayerCCCC[0], e3bbbbCmpGtQuadOffset);	// C + A * quadSize + B * quadSize

		// Accept
		raster.e3AcceptLayerOffset[0] = _mm_sub_ps(ZERO, e3aaaaCmpGtQuadOffset);						// A > 0 ? -A * quadSize : 0
		__m128 e3aaaaCmpLt = _mm_cmplt_ps(raster.e3aaaa, ZERO);
		__m128 e3aaaaCmpLtFactor = _mm_and_ps(e3aaaaCmpLt, raster.e3aaaa);
		__m128 e3aaaaCmpLtQuadOffset = _mm_mul_ps(e3aaaaCmpLtFactor, topQuadSize);
		raster.e3AcceptLayerOffset[0] = _mm_add_ps(raster.e3AcceptLayerOffset[0], e3aaaaCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0)

		raster.e3AcceptLayerOffset[0] = _mm_sub_ps(raster.e3AcceptLayerOffset[0], e3bbbbCmpGtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) - (B > 0 ? B * quadSize : 0)
		__m128 e3bbbbCmpLt = _mm_cmplt_ps(raster.e3bbbb, ZERO);
		__m128 e3bbbbCmpLtFactor = _mm_and_ps(e3bbbbCmpLt, raster.e3bbbb);
		__m128 e3bbbbCmpLtQuadOffset = _mm_mul_ps(e3bbbbCmpLtFactor, topQuadSize);
		raster.e3AcceptLayerOffset[0] = _mm_add_ps(raster.e3AcceptLayerOffset[0], e3bbbbCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) + (B > 0 ? -B * quadSize : 0) + (B < 0 ? B * quadSize : 0)

		// Mid tile
		const static _ALIGN(16) float MID_TILE_OFFSET[] = { midTileSize * 2.0f, midTileSize * 2.0f, midTileSize * 2.0f, midTileSize * 2.0f };

		__m128 midTileAddFactor = _mm_load_ps(MID_TILE_OFFSET);
		raster.e1XLayerAdd[1] = _mm_mul_ps(raster.e1aaaa, midTileAddFactor);
		raster.e2XLayerAdd[1] = _mm_mul_ps(raster.e2aaaa, midTileAddFactor);
		raster.e3XLayerAdd[1] = _mm_mul_ps(raster.e3aaaa, midTileAddFactor);

		raster.e1YLayerAdd[1] = _mm_mul_ps(raster.e1bbbb, midTileAddFactor);
		raster.e2YLayerAdd[1] = _mm_mul_ps(raster.e2bbbb, midTileAddFactor);
		raster.e3YLayerAdd[1] = _mm_mul_ps(raster.e3bbbb, midTileAddFactor);

		// E1
		// Reject
		const static _ALIGN(16) float MID_QUAD_SIZE[] = { (float)(midTileSize-1), (float)(midTileSize-1), (float)(midTileSize-1), (float)(midTileSize-1) };

		e1aaaaCmpGt = _mm_cmpgt_ps(raster.e1aaaa, ZERO);
		e1aaaaCmpGtFactor = _mm_and_ps(e1aaaaCmpGt, raster.e1aaaa);
		__m128 midQuadSize = _mm_load_ps(MID_QUAD_SIZE);
		e1aaaaCmpGtQuadOffset = _mm_mul_ps(e1aaaaCmpGtFactor, midQuadSize);				// A * quadSize
		raster.e1RejectLayerCCCC[1] = _mm_add_ps(raster.e1cccc, e1aaaaCmpGtQuadOffset);		// C + A * quadSize	

		e1bbbbCmpGt = _mm_cmpgt_ps(raster.e1bbbb, ZERO);
		e1bbbbCmpGtFactor = _mm_and_ps(e1bbbbCmpGt, raster.e1bbbb);
		e1bbbbCmpGtQuadOffset = _mm_mul_ps(e1bbbbCmpGtFactor, midQuadSize);					// B * quadSize
		raster.e1RejectLayerCCCC[1] = _mm_add_ps(raster.e1RejectLayerCCCC[1], e1bbbbCmpGtQuadOffset);	// C + A * quadSize + B * quadSize

		// Accept
		raster.e1AcceptLayerOffset[1] = _mm_sub_ps(ZERO, e1aaaaCmpGtQuadOffset);						// A > 0 ? -A * quadSize : 0
		e1aaaaCmpLt = _mm_cmplt_ps(raster.e1aaaa, ZERO);
		e1aaaaCmpLtFactor = _mm_and_ps(e1aaaaCmpLt, raster.e1aaaa);
		e1aaaaCmpLtQuadOffset = _mm_mul_ps(e1aaaaCmpLtFactor, midQuadSize);
		raster.e1AcceptLayerOffset[1] = _mm_add_ps(raster.e1AcceptLayerOffset[1], e1aaaaCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0)

		raster.e1AcceptLayerOffset[1] = _mm_sub_ps(raster.e1AcceptLayerOffset[1], e1bbbbCmpGtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) - (B > 0 ? B * quadSize : 0)
		e1bbbbCmpLt = _mm_cmplt_ps(raster.e1bbbb, ZERO);
		e1bbbbCmpLtFactor = _mm_and_ps(e1bbbbCmpLt, raster.e1bbbb);
		e1bbbbCmpLtQuadOffset = _mm_mul_ps(e1bbbbCmpLtFactor, midQuadSize);
		raster.e1AcceptLayerOffset[1] = _mm_add_ps(raster.e1AcceptLayerOffset[1], e1bbbbCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) + (B > 0 ? -B * quadSize : 0) + (B < 0 ? B * quadSize : 0)

		// E2
		// Reject
		e2aaaaCmpGt = _mm_cmpgt_ps(raster.e2aaaa, ZERO);
		e2aaaaCmpGtFactor = _mm_and_ps(e2aaaaCmpGt, raster.e2aaaa);
		e2aaaaCmpGtQuadOffset = _mm_mul_ps(e2aaaaCmpGtFactor, midQuadSize);		// A * quadSize
		raster.e2RejectLayerCCCC[1] = _mm_add_ps(raster.e2cccc, e2aaaaCmpGtQuadOffset);		// C + A * quadSize	

		e2bbbbCmpGt = _mm_cmpgt_ps(raster.e2bbbb, ZERO);
		e2bbbbCmpGtFactor = _mm_and_ps(e2bbbbCmpGt, raster.e2bbbb);
		e2bbbbCmpGtQuadOffset = _mm_mul_ps(e2bbbbCmpGtFactor, midQuadSize);			// B * quadSize
		raster.e2RejectLayerCCCC[1] = _mm_add_ps(raster.e2RejectLayerCCCC[1], e2bbbbCmpGtQuadOffset);	// C + A * quadSize + B * quadSize

		// Accept
		raster.e2AcceptLayerOffset[1] = _mm_sub_ps(ZERO, e2aaaaCmpGtQuadOffset);						// A > 0 ? -A * quadSize : 0
		e2aaaaCmpLt = _mm_cmplt_ps(raster.e2aaaa, ZERO);
		e2aaaaCmpLtFactor = _mm_and_ps(e2aaaaCmpLt, raster.e2aaaa);
		e2aaaaCmpLtQuadOffset = _mm_mul_ps(e2aaaaCmpLtFactor, midQuadSize);
		raster.e2AcceptLayerOffset[1] = _mm_add_ps(raster.e2AcceptLayerOffset[1], e2aaaaCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0)

		raster.e2AcceptLayerOffset[1] = _mm_sub_ps(raster.e2AcceptLayerOffset[1], e2bbbbCmpGtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) - (B > 0 ? B * quadSize : 0)
		e2bbbbCmpLt = _mm_cmplt_ps(raster.e2bbbb, ZERO);
		e2bbbbCmpLtFactor = _mm_and_ps(e2bbbbCmpLt, raster.e2bbbb);
		e2bbbbCmpLtQuadOffset = _mm_mul_ps(e2bbbbCmpLtFactor, midQuadSize);
		raster.e2AcceptLayerOffset[1] = _mm_add_ps(raster.e2AcceptLayerOffset[1], e2bbbbCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) + (B > 0 ? -B * quadSize : 0) + (B < 0 ? B * quadSize : 0)

		// E3
		// Reject
		e3aaaaCmpGt = _mm_cmpgt_ps(raster.e3aaaa, ZERO);
		e3aaaaCmpGtFactor = _mm_and_ps(e3aaaaCmpGt, raster.e3aaaa);
		e3aaaaCmpGtQuadOffset = _mm_mul_ps(e3aaaaCmpGtFactor, midQuadSize);		// A * quadSize
		raster.e3RejectLayerCCCC[1] = _mm_add_ps(raster.e3cccc, e3aaaaCmpGtQuadOffset);		// C + A * quadSize	

		e3bbbbCmpGt = _mm_cmpgt_ps(raster.e3bbbb, ZERO);
		e3bbbbCmpGtFactor = _mm_and_ps(e3bbbbCmpGt, raster.e3bbbb);
		e3bbbbCmpGtQuadOffset = _mm_mul_ps(e3bbbbCmpGtFactor, midQuadSize);			// B * quadSize
		raster.e3RejectLayerCCCC[1] = _mm_add_ps(raster.e3RejectLayerCCCC[1], e3bbbbCmpGtQuadOffset);	// C + A * quadSize + B * quadSize

		// Accept
		raster.e3AcceptLayerOffset[1] = _mm_sub_ps(ZERO, e3aaaaCmpGtQuadOffset);						// A > 0 ? -A * quadSize : 0
		e3aaaaCmpLt = _mm_cmplt_ps(raster.e3aaaa, ZERO);
		e3aaaaCmpLtFactor = _mm_and_ps(e3aaaaCmpLt, raster.e3aaaa);
		e3aaaaCmpLtQuadOffset = _mm_mul_ps(e3aaaaCmpLtFactor, midQuadSize);
		raster.e3AcceptLayerOffset[1] = _mm_add_ps(raster.e3AcceptLayerOffset[1], e3aaaaCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0)

		raster.e3AcceptLayerOffset[1] = _mm_sub_ps(raster.e3AcceptLayerOffset[1], e3bbbbCmpGtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) - (B > 0 ? B * quadSize : 0)
		e3bbbbCmpLt = _mm_cmplt_ps(raster.e3bbbb, ZERO);
		e3bbbbCmpLtFactor = _mm_and_ps(e3bbbbCmpLt, raster.e3bbbb);
		e3bbbbCmpLtQuadOffset = _mm_mul_ps(e3bbbbCmpLtFactor, midQuadSize);
		raster.e3AcceptLayerOffset[1] = _mm_add_ps(raster.e3AcceptLayerOffset[1], e3bbbbCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) + (B > 0 ? -B * quadSize : 0) + (B < 0 ? B * quadSize : 0)

		// Final tile
		const static _ALIGN(16) float FINAL_TILE_SIZE[] = { 2.0f, 2.0f, 2.0f, 2.0f };

		__m128 finalTileAddFactor = _mm_load_ps(FINAL_TILE_SIZE);
		raster.e1XFinalAdd = _mm_mul_ps(raster.e1aaaa, finalTileAddFactor);
		raster.e2XFinalAdd = _mm_mul_ps(raster.e2aaaa, finalTileAddFactor);
		raster.e3XFinalAdd = _mm_mul_ps(raster.e3aaaa, finalTileAddFactor);

		raster.e1YFinalAdd = _mm_mul_ps(raster.e1bbbb, finalTileAddFactor);
		raster.e2YFinalAdd = _mm_mul_ps(raster.e2bbbb, finalTileAddFactor);
		raster.e3YFinalAdd = _mm_mul_ps(raster.e3bbbb, finalTileAddFactor);

		//// Store determinant in edge1.w
		//_mm_store_ss(((float*) &(pOut->edge1)) + 3, determinant);

		// Use Newton-Rhapson for more accurate rcp approximation
		//__m128 rcpDeterminant = _mm_div_ss(ONE, determinant);
		__m128 rcpDeterminant = _mm_rcp_ss(determinant);
		rcpDeterminant = _mm_mul_ss(rcpDeterminant, _mm_sub_ss(finalTileAddFactor, _mm_mul_ss(rcpDeterminant, determinant))); // We use finalTileAddFactor for the 2.0f...

		rcpDeterminant = _mm_shuffle_ps(rcpDeterminant, rcpDeterminant, _MM_SHUFFLE(0, 0, 0, 0));

		// Finally, this is the inv 3x3 matrix
		__m128 invMatRow1 = _mm_mul_ps(rcpDeterminant, _3322s3223_3213s3312_2312s2213_2312s2213);
		__m128 invMatRow2 = _mm_mul_ps(rcpDeterminant, _3123s3321_3311s3113_2113s2311_2113s2311);
		__m128 invMatRow3 = _mm_mul_ps(rcpDeterminant, _3221s3122_3112s3211_2211s2112_2211s2112);

		//// TEST CODE
		//gmtl::VecA3f testInvRow1;
		//gmtl::VecA3f testInvRow2;
		//gmtl::VecA3f testInvRow3;
		//_mm_store_ps((float*) &testInvRow1, invMatRow1);
		//_mm_store_ps((float*) &testInvRow2, invMatRow2);
		//_mm_store_ps((float*) &testInvRow3, invMatRow3);
		//
		//gmtl::Matrix33f testInvMat;
		//testInvMat.set( testInvRow1[0], testInvRow1[1], testInvRow1[2],
		//				testInvRow2[0], testInvRow2[1], testInvRow2[2],
		//				testInvRow3[0], testInvRow3[1], testInvRow3[2] );
		//gmtl::Matrix33f origMat;
		//origMat.set( v1[0], v2[0], v3[0],
		//			 v1[1], v2[1], v3[1],
		//			 v1[3], v2[3], v3[3] );
		//gmtl::Matrix33f invertOrig;
		//gmtl::invertFull(invertOrig, origMat);

		//gmtl::Matrix33f newMat = testInvMat * origMat;
		//gmtl::Matrix33f newMa2t = origMat * invertOrig;

		// Find invW interpolants
		__m128 invWInterpolant = _mm_add_ps(invMatRow1, invMatRow2);
		invWInterpolant = _mm_add_ps(invWInterpolant, invMatRow3);

		_B = _mm_shuffle_ps(invWInterpolant, invWInterpolant, _MM_SHUFFLE(1, 1, 1, 1));
		_C = _mm_movehl_ps(invWInterpolant, invWInterpolant);
		_CaBsA = _mm_add_ss(_C, _B);
		_CaBsA = _mm_sub_ss(_CaBsA, invWInterpolant);
		__m128 invWInterpolantAB_CaBsA = _mm_movelh_ps(invWInterpolant, _CaBsA);
		invWInterpolantAB_CaBsA = _mm_mul_ps(invWInterpolantAB_CaBsA, SCREEN_BUFFER_SCALE);
		
		raster.invWAAAA = _mm_shuffle_ps(invWInterpolantAB_CaBsA, invWInterpolantAB_CaBsA, _MM_SHUFFLE(0, 0, 0, 0));
		raster.invWBBBB = _mm_shuffle_ps(invWInterpolantAB_CaBsA, invWInterpolantAB_CaBsA, _MM_SHUFFLE(1, 1, 1, 1));
		raster.invWCCCC = _mm_shuffle_ps(invWInterpolantAB_CaBsA, invWInterpolantAB_CaBsA, _MM_SHUFFLE(2, 2, 2, 2));
		raster.invWXFinalAdd = _mm_mul_ps(raster.invWAAAA, finalTileAddFactor);
		raster.invWYFinalAdd = _mm_mul_ps(raster.invWBBBB, finalTileAddFactor);

		//_mm_store_ps( ((float*) &(pOut->invWInterpolant)), invWInterpolant );

		//// Scale edge fns to [0, bufferWidth] and [0, bufferHeight]
		//pOut->invWInterpolant[2] = pOut->invWInterpolant[2] + pOut->invWInterpolant[1] - pOut->invWInterpolant[0];
		//pOut->invWInterpolant[0] = pOut->invWInterpolant[0] * SCREEN_BUFFER_SCALEX;
		//pOut->invWInterpolant[1] = pOut->invWInterpolant[1] * SCREEN_BUFFER_SCALEY;

		// Compute depth
		__m128 v1Depth = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 v2Depth = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 v3Depth = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(2, 2, 2, 2));

		v1Depth = _mm_mul_ps(v1Depth, invMatRow1);
		v2Depth = _mm_mul_ps(v2Depth, invMatRow2);
		v3Depth = _mm_mul_ps(v3Depth, invMatRow3);

		__m128 depthInterpolant = _mm_add_ps(v1Depth, v2Depth);
		depthInterpolant = _mm_add_ps(depthInterpolant, v3Depth);
		_B = _mm_shuffle_ps(depthInterpolant, depthInterpolant, _MM_SHUFFLE(1, 1, 1, 1));
		_C = _mm_movehl_ps(depthInterpolant, depthInterpolant);
		_CaBsA = _mm_add_ss(_C, _B);
		_CaBsA = _mm_sub_ss(_CaBsA, depthInterpolant);
		__m128 depthInterpolantAB_CaBsA = _mm_movelh_ps(depthInterpolant, _CaBsA);
		depthInterpolantAB_CaBsA = _mm_mul_ps(depthInterpolantAB_CaBsA, SCREEN_BUFFER_SCALE);

		raster.depthAAAA = _mm_shuffle_ps(depthInterpolantAB_CaBsA, depthInterpolantAB_CaBsA, _MM_SHUFFLE(0, 0, 0, 0));
		raster.depthBBBB = _mm_shuffle_ps(depthInterpolantAB_CaBsA, depthInterpolantAB_CaBsA, _MM_SHUFFLE(1, 1, 1, 1));
		raster.depthCCCC = _mm_shuffle_ps(depthInterpolantAB_CaBsA, depthInterpolantAB_CaBsA, _MM_SHUFFLE(2, 2, 2, 2));
		raster.depthXFinalAdd = _mm_mul_ps(raster.depthAAAA, finalTileAddFactor);
		raster.depthYFinalAdd = _mm_mul_ps(raster.depthBBBB, finalTileAddFactor);

		// Now compute additional interpolants
		_LOOPi(numInterpolants)
		{
			__m128 v1 = _mm_load1_ps(pInterpolantsV1+i);
			__m128 v2 = _mm_load1_ps(pInterpolantsV2+i);
			__m128 v3 = _mm_load1_ps(pInterpolantsV3+i);

			v1 = _mm_mul_ps(v1, invMatRow1);
			v2 = _mm_mul_ps(v2, invMatRow2);
			v3 = _mm_mul_ps(v3, invMatRow3);

			__m128 res = _mm_add_ps(v1, v2);
			res = _mm_add_ps(res, v3);
			_B = _mm_shuffle_ps(res, res, _MM_SHUFFLE(1, 1, 1, 1));
			_C = _mm_movehl_ps(res, res);
			_CaBsA = _mm_add_ss(_C, _B);
			_CaBsA = _mm_sub_ss(_CaBsA, res);
			__m128 resAB_CaBsA = _mm_movelh_ps(res, _CaBsA);
			resAB_CaBsA = _mm_mul_ps(resAB_CaBsA, SCREEN_BUFFER_SCALE);

			raster.edgeInterpolantsAAAA[i] = _mm_shuffle_ps(resAB_CaBsA, resAB_CaBsA, _MM_SHUFFLE(0, 0, 0, 0));
			raster.edgeInterpolantsBBBB[i] = _mm_shuffle_ps(resAB_CaBsA, resAB_CaBsA, _MM_SHUFFLE(1, 1, 1, 1));
			raster.edgeInterpolantsCCCC[i] = _mm_shuffle_ps(resAB_CaBsA, resAB_CaBsA, _MM_SHUFFLE(2, 2, 2, 2));
			raster.edgeInterpolantsXFinalAdd[i] = _mm_mul_ps(raster.edgeInterpolantsAAAA[i], finalTileAddFactor);
			raster.edgeInterpolantsYFinalAdd[i] = _mm_mul_ps(raster.edgeInterpolantsBBBB[i], finalTileAddFactor);

			//_mm_store_ps( ((float*) &(pOut->edgeInterpolants[i])), res );

			// Scale edge fns to [0, bufferWidth] and [0, bufferHeight]
			//pOut->edgeInterpolants[i][2] = pOut->edgeInterpolants[i][2] + pOut->edgeInterpolants[i][1] - pOut->edgeInterpolants[i][0];
			//pOut->edgeInterpolants[i][0] = pOut->edgeInterpolants[i][0] * SCREEN_BUFFER_SCALEX;
			//pOut->edgeInterpolants[i][1] = pOut->edgeInterpolants[i][1] * SCREEN_BUFFER_SCALEY;
		}

		return TRUE;
	}

	return FALSE;
}



template <uint numInterpolants, uint topTileSize>
boolean TriangleSetup1Layer(const gmtl::VecA4f& v1, const gmtl::VecA4f& v2, const gmtl::VecA4f& v3,
	const float* pInterpolantsV1, const float* pInterpolantsV2, const float* pInterpolantsV3, 
	uint bufferWidth, uint bufferHeight, void* pRasterInfo)
{
	RasterInfo<numInterpolants, 1>& raster = *((RasterInfo<numInterpolants, 1>*) pRasterInfo);

	// Compute inverse of 3x3

	__m128 col1 = _mm_load_ps((float*) &v1);
	__m128 col2 = _mm_load_ps((float*) &v2);
	__m128 col3 = _mm_load_ps((float*) &v3);

	// Get the 9 matrix values
	__m128 _33_33_33_33 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(3, 3, 3, 3));

	//__m128 _11_21_12_22	= _mm_shuffle_ps(col1, col2, _MM_SHUFFLE(1, 0, 1, 0));
	//__m128 _11_12_21_22	= _mm_shuffle_ps(_11_21_12_22, _11_21_12_22, _MM_SHUFFLE(3, 1, 2, 0));

	__m128 _11_12_21_22	= _mm_unpacklo_ps(col1, col2);

	__m128 _31_31_32_32 = _mm_shuffle_ps(col1, col2, _MM_SHUFFLE(3, 3, 3, 3));
	__m128 _32_31_32_31 = _mm_shuffle_ps(_31_31_32_32, _31_31_32_32, _MM_SHUFFLE(0, 3, 0, 3));

	__m128 _23_23_13_13 = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(0, 0, 1, 1));

	__m128 _22_21_12_11 = _mm_shuffle_ps(_11_12_21_22, _11_12_21_22, _MM_SHUFFLE(0, 1, 2, 3));

	__m128 _3311_3312_3321_3322 = _mm_mul_ps(_33_33_33_33, _11_12_21_22);
	__m128 _3223_3123_3213_3113 = _mm_mul_ps(_32_31_32_31, _23_23_13_13);
	__m128 _3211_3112_3221_3122 = _mm_mul_ps(_32_31_32_31, _11_12_21_22);
	__m128 _2311_2312_2113_2213 = _mm_mul_ps(_23_23_13_13, _11_12_21_22);
	__m128 _2211_2112_2112_2211 = _mm_mul_ps(_22_21_12_11, _11_12_21_22);

	__m128 _3221_3112_2312_2113 = _mm_shuffle_ps(_3211_3112_3221_3122, _2311_2312_2113_2213, _MM_SHUFFLE(2, 1, 1, 2));
	__m128 _3122_3211_2213_2311 = _mm_shuffle_ps(_3211_3112_3221_3122, _2311_2312_2113_2213, _MM_SHUFFLE(0, 3, 0, 3));
	__m128 _3221s3122_3112s3211_2312s2213_2113s2311 = _mm_sub_ps(_3221_3112_2312_2113, _3122_3211_2213_2311);

	__m128 _3322_3311_3123_3213 = _mm_shuffle_ps(_3311_3312_3321_3322, _3223_3123_3213_3113, _MM_SHUFFLE(2, 1, 0, 3));
	__m128 _3223_3113_3321_3312 = _mm_shuffle_ps(_3223_3123_3213_3113, _3311_3312_3321_3322, _MM_SHUFFLE(1, 2, 3, 0));
	__m128 _3322s3223_3311s3113_3123s3321_3213s3312 = _mm_sub_ps(_3322_3311_3123_3213, _3223_3113_3321_3312);

	__m128 _2112_2112_2211_2211 = _mm_unpackhi_ps(_2211_2112_2112_2211, _2211_2112_2112_2211);
	__m128 _2211s2112 = _mm_sub_ss(_2211_2112_2112_2211, _2112_2112_2211_2211);

	// Shuffle to rows
	// This is the 3x3 adjoint matrix
	__m128 _3322s3223_3213s3312_2312s2213_2312s2213 = _mm_shuffle_ps(_3322s3223_3311s3113_3123s3321_3213s3312, _3221s3122_3112s3211_2312s2213_2113s2311, _MM_SHUFFLE(2, 2, 3, 0));
	__m128 _3123s3321_3311s3113_2113s2311_2113s2311 = _mm_shuffle_ps(_3322s3223_3311s3113_3123s3321_3213s3312, _3221s3122_3112s3211_2312s2213_2113s2311, _MM_SHUFFLE(3, 3, 1, 2));
	__m128 _3221s3122_3112s3211_2211s2112_2211s2112 = _mm_shuffle_ps(_3221s3122_3112s3211_2312s2213_2113s2311, _2211s2112, _MM_SHUFFLE(0, 0, 1, 0));

	// Compute determinant
	__m128 determinant		= _mm_mul_ps(col1, _3322s3223_3213s3312_2312s2213_2312s2213);
	__m128 col1y_3213s3312	= _mm_shuffle_ps(determinant, determinant, _MM_SHUFFLE(1, 1, 1, 1));
	__m128 col1w_2312s2213	= _mm_shuffle_ps(determinant, determinant, _MM_SHUFFLE(3, 3, 3, 3));

	determinant = _mm_add_ss(determinant, col1y_3213s3312);
	determinant = _mm_add_ss(determinant, col1w_2312s2213);

	// Only accept front facing triangles for now
	__m128 detRes = _mm_cmpgt_ss(determinant, _mm_setzero_ps());
	int detMask = _mm_movemask_ps(detRes);
	//float det;
	//_mm_store_ss(&det, determinant);
	if(detMask > 0)
	{
		// Compute the edge functions
		// Make parameters relative to viewport space
		const __m128 SCREEN_BUFFER_SCALE = _mm_set_ps(1.0f, 1.0f, -2.0f / bufferHeight, 2.0f / bufferWidth);

		// Row 1
		__m128 _B = _mm_shuffle_ps(_3322s3223_3213s3312_2312s2213_2312s2213, _3322s3223_3213s3312_2312s2213_2312s2213, _MM_SHUFFLE(1, 1, 1, 1));
		__m128 _C = _mm_movehl_ps(_3322s3223_3213s3312_2312s2213_2312s2213, _3322s3223_3213s3312_2312s2213_2312s2213);
		__m128 _CaBsA = _mm_add_ss(_C, _B);
		_CaBsA = _mm_sub_ss(_CaBsA, _3322s3223_3213s3312_2312s2213_2312s2213);
		__m128 _3322s3223_3213s3312_CaBsA_3213s3312 = _mm_movelh_ps(_3322s3223_3213s3312_2312s2213_2312s2213, _CaBsA);
		__m128 row1_ABC = _mm_mul_ps(_3322s3223_3213s3312_CaBsA_3213s3312, SCREEN_BUFFER_SCALE);

		// Row 2
		_B = _mm_shuffle_ps(_3123s3321_3311s3113_2113s2311_2113s2311, _3123s3321_3311s3113_2113s2311_2113s2311, _MM_SHUFFLE(1, 1, 1, 1));
		_C = _mm_movehl_ps(_3123s3321_3311s3113_2113s2311_2113s2311, _3123s3321_3311s3113_2113s2311_2113s2311);
		_CaBsA = _mm_add_ss(_C, _B);
		_CaBsA = _mm_sub_ss(_CaBsA, _3123s3321_3311s3113_2113s2311_2113s2311);
		__m128 _3123s3321_3311s3113_CaBsA_3311s3113 = _mm_movelh_ps(_3123s3321_3311s3113_2113s2311_2113s2311, _CaBsA);
		__m128 row2_ABC = _mm_mul_ps(_3123s3321_3311s3113_CaBsA_3311s3113, SCREEN_BUFFER_SCALE);

		// Row 3
		_B = _mm_shuffle_ps(_3221s3122_3112s3211_2211s2112_2211s2112, _3221s3122_3112s3211_2211s2112_2211s2112, _MM_SHUFFLE(1, 1, 1, 1));
		_C = _mm_movehl_ps(_3221s3122_3112s3211_2211s2112_2211s2112, _3221s3122_3112s3211_2211s2112_2211s2112);
		_CaBsA = _mm_add_ss(_C, _B);
		_CaBsA = _mm_sub_ss(_CaBsA, _3221s3122_3112s3211_2211s2112_2211s2112);
		__m128 _3221s3122_3112s3211_CaBsA_3112s3211 = _mm_movelh_ps(_3221s3122_3112s3211_2211s2112_2211s2112, _CaBsA);
		__m128 row3_ABC = _mm_mul_ps(_3221s3122_3112s3211_CaBsA_3112s3211, SCREEN_BUFFER_SCALE);

		// Normalize to A and B
		// Since we only care about the half space aspect of the edge functions,
		// we can normalize with respect to screen/pixel space to reduce floating point
		// precision issues with very large triangles or very thin ones
		__m128 _B2 = _mm_shuffle_ps(row1_ABC, row1_ABC, _MM_SHUFFLE(1, 1, 1, 1));
		_B2 = _mm_mul_ss(_B2, _B2);
		__m128 _A2 = _mm_mul_ss(row1_ABC, row1_ABC);
		__m128 rcpSqrtAB = _mm_add_ss(_B2, _A2);
		rcpSqrtAB = _mm_rsqrt_ss(rcpSqrtAB);
		rcpSqrtAB = _mm_shuffle_ps(rcpSqrtAB, rcpSqrtAB, _MM_SHUFFLE(0, 0, 0, 0));
		row1_ABC = _mm_mul_ps(row1_ABC, rcpSqrtAB);

		_B2 = _mm_shuffle_ps(row2_ABC, row2_ABC, _MM_SHUFFLE(1, 1, 1, 1));
		_B2 = _mm_mul_ss(_B2, _B2);
		_A2 = _mm_mul_ss(row2_ABC, row2_ABC);
		rcpSqrtAB = _mm_add_ss(_B2, _A2);
		rcpSqrtAB = _mm_rsqrt_ss(rcpSqrtAB);
		rcpSqrtAB = _mm_shuffle_ps(rcpSqrtAB, rcpSqrtAB, _MM_SHUFFLE(0, 0, 0, 0));
		row2_ABC = _mm_mul_ps(row2_ABC, rcpSqrtAB);

		_B2 = _mm_shuffle_ps(row3_ABC, row3_ABC, _MM_SHUFFLE(1, 1, 1, 1));
		_B2 = _mm_mul_ss(_B2, _B2);
		_A2 = _mm_mul_ss(row3_ABC, row3_ABC);
		rcpSqrtAB = _mm_add_ss(_B2, _A2);
		rcpSqrtAB = _mm_rsqrt_ss(rcpSqrtAB);
		rcpSqrtAB = _mm_shuffle_ps(rcpSqrtAB, rcpSqrtAB, _MM_SHUFFLE(0, 0, 0, 0));
		row3_ABC = _mm_mul_ps(row3_ABC, rcpSqrtAB);

		const static __m128 NEG = _mm_set_ps1(-1.0f);
		const static __m128 ZERO = _mm_set_ps1(0.0f);
		static const __m128 ONE = _mm_set_ps1(1.0f);

		// Edge 1 -> (0, 0, 1) -> row 3
		// Edge 2 -> (1, 0, 0) -> row 1
		// Edge 3 -> (0, 1, 0) -> row 2
		__m128 edge1 = row3_ABC;
		__m128 edge2 = row1_ABC;
		__m128 edge3 = row2_ABC;

		raster.e1aaaa = _mm_shuffle_ps(edge1, edge1, _MM_SHUFFLE(0, 0, 0, 0));
		raster.e1bbbb = _mm_shuffle_ps(edge1, edge1, _MM_SHUFFLE(1, 1, 1, 1));
		raster.e1cccc = _mm_shuffle_ps(edge1, edge1, _MM_SHUFFLE(2, 2, 2, 2));

		raster.e2aaaa = _mm_shuffle_ps(edge2, edge2, _MM_SHUFFLE(0, 0, 0, 0));
		raster.e2bbbb = _mm_shuffle_ps(edge2, edge2, _MM_SHUFFLE(1, 1, 1, 1));
		raster.e2cccc = _mm_shuffle_ps(edge2, edge2, _MM_SHUFFLE(2, 2, 2, 2));

		raster.e3aaaa = _mm_shuffle_ps(edge3, edge3, _MM_SHUFFLE(0, 0, 0, 0));
		raster.e3bbbb = _mm_shuffle_ps(edge3, edge3, _MM_SHUFFLE(1, 1, 1, 1));
		raster.e3cccc = _mm_shuffle_ps(edge3, edge3, _MM_SHUFFLE(2, 2, 2, 2));	

		// Top tile
		const static _ALIGN(16) float TOP_TILE_OFFSET[] = { topTileSize * 2.0f, topTileSize * 2.0f, topTileSize * 2.0f, topTileSize * 2.0f };

		__m128 topTileAddFactor = _mm_load_ps(TOP_TILE_OFFSET);
		raster.e1XLayerAdd[0] = _mm_mul_ps(raster.e1aaaa, topTileAddFactor);
		raster.e2XLayerAdd[0] = _mm_mul_ps(raster.e2aaaa, topTileAddFactor);
		raster.e3XLayerAdd[0] = _mm_mul_ps(raster.e3aaaa, topTileAddFactor);

		raster.e1YLayerAdd[0] = _mm_mul_ps(raster.e1bbbb, topTileAddFactor);
		raster.e2YLayerAdd[0] = _mm_mul_ps(raster.e2bbbb, topTileAddFactor);
		raster.e3YLayerAdd[0] = _mm_mul_ps(raster.e3bbbb, topTileAddFactor);

		// E1
		// Reject
		const static _ALIGN(16) float TOP_QUAD_SIZE[] = { (float)(topTileSize-1), (float)(topTileSize-1), (float)(topTileSize-1), (float)(topTileSize-1) };

		__m128 e1aaaaCmpGt = _mm_cmpgt_ps(raster.e1aaaa, ZERO);
		__m128 e1aaaaCmpGtFactor = _mm_and_ps(e1aaaaCmpGt, raster.e1aaaa);
		__m128 topQuadSize = _mm_load_ps(TOP_QUAD_SIZE);
		__m128 e1aaaaCmpGtQuadOffset = _mm_mul_ps(e1aaaaCmpGtFactor, topQuadSize);		// A * quadSize
		raster.e1RejectLayerCCCC[0] = _mm_add_ps(raster.e1cccc, e1aaaaCmpGtQuadOffset);		// C + A * quadSize	

		__m128 e1bbbbCmpGt = _mm_cmpgt_ps(raster.e1bbbb, ZERO);
		__m128 e1bbbbCmpGtFactor = _mm_and_ps(e1bbbbCmpGt, raster.e1bbbb);
		__m128 e1bbbbCmpGtQuadOffset = _mm_mul_ps(e1bbbbCmpGtFactor, topQuadSize);			// B * quadSize
		raster.e1RejectLayerCCCC[0] = _mm_add_ps(raster.e1RejectLayerCCCC[0], e1bbbbCmpGtQuadOffset);	// C + A * quadSize + B * quadSize

		// Accept
		raster.e1AcceptLayerOffset[0] = _mm_sub_ps(ZERO, e1aaaaCmpGtQuadOffset);						// A > 0 ? -A * quadSize : 0
		__m128 e1aaaaCmpLt = _mm_cmplt_ps(raster.e1aaaa, ZERO);
		__m128 e1aaaaCmpLtFactor = _mm_and_ps(e1aaaaCmpLt, raster.e1aaaa);
		__m128 e1aaaaCmpLtQuadOffset = _mm_mul_ps(e1aaaaCmpLtFactor, topQuadSize);
		raster.e1AcceptLayerOffset[0] = _mm_add_ps(raster.e1AcceptLayerOffset[0], e1aaaaCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0)

		raster.e1AcceptLayerOffset[0] = _mm_sub_ps(raster.e1AcceptLayerOffset[0], e1bbbbCmpGtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) - (B > 0 ? B * quadSize : 0)
		__m128 e1bbbbCmpLt = _mm_cmplt_ps(raster.e1bbbb, ZERO);
		__m128 e1bbbbCmpLtFactor = _mm_and_ps(e1bbbbCmpLt, raster.e1bbbb);
		__m128 e1bbbbCmpLtQuadOffset = _mm_mul_ps(e1bbbbCmpLtFactor, topQuadSize);
		raster.e1AcceptLayerOffset[0] = _mm_add_ps(raster.e1AcceptLayerOffset[0], e1bbbbCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) + (B > 0 ? -B * quadSize : 0) + (B < 0 ? B * quadSize : 0)

		// E2
		// Reject
		__m128 e2aaaaCmpGt = _mm_cmpgt_ps(raster.e2aaaa, ZERO);
		__m128 e2aaaaCmpGtFactor = _mm_and_ps(e2aaaaCmpGt, raster.e2aaaa);
		__m128 e2aaaaCmpGtQuadOffset = _mm_mul_ps(e2aaaaCmpGtFactor, topQuadSize);		// A * quadSize
		raster.e2RejectLayerCCCC[0] = _mm_add_ps(raster.e2cccc, e2aaaaCmpGtQuadOffset);		// C + A * quadSize	

		__m128 e2bbbbCmpGt = _mm_cmpgt_ps(raster.e2bbbb, ZERO);
		__m128 e2bbbbCmpGtFactor = _mm_and_ps(e2bbbbCmpGt, raster.e2bbbb);
		__m128 e2bbbbCmpGtQuadOffset = _mm_mul_ps(e2bbbbCmpGtFactor, topQuadSize);			// B * quadSize
		raster.e2RejectLayerCCCC[0] = _mm_add_ps(raster.e2RejectLayerCCCC[0], e2bbbbCmpGtQuadOffset);	// C + A * quadSize + B * quadSize

		// Accept
		raster.e2AcceptLayerOffset[0] = _mm_sub_ps(ZERO, e2aaaaCmpGtQuadOffset);						// A > 0 ? -A * quadSize : 0
		__m128 e2aaaaCmpLt = _mm_cmplt_ps(raster.e2aaaa, ZERO);
		__m128 e2aaaaCmpLtFactor = _mm_and_ps(e2aaaaCmpLt, raster.e2aaaa);
		__m128 e2aaaaCmpLtQuadOffset = _mm_mul_ps(e2aaaaCmpLtFactor, topQuadSize);
		raster.e2AcceptLayerOffset[0] = _mm_add_ps(raster.e2AcceptLayerOffset[0], e2aaaaCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0)

		raster.e2AcceptLayerOffset[0] = _mm_sub_ps(raster.e2AcceptLayerOffset[0], e2bbbbCmpGtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) - (B > 0 ? B * quadSize : 0)
		__m128 e2bbbbCmpLt = _mm_cmplt_ps(raster.e2bbbb, ZERO);
		__m128 e2bbbbCmpLtFactor = _mm_and_ps(e2bbbbCmpLt, raster.e2bbbb);
		__m128 e2bbbbCmpLtQuadOffset = _mm_mul_ps(e2bbbbCmpLtFactor, topQuadSize);
		raster.e2AcceptLayerOffset[0] = _mm_add_ps(raster.e2AcceptLayerOffset[0], e2bbbbCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) + (B > 0 ? -B * quadSize : 0) + (B < 0 ? B * quadSize : 0)

		// E3
		// Reject
		__m128 e3aaaaCmpGt = _mm_cmpgt_ps(raster.e3aaaa, ZERO);
		__m128 e3aaaaCmpGtFactor = _mm_and_ps(e3aaaaCmpGt, raster.e3aaaa);
		__m128 e3aaaaCmpGtQuadOffset = _mm_mul_ps(e3aaaaCmpGtFactor, topQuadSize);		// A * quadSize
		raster.e3RejectLayerCCCC[0] = _mm_add_ps(raster.e3cccc, e3aaaaCmpGtQuadOffset);		// C + A * quadSize	

		__m128 e3bbbbCmpGt = _mm_cmpgt_ps(raster.e3bbbb, ZERO);
		__m128 e3bbbbCmpGtFactor = _mm_and_ps(e3bbbbCmpGt, raster.e3bbbb);
		__m128 e3bbbbCmpGtQuadOffset = _mm_mul_ps(e3bbbbCmpGtFactor, topQuadSize);			// B * quadSize
		raster.e3RejectLayerCCCC[0] = _mm_add_ps(raster.e3RejectLayerCCCC[0], e3bbbbCmpGtQuadOffset);	// C + A * quadSize + B * quadSize

		// Accept
		raster.e3AcceptLayerOffset[0] = _mm_sub_ps(ZERO, e3aaaaCmpGtQuadOffset);						// A > 0 ? -A * quadSize : 0
		__m128 e3aaaaCmpLt = _mm_cmplt_ps(raster.e3aaaa, ZERO);
		__m128 e3aaaaCmpLtFactor = _mm_and_ps(e3aaaaCmpLt, raster.e3aaaa);
		__m128 e3aaaaCmpLtQuadOffset = _mm_mul_ps(e3aaaaCmpLtFactor, topQuadSize);
		raster.e3AcceptLayerOffset[0] = _mm_add_ps(raster.e3AcceptLayerOffset[0], e3aaaaCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0)

		raster.e3AcceptLayerOffset[0] = _mm_sub_ps(raster.e3AcceptLayerOffset[0], e3bbbbCmpGtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) - (B > 0 ? B * quadSize : 0)
		__m128 e3bbbbCmpLt = _mm_cmplt_ps(raster.e3bbbb, ZERO);
		__m128 e3bbbbCmpLtFactor = _mm_and_ps(e3bbbbCmpLt, raster.e3bbbb);
		__m128 e3bbbbCmpLtQuadOffset = _mm_mul_ps(e3bbbbCmpLtFactor, topQuadSize);
		raster.e3AcceptLayerOffset[0] = _mm_add_ps(raster.e3AcceptLayerOffset[0], e3bbbbCmpLtQuadOffset); // (A > 0 ? -A * quadSize : 0) + (A < 0 ? A * quadSize : 0) + (B > 0 ? -B * quadSize : 0) + (B < 0 ? B * quadSize : 0)

		// Final tile
		const static _ALIGN(16) float FINAL_TILE_SIZE[] = { 2.0f, 2.0f, 2.0f, 2.0f };

		__m128 finalTileAddFactor = _mm_load_ps(FINAL_TILE_SIZE);
		raster.e1XFinalAdd = _mm_mul_ps(raster.e1aaaa, finalTileAddFactor);
		raster.e2XFinalAdd = _mm_mul_ps(raster.e2aaaa, finalTileAddFactor);
		raster.e3XFinalAdd = _mm_mul_ps(raster.e3aaaa, finalTileAddFactor);

		raster.e1YFinalAdd = _mm_mul_ps(raster.e1bbbb, finalTileAddFactor);
		raster.e2YFinalAdd = _mm_mul_ps(raster.e2bbbb, finalTileAddFactor);
		raster.e3YFinalAdd = _mm_mul_ps(raster.e3bbbb, finalTileAddFactor);

		//// Store determinant in edge1.w
		//_mm_store_ss(((float*) &(pOut->edge1)) + 3, determinant);

		// Use Newton-Rhapson for more accurate rcp approximation
		//__m128 rcpDeterminant = _mm_div_ss(ONE, determinant);
		__m128 rcpDeterminant = _mm_rcp_ss(determinant);
		rcpDeterminant = _mm_mul_ss(rcpDeterminant, _mm_sub_ss(finalTileAddFactor, _mm_mul_ss(rcpDeterminant, determinant))); // We use finalTileAddFactor for the 2.0f...

		rcpDeterminant = _mm_shuffle_ps(rcpDeterminant, rcpDeterminant, _MM_SHUFFLE(0, 0, 0, 0));

		// Finally, this is the inv 3x3 matrix
		__m128 invMatRow1 = _mm_mul_ps(rcpDeterminant, _3322s3223_3213s3312_2312s2213_2312s2213);
		__m128 invMatRow2 = _mm_mul_ps(rcpDeterminant, _3123s3321_3311s3113_2113s2311_2113s2311);
		__m128 invMatRow3 = _mm_mul_ps(rcpDeterminant, _3221s3122_3112s3211_2211s2112_2211s2112);

		//// TEST CODE
		//gmtl::VecA3f testInvRow1;
		//gmtl::VecA3f testInvRow2;
		//gmtl::VecA3f testInvRow3;
		//_mm_store_ps((float*) &testInvRow1, invMatRow1);
		//_mm_store_ps((float*) &testInvRow2, invMatRow2);
		//_mm_store_ps((float*) &testInvRow3, invMatRow3);
		//
		//gmtl::Matrix33f testInvMat;
		//testInvMat.set( testInvRow1[0], testInvRow1[1], testInvRow1[2],
		//				testInvRow2[0], testInvRow2[1], testInvRow2[2],
		//				testInvRow3[0], testInvRow3[1], testInvRow3[2] );
		//gmtl::Matrix33f origMat;
		//origMat.set( v1[0], v2[0], v3[0],
		//			 v1[1], v2[1], v3[1],
		//			 v1[3], v2[3], v3[3] );
		//gmtl::Matrix33f invertOrig;
		//gmtl::invertFull(invertOrig, origMat);

		//gmtl::Matrix33f newMat = testInvMat * origMat;
		//gmtl::Matrix33f newMa2t = origMat * invertOrig;

		// Find invW interpolants
		__m128 invWInterpolant = _mm_add_ps(invMatRow1, invMatRow2);
		invWInterpolant = _mm_add_ps(invWInterpolant, invMatRow3);

		_B = _mm_shuffle_ps(invWInterpolant, invWInterpolant, _MM_SHUFFLE(1, 1, 1, 1));
		_C = _mm_movehl_ps(invWInterpolant, invWInterpolant);
		_CaBsA = _mm_add_ss(_C, _B);
		_CaBsA = _mm_sub_ss(_CaBsA, invWInterpolant);
		__m128 invWInterpolantAB_CaBsA = _mm_movelh_ps(invWInterpolant, _CaBsA);
		invWInterpolantAB_CaBsA = _mm_mul_ps(invWInterpolantAB_CaBsA, SCREEN_BUFFER_SCALE);

		raster.invWAAAA = _mm_shuffle_ps(invWInterpolantAB_CaBsA, invWInterpolantAB_CaBsA, _MM_SHUFFLE(0, 0, 0, 0));
		raster.invWBBBB = _mm_shuffle_ps(invWInterpolantAB_CaBsA, invWInterpolantAB_CaBsA, _MM_SHUFFLE(1, 1, 1, 1));
		raster.invWCCCC = _mm_shuffle_ps(invWInterpolantAB_CaBsA, invWInterpolantAB_CaBsA, _MM_SHUFFLE(2, 2, 2, 2));
		raster.invWXFinalAdd = _mm_mul_ps(raster.invWAAAA, finalTileAddFactor);
		raster.invWYFinalAdd = _mm_mul_ps(raster.invWBBBB, finalTileAddFactor);

		//_mm_store_ps( ((float*) &(pOut->invWInterpolant)), invWInterpolant );

		//// Scale edge fns to [0, bufferWidth] and [0, bufferHeight]
		//pOut->invWInterpolant[2] = pOut->invWInterpolant[2] + pOut->invWInterpolant[1] - pOut->invWInterpolant[0];
		//pOut->invWInterpolant[0] = pOut->invWInterpolant[0] * SCREEN_BUFFER_SCALEX;
		//pOut->invWInterpolant[1] = pOut->invWInterpolant[1] * SCREEN_BUFFER_SCALEY;

		// Compute depth
		__m128 v1Depth = _mm_shuffle_ps(col1, col1, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 v2Depth = _mm_shuffle_ps(col2, col2, _MM_SHUFFLE(2, 2, 2, 2));
		__m128 v3Depth = _mm_shuffle_ps(col3, col3, _MM_SHUFFLE(2, 2, 2, 2));

		v1Depth = _mm_mul_ps(v1Depth, invMatRow1);
		v2Depth = _mm_mul_ps(v2Depth, invMatRow2);
		v3Depth = _mm_mul_ps(v3Depth, invMatRow3);

		__m128 depthInterpolant = _mm_add_ps(v1Depth, v2Depth);
		depthInterpolant = _mm_add_ps(depthInterpolant, v3Depth);
		_B = _mm_shuffle_ps(depthInterpolant, depthInterpolant, _MM_SHUFFLE(1, 1, 1, 1));
		_C = _mm_movehl_ps(depthInterpolant, depthInterpolant);
		_CaBsA = _mm_add_ss(_C, _B);
		_CaBsA = _mm_sub_ss(_CaBsA, depthInterpolant);
		__m128 depthInterpolantAB_CaBsA = _mm_movelh_ps(depthInterpolant, _CaBsA);
		depthInterpolantAB_CaBsA = _mm_mul_ps(depthInterpolantAB_CaBsA, SCREEN_BUFFER_SCALE);

		raster.depthAAAA = _mm_shuffle_ps(depthInterpolantAB_CaBsA, depthInterpolantAB_CaBsA, _MM_SHUFFLE(0, 0, 0, 0));
		raster.depthBBBB = _mm_shuffle_ps(depthInterpolantAB_CaBsA, depthInterpolantAB_CaBsA, _MM_SHUFFLE(1, 1, 1, 1));
		raster.depthCCCC = _mm_shuffle_ps(depthInterpolantAB_CaBsA, depthInterpolantAB_CaBsA, _MM_SHUFFLE(2, 2, 2, 2));
		raster.depthXFinalAdd = _mm_mul_ps(raster.depthAAAA, finalTileAddFactor);
		raster.depthYFinalAdd = _mm_mul_ps(raster.depthBBBB, finalTileAddFactor);

		// Now compute additional interpolants
		_LOOPi(numInterpolants)
		{
			__m128 v1 = _mm_load1_ps(pInterpolantsV1+i);
			__m128 v2 = _mm_load1_ps(pInterpolantsV2+i);
			__m128 v3 = _mm_load1_ps(pInterpolantsV3+i);

			v1 = _mm_mul_ps(v1, invMatRow1);
			v2 = _mm_mul_ps(v2, invMatRow2);
			v3 = _mm_mul_ps(v3, invMatRow3);

			__m128 res = _mm_add_ps(v1, v2);
			res = _mm_add_ps(res, v3);
			_B = _mm_shuffle_ps(res, res, _MM_SHUFFLE(1, 1, 1, 1));
			_C = _mm_movehl_ps(res, res);
			_CaBsA = _mm_add_ss(_C, _B);
			_CaBsA = _mm_sub_ss(_CaBsA, res);
			__m128 resAB_CaBsA = _mm_movelh_ps(res, _CaBsA);
			resAB_CaBsA = _mm_mul_ps(resAB_CaBsA, SCREEN_BUFFER_SCALE);

			raster.edgeInterpolantsAAAA[i] = _mm_shuffle_ps(resAB_CaBsA, resAB_CaBsA, _MM_SHUFFLE(0, 0, 0, 0));
			raster.edgeInterpolantsBBBB[i] = _mm_shuffle_ps(resAB_CaBsA, resAB_CaBsA, _MM_SHUFFLE(1, 1, 1, 1));
			raster.edgeInterpolantsCCCC[i] = _mm_shuffle_ps(resAB_CaBsA, resAB_CaBsA, _MM_SHUFFLE(2, 2, 2, 2));
			raster.edgeInterpolantsXFinalAdd[i] = _mm_mul_ps(raster.edgeInterpolantsAAAA[i], finalTileAddFactor);
			raster.edgeInterpolantsYFinalAdd[i] = _mm_mul_ps(raster.edgeInterpolantsBBBB[i], finalTileAddFactor);

			//_mm_store_ps( ((float*) &(pOut->edgeInterpolants[i])), res );

			// Scale edge fns to [0, bufferWidth] and [0, bufferHeight]
			//pOut->edgeInterpolants[i][2] = pOut->edgeInterpolants[i][2] + pOut->edgeInterpolants[i][1] - pOut->edgeInterpolants[i][0];
			//pOut->edgeInterpolants[i][0] = pOut->edgeInterpolants[i][0] * SCREEN_BUFFER_SCALEX;
			//pOut->edgeInterpolants[i][1] = pOut->edgeInterpolants[i][1] * SCREEN_BUFFER_SCALEY;
		}

		return TRUE;
	}

	return FALSE;
}

_NAMESPACE_END