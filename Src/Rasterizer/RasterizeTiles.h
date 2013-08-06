//================================================================================
//
//		RasterizeTiles.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		1/12/2011
//
//================================================================================

#pragma once

_NAMESPACE_BEGIN

typedef void (*RasterizeTilesFullyFn)(void* pRasterizationInfo, float startX, float startY);
typedef void (*RasterizeTilesFn)(void* pRasterizationInfo, float startX, float startY);

typedef void (*CustomPixelShaderFn)(int pixelBlockX, int pixelBlockY, 
									FloatSIMD coverage, FloatSIMD depthVals, FloatSIMD* interVals);

template <uint numInterpolants, uint numPixelsH, uint numPixelsV, uint numTileLevels, CustomPixelShaderFn pixelShaderFn>
void RasterizePixelBlock(RasterizationInfo<numInterpolants, numTileLevels>& raster, float startX, float startY);

template <uint numInterpolants, uint numPixelsH, uint numPixelsV, uint numTileLevels, CustomPixelShaderFn pixelShaderFn>
void RasterizePixelBlockNoCheck(RasterizationInfo<numInterpolants, numTileLevels>& raster, float startX, float startY);

template <uint numInterpolants, uint numPixelsH, uint numPixelsV, uint numTileLevels, uint tileLevel, 
		  RasterizeTilesFullyFn rasterizeFullyFn, RasterizeTilesFn rasterizeFn>
void RasterizeTiles(RasterizationInfo<numInterpolants, numTileLevels>& raster, float startX, float startY);

template <uint numInterpolants, uint numPixelsH, uint numPixelsV, uint numTileLevels, uint tileLevel>
void RasterizeTilesInline(RasterizationInfo<numInterpolants, numTileLevels>& raster, float startX, float startY);

_NAMESPACE_END