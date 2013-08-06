//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		12/14/2008
//
//==============================================================================


#pragma once 

_NAMESPACE_BEGIN

struct SDBDataHandle
{
    const static uint INVALID_HANDLE = 0xFFFFFFFF;

    size_t handle;
    
    SDBDataHandle()                             { Invalidate(); }
    SDBDataHandle(size_t val)                   { handle = val; }
    SDBDataHandle(const SDBDataHandle& other)   { handle = other.handle; }

    operator size_t()                       { return handle; }
    SDBDataHandle& operator=(size_t val)    { handle = val; return *this; }

    inline void    Invalidate()             { handle = INVALID_HANDLE; }
    inline boolean IsValid()                { return handle != INVALID_HANDLE; }
};

struct ISDBSceneContainer : public IRefCounted
{
	_PURE( SDBDataHandle Insert(const AABox* pBV, const OOBox* pOOBV, uint filterType, void* pData) )
	_PURE( void	Remove(SDBDataHandle handle) )
	_PURE( boolean UpdateMove(SDBDataHandle handle, const AABox* pNewBounds) )

	_PURE( uint	Query(void** ppToStore, uint bufferSize, const AABox& queryBV, uint filterType) )
	_PURE( uint	QuerySphere(void** ppToStore, uint bufferSize, const AABox& sphereAA, const Sphere& sphereBV, uint filterType) )
	_PURE( uint	QueryInFrustumOrtho(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, uint filterType) )
	_PURE( uint	QueryInFrustumPersp(void** ppToStore, AABox* pScreenSpaceRes, uint bufferSize, AABox& frustumAA, const gmtl::MatrixA44f& viewProj, float nearZ, float farZ, uint filterType) )

	_PURE( uint QueryAll(void** ppToStore, uint bufferSize, uint filterType) )
};

struct ISDBArray : public ISDBSceneContainer
{

};

struct ISDBSparseLooseOctTree : public ISDBSceneContainer
{
	_PURE( uint	QueryCellInfo(AABox* pToStore, uint bufferSize, const AABox& queryBV) )
	_PURE( float GetMaxGridSize() )
};

struct ISDBResourceMgr
{
	// Levels = num of hierachical oct-tree layers
	// Scale = size of the smallest grid
	// initSize = hash map initial size (not needed)
	_PURE( ISDBSparseLooseOctTree* CreateSparseLooseOctTree(uint levels, float scale, uint initSize) );

	_PURE( ISDBSparseLooseOctTree* CreateOctantOrderLooseOctTree(uint levels, float scale) );
	_PURE( ISDBArray* CreateArray() );
};

struct ISDBSceneDB : public IRefCounted
{
	_PURE( void Initialize() )
	_PURE( ISDBResourceMgr& GetResourceMgr() )
};


//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(ISDBSceneDB)
_DECLARE_SMARTPTR(ISDBSparseLooseOctTree)
_DECLARE_SMARTPTR(ISDBSceneContainer)
_DECLARE_SMARTPTR(ISDBArray)

_NAMESPACE_END


//////////////////////////////////////////////////////////////////////////
//	 
//	DLL exposed functions
//
//////////////////////////////////////////////////////////////////////////


extern "C"
{
	typedef _NAMESPACE ISDBSceneDB* (*CreateSceneDBFn)();
	_DECL_DYNAMIC_EXPORT( _NAMESPACE ISDBSceneDB* CreateSceneDB() )
}
