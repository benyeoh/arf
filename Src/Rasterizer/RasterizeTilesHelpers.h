//================================================================================
//
//		RasterizeTilesHelpers.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		6/12/2011
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

template <uint numInterpolants, uint bufferWidth, uint bufferHeight, uint tileLevel, uint numTileLevels, CustomPixelShaderFn ps>
void RasterizeTileLevelFully(void* pRasterizationInfo, float startX, float startY)
{
	const static uint PIXEL_BLOCK_H = ( bufferWidth / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * numTileLevels)) );
	const static uint PIXEL_BLOCK_V = ( bufferHeight / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * numTileLevels)) );
	
	const static uint NUM_PIXELS_H = ( bufferWidth / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * (numTileLevels - tileLevel))) );
	const static uint NUM_PIXELS_V = ( bufferHeight / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * (numTileLevels - tileLevel))) );

	RasterizePixelBlockNoCheck<numInterpolants, NUM_PIXELS_H, NUM_PIXELS_V, numTileLevels, ps>
		(
			*((RasterizationInfo<numInterpolants, numTileLevels>*) pRasterizationInfo), 
			startX, 
			startY
		);

	//const static uint NUM_BLOCKS_H = NUM_PIXELS_H / PIXEL_BLOCK_H;
	//const static uint NUM_BLOCKS_V = NUM_PIXELS_V / PIXEL_BLOCK_V;

	_DEBUG_COMPILE_ASSERT( (bufferWidth % (1 << ((NUM_TILES_SHIFT+1) * numTileLevels))) == 0 );
	_DEBUG_COMPILE_ASSERT( (bufferHeight % (1 << ((NUM_TILES_SHIFT+1) * numTileLevels))) == 0 );
	//
	//_DEBUG_COMPILE_ASSERT( (NUM_PIXELS_H % PIXEL_BLOCK_H) == 0 );
	//_DEBUG_COMPILE_ASSERT( (NUM_PIXELS_V % PIXEL_BLOCK_V) == 0 );
	//
	//if(NUM_PIXELS_H < PIXEL_BLOCK_H || NUM_PIXELS_V < PIXEL_BLOCK_V)
	//{
	//	RasterizePixelBlockNoCheck<numInterpolants, NUM_PIXELS_H, NUM_PIXELS_V, numTileLevels, ps>
	//	(
	//		*((RasterizationInfo<numInterpolants, numTileLevels>*) pRasterizationInfo), 
	//		startX, 
	//		startY
	//	);
	//}
	//else
	//{
	//	float curStartY = startY;

	//	_LOOPi(NUM_BLOCKS_V)
	//	{
	//		float curStartX = startX;

	//		_LOOPj(NUM_BLOCKS_H)
	//		{
	//			RasterizePixelBlockNoCheck<numInterpolants, PIXEL_BLOCK_H, PIXEL_BLOCK_V, numTileLevels, ps>
	//			(
	//				*((RasterizationInfo<numInterpolants, numTileLevels>*) pRasterizationInfo), 
	//				curStartX, 
	//				curStartY
	//			);

	//			curStartX += (float) PIXEL_BLOCK_H;
	//		}

	//		curStartY += (float) PIXEL_BLOCK_V;
	//	}
	//}
}

template <uint numInterpolants, uint bufferWidth, uint bufferHeight, uint tileLevel, uint numTileLevels, CustomPixelShaderFn ps>
void RasterizeTileLevel(void* pRasterizationInfo, float startX, float startY)
{
	const static uint PIXEL_BLOCK_H = (bufferWidth / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * numTileLevels)) );
	const static uint PIXEL_BLOCK_V = (bufferHeight / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * numTileLevels)) );
			
	const static uint NUM_PIXELS_H = ( bufferWidth / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * (numTileLevels - tileLevel))) );
	const static uint NUM_PIXELS_V = ( bufferHeight / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * (numTileLevels - tileLevel))) );
	
	const static uint NUM_BLOCKS_H = NUM_PIXELS_H / PIXEL_BLOCK_H;
	const static uint NUM_BLOCKS_V = NUM_PIXELS_V / PIXEL_BLOCK_V;

	_DEBUG_COMPILE_ASSERT( (bufferWidth % (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * numTileLevels))) == 0 );
	_DEBUG_COMPILE_ASSERT( (bufferHeight % (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * numTileLevels))) == 0 );
	
	_DEBUG_COMPILE_ASSERT( (NUM_PIXELS_H % PIXEL_BLOCK_H) == 0 );
	_DEBUG_COMPILE_ASSERT( (NUM_PIXELS_V % PIXEL_BLOCK_V) == 0 );

	RasterizePixelBlock<numInterpolants, PIXEL_BLOCK_H, PIXEL_BLOCK_V, numTileLevels, ps>
				(
					*((RasterizationInfo<numInterpolants, numTileLevels>*) pRasterizationInfo), 
					startX, 
					startY
				);

	//if(NUM_PIXELS_H < PIXEL_BLOCK_H || NUM_PIXELS_V < PIXEL_BLOCK_V)
	//{
	//	RasterizePixelBlock<numInterpolants, NUM_PIXELS_H, NUM_PIXELS_V, numTileLevels, ps>
	//	(
	//		*((RasterizationInfo<numInterpolants, numTileLevels>*) pRasterizationInfo), 
	//		startX, 
	//		startY
	//	);
	//}
	//else
	//{
	//	float curStartY = startY;

	//	_LOOPi(NUM_BLOCKS_V)
	//	{
	//		float curStartX = startX;

	//		_LOOPj(NUM_BLOCKS_H)
	//		{
	//			RasterizePixelBlock<numInterpolants, PIXEL_BLOCK_H, PIXEL_BLOCK_V, numTileLevels, ps>
	//			(
	//				*((RasterizationInfo<numInterpolants, numTileLevels>*) pRasterizationInfo), 
	//				curStartX, 
	//				curStartY
	//			);

	//			curStartX += (float) PIXEL_BLOCK_H;
	//		}

	//		curStartY += (float) PIXEL_BLOCK_V;
	//	}
	//}
}

