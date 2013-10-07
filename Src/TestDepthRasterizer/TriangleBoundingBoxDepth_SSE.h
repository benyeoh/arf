//================================================================================
//
//		TriangleBoundingBoxDepth_SSE.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/1/2012
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

template<uint topTileSize>
boolean TriangleBoundingBoxDepth1Layer(void* pRasterInfo, const gmtl::VecA4f& v1, const gmtl::VecA4f& v2, const gmtl::VecA4f& v3)
{
    RasterInfoDepth<1>* pRaster = (RasterInfoDepth<1>*) pRasterInfo;

    const float EPSILON = 0.000001f;

    // If all points are behind the eye
    if(v1[3] < EPSILON && v2[3] < EPSILON && v3[3] < EPSILON)
        return FALSE;

    // If all three points are in front of the eye
    if(v1[3] > EPSILON && v2[3] > EPSILON && v3[3] > EPSILON)
    {
        // Find bounding box

        __m128 zRange = _mm_load_ss(&v1[2]);
        __m128 zRange2 = _mm_load_ss(&v2[2]);
        __m128 zRange3 = _mm_load_ss(&v3[2]);
        zRange = _mm_unpacklo_ps(zRange, zRange2);
        zRange = _mm_movelh_ps(zRange, zRange3);

        __m128 w = _mm_load_ss(&v1[3]);
        __m128 w2 = _mm_load_ss(&v2[3]);
        __m128 w3 = _mm_load_ss(&v3[3]);
        w = _mm_unpacklo_ps(w, w2);
        w = _mm_movelh_ps(w, w3);

        __m128 overFarPlane = _mm_cmpgt_ps(zRange, w);
        int overFarPlaneRes = _mm_movemask_ps(overFarPlane);
        if( (overFarPlaneRes & 0x7) == 0x7 )
            return FALSE;

        // Basically do edge intersection tests in screenspace from this formula:
        // Ax + By = -C
        // Due to FP imprecision, the RCP can be 0 (meaning 2 edges never intersect which is obviously false)

        // Edge 1, Edge 2
        __m128 a1b2 = _mm_mul_ss(pRaster->e1aaaa, pRaster->e2bbbb);
        __m128 b1a2 = _mm_mul_ss(pRaster->e1bbbb, pRaster->e2aaaa);
        __m128 rcp_a1b2sb1a2 = _mm_rcp_ss( _mm_sub_ss(a1b2, b1a2) );

        __m128 b1c2 = _mm_mul_ss(pRaster->e1bbbb, pRaster->e2cccc);
        __m128 c1b2 = _mm_mul_ss(pRaster->e1cccc, pRaster->e2bbbb);

        __m128 x12 = _mm_mul_ss( _mm_sub_ss(b1c2, c1b2), rcp_a1b2sb1a2);

        __m128 c1a2 = _mm_mul_ss(pRaster->e1cccc, pRaster->e2aaaa);
        __m128 a1c2 = _mm_mul_ss(pRaster->e1aaaa, pRaster->e2cccc);

        __m128 y12 = _mm_mul_ss( _mm_sub_ss(c1a2, a1c2), rcp_a1b2sb1a2);

        // Edge 1, Edge 3
        __m128 a1b3 = _mm_mul_ss(pRaster->e1aaaa, pRaster->e3bbbb);
        __m128 b1a3 = _mm_mul_ss(pRaster->e1bbbb, pRaster->e3aaaa);
        __m128 rcp_a1b3sb1a3 = _mm_rcp_ss( _mm_sub_ss(a1b3, b1a3) );

        __m128 b1c3 = _mm_mul_ss(pRaster->e1bbbb, pRaster->e3cccc);
        __m128 c1b3 = _mm_mul_ss(pRaster->e1cccc, pRaster->e3bbbb);

        __m128 x13 = _mm_mul_ss( _mm_sub_ss(b1c3, c1b3), rcp_a1b3sb1a3);

        __m128 c1a3 = _mm_mul_ss(pRaster->e1cccc, pRaster->e3aaaa);
        __m128 a1c3 = _mm_mul_ss(pRaster->e1aaaa, pRaster->e3cccc);

        __m128 y13 = _mm_mul_ss( _mm_sub_ss(c1a3, a1c3), rcp_a1b3sb1a3);

        // Edge 2, Edge 3
        __m128 a2b3 = _mm_mul_ss(pRaster->e2aaaa, pRaster->e3bbbb);
        __m128 b2a3 = _mm_mul_ss(pRaster->e2bbbb, pRaster->e3aaaa);
        __m128 rcp_a2b3sb2a3 = _mm_rcp_ss( _mm_sub_ss(a2b3, b2a3) );

        __m128 b2c3 = _mm_mul_ss(pRaster->e2bbbb, pRaster->e3cccc);
        __m128 c2b3 = _mm_mul_ss(pRaster->e2cccc, pRaster->e3bbbb);

        __m128 x23 = _mm_mul_ss( _mm_sub_ss(b2c3, c2b3), rcp_a2b3sb2a3);

        __m128 c2a3 = _mm_mul_ss(pRaster->e2cccc, pRaster->e3aaaa);
        __m128 a2c3 = _mm_mul_ss(pRaster->e2aaaa, pRaster->e3cccc);

        __m128 y23 = _mm_mul_ss( _mm_sub_ss(c2a3, a2c3), rcp_a2b3sb2a3);

        // Bounding box
        __m128 minX = _mm_min_ss(x12, x13);
        minX = _mm_min_ss(minX, x23);
        __m128 maxX = _mm_max_ss(x12, x13);
        maxX = _mm_max_ss(maxX, x23);

        __m128 minY = _mm_min_ss(y12, y13);
        minY = _mm_min_ss(minY, y23);
        __m128 maxY = _mm_max_ss(y12, y13);
        maxY = _mm_max_ss(maxY, y23);

        // Snap to tileblock size
        const float TOP_TILE_F = (float) topTileSize * 2.0f;
        const float RCP_TOP_TILE_F = 1.0f / TOP_TILE_F;
        const float FINAL_TILE_F = 2.0f;
        const float RCP_FINAL_TILE_F = 1.0f / FINAL_TILE_F;

        //const _ALIGN(16) float SNAP_MUL[] = {1.0f, 1.0f, -1.0f, -1.0f};
        const _ALIGN(16) float ROUND_OFFSET[] = {-0.5f, -0.5f, 1.5f, 1.5f};
        const _ALIGN(16) float PIXEL_OFFSET[] = {0.5f, 0.5f, -0.5f, -0.5f};

        const __m128 roundOffset = _mm_load_ps(ROUND_OFFSET);
        const __m128 pixelOffset = _mm_load_ps(PIXEL_OFFSET);

        //const __m128 roundOffset = _mm_set_ps1(-0.5f);
        //const __m128 snapMul = _mm_load_ps(SNAP_MUL);

        const __m128 rcpTopTile = _mm_set_ps1(RCP_TOP_TILE_F);
        const __m128 topTile = _mm_set_ps1(TOP_TILE_F);
        const __m128 rcpFinalTile = _mm_set_ps1(RCP_FINAL_TILE_F);
        const __m128 finalTile = _mm_set_ps1(FINAL_TILE_F);

        __m128 bbMin = _mm_unpacklo_ps(minX, minY);
        __m128 bbMax = _mm_unpacklo_ps(maxX, maxY);
        __m128 bb = _mm_movelh_ps(bbMin, bbMax);
        bb = _mm_max_ps(bb, _mm_set1_ps(-999999999.0f));
        bb = _mm_min_ps(bb, _mm_set1_ps(999999999.0f));

        // Top
        __m128 snapVal = _mm_mul_ps(bb, rcpTopTile);    // Snap to tilesize
        snapVal = _mm_add_ps(snapVal, snapVal);         // 2x snapVal
        snapVal = _mm_add_ps(roundOffset, snapVal);     // Floor(minx, miny), Round(maxX+1, maxY+1)
        __m128i roundVal = _mm_cvtps_epi32(snapVal);    
        roundVal = _mm_srai_epi32(roundVal, 1);
        snapVal = _mm_cvtepi32_ps(roundVal);
        snapVal = _mm_mul_ps(snapVal, topTile);
        snapVal = _mm_add_ps(snapVal, pixelOffset);     // Floor(minx, miny) * TILE_SIZE + (0.5, 0.5), Round(maxx+1, maxy+1) * TILE_SIZE + (0.5, 0.5)

        _mm_store_ps(&(pRaster->triBB[0]), snapVal);

        // Final
        snapVal = _mm_mul_ps(bb, rcpFinalTile);
        snapVal = _mm_add_ps(snapVal, snapVal);
        snapVal = _mm_add_ps(roundOffset, snapVal);
        roundVal = _mm_cvtps_epi32(snapVal);
        roundVal = _mm_srai_epi32(roundVal, 1);
        snapVal = _mm_cvtepi32_ps(roundVal);
        snapVal = _mm_mul_ps(snapVal, finalTile);
        snapVal = _mm_add_ps(snapVal, pixelOffset);

        _mm_store_ps(pRaster->triBBFinal, snapVal);

#ifdef _DEBUG
        _DEBUG_ASSERT(pRaster->triBBFinal[1] < pRaster->triBBFinal[3]);

#endif
        //maxXSnap = _mm_mul_ss(maxX, rcpMidTile);
        //maxXSnap = _mm_add_ss(maxXSnap, maxXSnap);
        //maxXSnap = _mm_sub_ss(ceilOffset, maxXSnap);
        //temp = _mm_cvt_ss2si(maxXSnap);
        //temp >>= 1;
        //temp = -temp;
        //maxXSnap = _mm_cvt_si2ss(maxXSnap, temp);
        //maxXSnap = _mm_mul_ss(maxXSnap, midTile);
        //_mm_store_ss(&(triMaxXFinal), maxXSnap);

        //__m128 minYSnap = _mm_mul_ss(minY, rcpTopTile);
        //minYSnap = _mm_add_ss(minYSnap, minYSnap);
        //minYSnap = _mm_add_ss(minYSnap, ceilOffset);
        //temp = _mm_cvt_ss2si(minYSnap);
        //temp >>= 1;
        //minYSnap = _mm_cvt_si2ss(minYSnap, temp);
        //minYSnap = _mm_mul_ss(minYSnap, topTile);
    }
    else
    {
        // TODO: WARNING, PERFORMANCE ISSUE HERE
        // Set bounding box to max screen size
        _ALIGN(16) const float BB[] = {0.5f, 0.5f, 32768.5f, 32768.5f};
        __m128 screenBB = _mm_load_ps(BB);
        _mm_store_ps(&(pRaster->triBB[0]), screenBB);
        _mm_store_ps(&(pRaster->triBB[4]), screenBB);
        _mm_store_ps(pRaster->triBBFinal, screenBB);
    }

    return TRUE;
}

_NAMESPACE_END