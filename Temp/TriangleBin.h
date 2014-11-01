#ifndef EBONY_RENDERER_DEPTH_RASTERIZER_TRIANGLE_BIN
#define EBONY_RENDERER_DEPTH_RASTERIZER_TRIANGLE_BIN

namespace SQEX { namespace Ebony { namespace Renderer {

struct TriangleData
{
	unsigned int xy[3];
	float z[3];
};

struct TriangleBin
{
	const static unsigned int MAX_NUM_TRIANGLES_PER_BIN = 1024 * 8;

	TriangleData tris[MAX_NUM_TRIANGLES_PER_BIN];
};

}}}

#endif 
