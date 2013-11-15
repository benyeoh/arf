//==============================================================================
//
//		RUPostTransVertCacheOpt.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		2/2/2009
//
//==============================================================================

#include "RendererUtils.h"

_NAMESPACE_BEGIN

inline float
RUPostTransVertCacheOpt::GetTriangleScore(TriangleData* pTri)
{
	float score	= m_pVertices[pTri->vertIndices[0]].score;
	score		+= m_pVertices[pTri->vertIndices[1]].score;
	score		+= m_pVertices[pTri->vertIndices[2]].score;
	
	return score;
}


inline float
RUPostTransVertCacheOpt::GetVertexScore(VertexData* pVert)
{
	const static float CACHE_DECAY_POWER	= 1.5f;
	const static float LAST_TRI_SCORE		= 0.75f;
	const static float VALENCE_BOOST_SCALE	= 2.0f;
	const static float VALENCE_BOOST_POWER	= 0.5f;

	if(pVert->numTrianglesNotAdded == 0)
    {
        // No tri needs this vertex!
        return -1.0f;
    }

    float score = 0.0f;
    if(pVert->pNode != NULL)
    {
        if(pVert->pNode->pos < 3)
        {

            // This vertex was used in the last triangle,
            // so it has a fixed score, whichever of the three
            // it's in. Otherwise, you can get very different
            // answers depending on whether you add
            // the triangle 1,2,3 or 3,1,2 - which is silly.

            score = LAST_TRI_SCORE;
        }
        else
        {
			_DEBUG_ASSERT(pVert->pNode->pos < CACHE_SIZE);

            // Points for being high in the cache.
            const float scaler = 1.0f / (CACHE_SIZE - 3);

            score = 1.0f - (pVert->pNode->pos - 3) * scaler;
            score = gmtl::Math::pow(score, CACHE_DECAY_POWER);
        }
    }

    // Bonus points for having a low number of tris still to
    // use the vert, so we get rid of lone verts quickly.
    _DEBUG_ASSERT(pVert->numTrianglesNotAdded > 0);
    float valenceBoost = gmtl::Math::pow((float)pVert->numTrianglesNotAdded, -VALENCE_BOOST_POWER);
    score += VALENCE_BOOST_SCALE * valenceBoost;
    return score;
}

void
RUPostTransVertCacheOpt::InitializeVertices()
{
	_LOOPi(m_NumVertices)
	{
		m_pVertices[i].pNode		= NULL;
		m_pVertices[i].score		= 0.0f;
		m_pVertices[i].numTotalTriangles	= 0;
		m_pVertices[i].numTrianglesNotAdded = 0;
	}
	
	_LOOPi(m_NumTriangles)
	{
		uint index = m_pTriangles[i].vertIndices[0];
		m_pVertices[index].triangleIndices[m_pVertices[index].numTotalTriangles] = i;
		m_pVertices[index].numTotalTriangles++;
		m_pVertices[index].numTrianglesNotAdded++;
		
		_DEBUG_ASSERT(m_pVertices[index].numTotalTriangles < VertexData::MAX_TRIANGLE_INDICES);

		index = m_pTriangles[i].vertIndices[1];
		m_pVertices[index].triangleIndices[m_pVertices[index].numTotalTriangles] = i;
		m_pVertices[index].numTotalTriangles++;
		m_pVertices[index].numTrianglesNotAdded++;
		
		_DEBUG_ASSERT(m_pVertices[index].numTotalTriangles < VertexData::MAX_TRIANGLE_INDICES);

		index = m_pTriangles[i].vertIndices[2];
		m_pVertices[index].triangleIndices[m_pVertices[index].numTotalTriangles] = i;
		m_pVertices[index].numTotalTriangles++;
		m_pVertices[index].numTrianglesNotAdded++;
		
		_DEBUG_ASSERT(m_pVertices[index].numTotalTriangles < VertexData::MAX_TRIANGLE_INDICES);
	}
	
	_LOOPi(m_NumVertices)
	{
		m_pVertices[i].score = GetVertexScore(m_pVertices+i);
	}
}

void
RUPostTransVertCacheOpt::InitializeTriangles(IRIndexBuffer* pIB)
{
	ushort* pIndices = pIB->Lock(0, 0);
	
	_LOOPi(m_NumTriangles)
	{
		m_pTriangles[i].vertIndices[0] = pIndices[i*3];
		m_pTriangles[i].vertIndices[1] = pIndices[i*3+1];
		m_pTriangles[i].vertIndices[2] = pIndices[i*3+2];
		
		m_pTriangles[i].isAdded = FALSE;
		m_pTriangles[i].score	= 0.0f;
	}
		
	pIB->Unlock(FALSE);
	
	InitializeVertices();
	
	float highestScoreSoFar = -99999.0f;
	
	_LOOPi(m_NumTriangles)
	{
		float score = GetTriangleScore(&(m_pTriangles[i]));
		m_pTriangles[i].score = score;
		
		if(score > highestScoreSoFar)
		{
			highestScoreSoFar = score;
			m_pHighestTriangle = &(m_pTriangles[i]);
		}	
	}
}

