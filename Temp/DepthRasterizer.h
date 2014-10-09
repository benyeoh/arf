#ifndef EBONY_RENDERER_DEPTH_RASTERIZER_DEPTH_RASTERIZER
#define EBONY_RENDERER_DEPTH_RASTERIZER_DEPTH_RASTERIZER

#include "../Prototype.h"
#include "Ebony/Math/Math.h"

#include "TriangleBin.h"

namespace SQEX { namespace Ebony { namespace Renderer {

class DepthRasterizer
{
public:
	const static int BUFFER_WIDTH		= 256;
	const static int BUFFER_HEIGHT		= 128;
	const static int MAX_NUM_MIPS		= 5;

public:
	DepthRasterizer()
		: numTrisInBin_(0)
	{
		for(int i=0; i < MAX_NUM_MIPS; i++)
		{
			pDepthBuffer_[i] = nullptr;
		}
	}

	~DepthRasterizer()
	{
		for(int i=0; i < MAX_NUM_MIPS; i++)
		{
			if(pDepthBuffer_[i])
				Ebony::Base::Memory::Free(pDepthBuffer_[i]);

			pDepthBuffer_[i] = nullptr;
		}
	}

public:
	void Initialize(bool isUseHiZ=false);

	void ClearDepth();	
	void Draw(const Math::MatrixA& worldViewProj, const float* pXYZ, unsigned int posStride, unsigned int numVertices, const unsigned short* pIndices, unsigned int numTriangles);
	void Flush();

	void UntileAndLinearizeDepthBuffer(float* pDest, float farZ, float nearZ, int mipLevel);
	void UntileDepthBuffer(float* pDest, int mipLevel);

	const float* GetDepthBuffer(int mipLevel=0) { return pDepthBuffer_[mipLevel]; }
	float** GetHiZDepthBuffer() { return (float**) pDepthBuffer_; }

private:
	void TransformVertices(Math::VectorA* pXYZWOut, const Math::MatrixA& worldViewProj, const float* pXYZ, unsigned int posStride, unsigned int numVertices);
	void GenerateHiZ2Levels(int startMip);

private:
	float* pDepthBuffer_[MAX_NUM_MIPS];
	
	TriangleBin triBin_;
	unsigned int numTrisInBin_;
};

}}}

#endif
