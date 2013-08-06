//==============================================================================
//
//		RUPreTransVertCacheOpt.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/15/2009
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class RUPreTransVertCacheOpt
{
private: 

public: 
	RUPreTransVertCacheOpt()
	{
	}

	virtual ~RUPreTransVertCacheOpt()
	{
	}

public: 
	void	Optimize(IRVertexBufferGroup* pToOptimize, IRIndexBuffer* pIB);
};

_NAMESPACE_END