void
RUPostTransVertCacheOpt::Optimize(IRIndexBuffer* pToOptimize, uint numVertices)
{
	_DEBUG_ASSERT(pToOptimize->GetNumIndices() % 3 == 0);
	m_NumTriangles		= pToOptimize->GetNumIndices() / 3;
	m_NumVertices		= numVertices;
	m_NumDrawnTriangles = 0;
	
	m_pVertices			= _NEW VertexData[m_NumVertices];
	m_pTriangles		= _NEW TriangleData[m_NumTriangles];
	m_ppDrawnTriangles	= _NEW TriangleData*[m_NumTriangles];
	
	InitializeTriangles(pToOptimize);	

	_LOOPi(CACHE_SIZE-1)
	{
		m_CacheNodes[i].pNext = &(m_CacheNodes[i+1]);
	}
	
	_LOOPi(CACHE_SIZE-1)
	{
		m_CacheNodes[i+1].pPrev = &(m_CacheNodes[i]);	
	}
	
	_LOOPi(CACHE_SIZE)
	{
		m_CacheNodes[i].pVert	= NULL;
		m_CacheNodes[i].pos		= i;
	}
	
	m_pHead = &(m_CacheNodes[0]);
	m_pHead->pPrev = NULL;
	m_pTail = &(m_CacheNodes[CACHE_SIZE-1]);
	m_pTail->pNext = NULL;
		
	Simulate();
	
	// Fill index buffer here
	
	_DEBUG_ASSERT(m_NumTriangles == m_NumDrawnTriangles);
	ushort* pToFill = pToOptimize->Lock(0, m_NumTriangles*3);
	
	_LOOPi(m_NumTriangles)
	{
		pToFill[i*3] = m_ppDrawnTriangles[i]->vertIndices[0];
		pToFill[i*3+1] = m_ppDrawnTriangles[i]->vertIndices[1];
		pToFill[i*3+2] = m_ppDrawnTriangles[i]->vertIndices[2];
	}
	
	pToOptimize->Unlock();
	
	_DELETE_ARRAY(m_pVertices);
	_DELETE_ARRAY(m_pTriangles);
	_DELETE_ARRAY(m_ppDrawnTriangles);
}

void
RUPostTransVertCacheOpt::MoveToHead(CacheNode* pNode)
{
	if(pNode != m_pHead)
	{
		CacheNode* pPrev = pNode->pPrev;
		CacheNode* pNext = pNode->pNext;
		
		if(pPrev)
			pPrev->pNext = pNext;
		
		if(pNext)
			pNext->pPrev = pPrev;
		
		if(pNode == m_pTail)
			m_pTail = pPrev;
			
		m_pHead->pPrev = pNode;
	
		pNode->pPrev = NULL;
		pNode->pNext = m_pHead;
		
		m_pHead = pNode;
	}
}

RUPostTransVertCacheOpt::VertexData* 
RUPostTransVertCacheOpt::AddOneVertToCache(VertexData& vert)
{
	VertexData* pJustBooted = NULL;
	
	if(vert.pNode)
	{
		MoveToHead(vert.pNode);
	}
	else
	{
		if(m_pTail->pVert)
		{
			_DEBUG_ASSERT(m_pTail->pVert->pNode == m_pTail);
			m_pTail->pVert->pNode = NULL;
		}
		
		pJustBooted = m_pTail->pVert;
		
		m_pTail->pVert = &vert;
		vert.pNode = m_pTail;

		// Move the current tail to the head
		MoveToHead(m_pTail);
	}
	
	return pJustBooted;
}