template <uint numInterpolants, uint bufferWidth, uint bufferHeight, uint numTileLevels, CustomPixelShaderFn ps>
void RasterizeTileLevel1(void* pRasterizationInfo, float startX, float startY)
{
	const static uint PIXEL_BLOCK_H = (bufferWidth / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * (numTileLevels-1))) );
	const static uint PIXEL_BLOCK_V = (bufferHeight / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * (numTileLevels-1))) );
	
	//const static uint PIXEL_BLOCK_H = (numPixelsH / (1 << ((NUM_TILES_SHIFT+1) * numTileLevels)) );
	//const static uint PIXEL_BLOCK_V = (numPixelsV / (1 << ((NUM_TILES_SHIFT+1) * numTileLevels)) );
	//
	//if(numPixelsH < PIXEL_BLOCK_H || numPixelsV < PIXEL_BLOCK_V)
	//{
	//	RasterizePixelBlock<numInterpolants, numPixelsH, numPixelsV, numTileLevels, ps>
	//	(
	//		*((RasterizationInfo<numInterpolants, numTileLevels>*) pRasterizationInfo), startX, startY
	//	);
	//}
	//else
	{
		RasterizeTiles <numInterpolants,
						PIXEL_BLOCK_H,
						PIXEL_BLOCK_V,
						numTileLevels,
						numTileLevels - 1,

						&RasterizeTileLevelFully <numInterpolants,
												 bufferWidth,
												 bufferHeight,
												 0,
												 numTileLevels,
												 ps>,

						&RasterizeTileLevel <numInterpolants, 
											bufferWidth,
											bufferHeight,
											0,
											numTileLevels, 
											ps>> 
		(
			*((RasterizationInfo<numInterpolants, numTileLevels>*) pRasterizationInfo), 
			startX, 
			startY
		);
	}
}

template <uint numInterpolants, uint bufferWidth, uint bufferHeight, uint numTileLevels, CustomPixelShaderFn ps>
void RasterizeTileLevel2(void* pRasterizationInfo, float startX, float startY)
{
	const static uint PIXEL_BLOCK_H = (bufferWidth / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * (numTileLevels-2))) );
	const static uint PIXEL_BLOCK_V = (bufferHeight / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * (numTileLevels-2))) );
	
	//if(numPixelsH < PIXEL_BLOCK_H || numPixelsV < PIXEL_BLOCK_V)
	//{
	//	RasterizePixelBlock<numInterpolants, numPixelsH, numPixelsV, numTileLevels, ps>
	//	(
	//		*((RasterizationInfo<numInterpolants, numTileLevels>*) pRasterizationInfo), startX, startY
	//	);
	//}
	//else
	{
		RasterizeTiles <numInterpolants, 
						PIXEL_BLOCK_H,
						PIXEL_BLOCK_V,
						numTileLevels,
						numTileLevels - 2,

						&RasterizeTileLevelFully <numInterpolants,
												 bufferWidth,
												 bufferHeight,
												 1,
												 numTileLevels,
												 ps>,

						&RasterizeTileLevel1 <numInterpolants, 
											bufferWidth,
											bufferHeight,
											numTileLevels, 
											ps>> 
		(
			*((RasterizationInfo<numInterpolants, numTileLevels>*) pRasterizationInfo), 
			startX, 
			startY
		);
	}
}

template <uint numInterpolants, uint bufferWidth, uint bufferHeight, uint numTileLevels, CustomPixelShaderFn ps>
void RasterizeTileLevel3(void* pRasterizationInfo, float startX, float startY)
{
	const static uint PIXEL_BLOCK_H = (bufferWidth / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * (numTileLevels-3))) );
	const static uint PIXEL_BLOCK_V = (bufferHeight / (1 << ((NUM_TILES_SHIFT+SIMD_BLOCK_SHIFT) * (numTileLevels-3))) );
	
	//if(numPixelsH < PIXEL_BLOCK_H || numPixelsV < PIXEL_BLOCK_V)
	//{
	//	RasterizePixelBlock<numInterpolants, numPixelsH, numPixelsV, numTileLevels, ps>
	//	(
	//		*((RasterizationInfo<numInterpolants, numTileLevels>*) pRasterizationInfo), startX, startY
	//	);
	//}
	//else
	{
		RasterizeTiles <numInterpolants, 
						PIXEL_BLOCK_H,
						PIXEL_BLOCK_V,
						numTileLevels,
						numTileLevels - 3,

						&RasterizeTileLevelFully <numInterpolants,
												 bufferWidth,
												 bufferHeight,
												 2,
												 numTileLevels,
												 ps>,

						&RasterizeTileLevel2 <numInterpolants, 
											bufferWidth,
											bufferHeight,
											numTileLevels, 
											ps>> 
		(
			*((RasterizationInfo<numInterpolants, numTileLevels>*) pRasterizationInfo), 
			startX, 
			startY
		);
	}
}

_NAMESPACE_END