void
RUPostTransVertCacheOpt::AddToVertCache(VertexData& v1, 
										VertexData& v2,
										VertexData& v3)
{
	VertexData* pJustBooted[3];
	
	pJustBooted[0] = AddOneVertToCache(v1);
	pJustBooted[1] = AddOneVertToCache(v2);
	pJustBooted[2] = AddOneVertToCache(v3);

	// Update cache positions	
	CacheNode* pCurNode = m_pHead;
	uint curPos = 0;
	while(pCurNode)
	{
		pCurNode->pos = curPos;
		curPos++;

		VertexData* pCurVert = pCurNode->pVert;
		if(pCurVert)
			pCurVert->score = GetVertexScore(pCurVert);
			
		pCurNode = pCurNode->pNext;
	}
	
	_LOOPi(3)
	{
		if(pJustBooted[i])
			pJustBooted[i]->score = GetVertexScore(pJustBooted[i]);
	}
	
	// Update triangle scores
	m_pHighestTriangle = NULL;
	float highestScoreSoFar = -99999.0f;
	
	pCurNode = m_pHead;
	while(pCurNode)
	{
		VertexData* pCurVert = pCurNode->pVert;
		if(pCurVert)
		{
			_LOOPi(pCurVert->numTrianglesNotAdded)
			{
				TriangleData* pCurTri = &(m_pTriangles[pCurVert->triangleIndices[i]]);
				_DEBUG_ASSERT(!pCurTri->isAdded);
				pCurTri->score = GetTriangleScore(pCurTri);
				if(pCurTri->score > highestScoreSoFar)
				{
					m_pHighestTriangle = pCurTri;
					highestScoreSoFar = pCurTri->score;
				}
			}
			
			//uint startTri = pCurVert->numTotalTriangles - pCurVert->numTrianglesNotAdded;
			//while(startTri < (uint)pCurVert->numTrianglesNotAdded)
			//{
			//	// NOTE:!!! startTri INDEX IS WRONG
			//	TriangleData* pCurTri = &(m_pTriangles[pCurVert->triangleIndices[startTri]]);
			//	pCurTri->score = GetTriangleScore(pCurTri);
			//	if(pCurTri->score > highestScoreSoFar)
			//	{
			//		m_pHighestTriangle = pCurTri;
			//		highestScoreSoFar = pCurTri->score;
			//	}
			//	
			//	startTri++;
			//}
		}
			
		pCurNode = pCurNode->pNext;
	}
	
	_LOOPi(3)
	{
		if(pJustBooted[i])
		{	
			_LOOPj(pJustBooted[i]->numTrianglesNotAdded)
			{
				TriangleData* pCurTri = &(m_pTriangles[pJustBooted[i]->triangleIndices[j]]);
				_DEBUG_ASSERT(!pCurTri->isAdded);
				pCurTri->score = GetTriangleScore(pCurTri);
				if(pCurTri->score > highestScoreSoFar)
				{
					m_pHighestTriangle = pCurTri;
					highestScoreSoFar = pCurTri->score;
				}
			}
		}
	}
	
	if(!m_pHighestTriangle)
	{
		_LOOPi(m_NumVertices)
		{
			if(m_pVertices[i].numTrianglesNotAdded > 0)
				m_pVertices[i].score = GetVertexScore(m_pVertices+i);
		}
		
		_LOOPi(m_NumTriangles)
		{
			if(!m_pTriangles[i].isAdded)
			{
				m_pTriangles[i].score = GetTriangleScore(m_pTriangles + i);
				if(highestScoreSoFar < m_pTriangles[i].score)
				{
					highestScoreSoFar = m_pTriangles[i].score;
					m_pHighestTriangle = m_pTriangles + i;
				}
			}
		}
	}
}

void
RUPostTransVertCacheOpt::RemoveTriangleFromVertex(TriangleData& tri, VertexData& data)
{
	uint triangleIndex = (uint) (((size_t)(&tri)) - (size_t) m_pTriangles) / sizeof(TriangleData);
	_LOOPi(data.numTrianglesNotAdded)
	{
		if(data.triangleIndices[i] == triangleIndex)
		{
			_LOOPj(data.numTrianglesNotAdded - i - 1)
			{
				uint destIndex = j+i;
				uint srcIndex = j+i+1;
				data.triangleIndices[destIndex]	= data.triangleIndices[srcIndex];	
			}
			
			data.numTrianglesNotAdded--;			
			return;
		}
	}
	
	_DEBUG_ASSERT(FALSE);
}

void
RUPostTransVertCacheOpt::DrawTriangle(TriangleData& tri)
{
	_DEBUG_ASSERT(!tri.isAdded);
	
	tri.isAdded = TRUE;
	m_ppDrawnTriangles[m_NumDrawnTriangles++] = &tri;
	
	RemoveTriangleFromVertex(tri, m_pVertices[tri.vertIndices[0]]);
	RemoveTriangleFromVertex(tri, m_pVertices[tri.vertIndices[1]]);
	RemoveTriangleFromVertex(tri, m_pVertices[tri.vertIndices[2]]);
			
	AddToVertCache(m_pVertices[tri.vertIndices[0]],
					m_pVertices[tri.vertIndices[1]],
					m_pVertices[tri.vertIndices[2]]);
}

void	
RUPostTransVertCacheOpt::Simulate()
{
	while(m_pHighestTriangle)
	{
		DrawTriangle(*m_pHighestTriangle);		
	}
}

_NAMESPACE_END