//==============================================================================
//
//		DenseChainedHashMap.h
//
//			(Only accepts POD value types atm)
//
//		Author:		Ben Yeoh
//		Date:		12/14/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

struct NullUintHash
{
	inline uint operator() (uint key) { return key; }
};

struct NullSizeTHash
{
	inline size_t operator() (size_t key) { return key; }
};

const static uint MAX_PRIME_NUMBERS = 20;
const static uint PRIME_NUMBERS[MAX_PRIME_NUMBERS] =
{
	1,
	2,
	5,
	11,
	17,
	37,
	67,
	131,
	257,
	521,
	1031,
	2053,
	4099,
	8209,
	16411,
	32771,
	65539,
	131101,
	262147,
	524309
};

// Warning: Only POD value types
template<typename Key, typename Value, typename HashFn>
class DenseChainedHashMap
{
	#define	__DCHM_BUCKET_UNUSED	((Bucket*)(size_t)0xFFFFFFFF)

	typedef DenseChainedHashMap<Key, Value, HashFn> TheHashMap;
	const static uint MIN_BUCKET_SIZE	= 32;
	
	struct Bucket
	{
		//uint	indices;
		Bucket* pNext;
		Value	val;
		Key		key;
	};

public:
	class Iterator
	{
	private:
		TheHashMap* m_pParent;
		uint	m_Index;
		Bucket* m_pNextBucket;
		
		Key*	m_pKey;
		Value*	m_pVal;
		
	public:
		Iterator(TheHashMap* pParent) 
			: m_pParent(pParent) 
			, m_Index(0)
			, m_pNextBucket(NULL)
		{
			Next();
		}
	
	public:
		inline Key*		GetKey()	{ return m_pKey; }
		inline Value*	GetValue()	{ return m_pVal; }
		
		inline boolean	IsValid()
		{
			return m_pKey != NULL;
		}

		void	Next() 
		{
			m_pKey	= NULL;
			m_pVal	= NULL;
						
			if(m_pNextBucket == NULL)
			{
				for(uint i=m_Index; i < m_pParent->m_NumOfBuckets; i++)
				{
					Bucket* pBucket = &(m_pParent->m_pBuckets[i]);
					if(pBucket->pNext != __DCHM_BUCKET_UNUSED)
					{
						m_Index = i+1;
						m_pNextBucket = pBucket->pNext;
						m_pKey	= &(pBucket->key);
						m_pVal	= &(pBucket->val);
						break;
					}
				}
			}
			else
			{
				Bucket* pBucket = m_pNextBucket;
				m_pNextBucket = pBucket->pNext;
				m_pKey	= &(pBucket->key);
				m_pVal	= &(pBucket->val);
			}
		}
	};
	
private: 
	Bucket*		m_pBuckets;
	Bucket*		m_pChained;
	Bucket*		m_pFreeChain;
	uint		m_NumOfBuckets;
	uint		m_NumOfChain;
	
	uint		m_Occupancy;
	uint		m_ChainUsed;
	
	float		m_ExpandRate;
	
	HashFn		m_Hash;
	
public:
	DenseChainedHashMap()
		: m_Occupancy(0)
		, m_ChainUsed(0)
		, m_pBuckets(NULL)
		, m_pChained(NULL)
		, m_pFreeChain(NULL)
		, m_NumOfBuckets(0)
		, m_NumOfChain(0)
		, m_ExpandRate(0.0f)
	{
	}
	
	DenseChainedHashMap(uint initSize, float expandRate)
		: m_Occupancy(0)
		, m_ChainUsed(0)
		, m_pBuckets(NULL)
		, m_pChained(NULL)
	{		
		Initialize(initSize, expandRate);
	}

	~DenseChainedHashMap()
	{
		_DELETE_ARRAY(m_pBuckets);
		_DELETE_ARRAY(m_pChained);
	}

private:
	uint FindNextPrime(uint index)
	{
		_DEBUG_ASSERT(index < MAX_PRIME_NUMBERS);
		return PRIME_NUMBERS[index];
	}

	Bucket*	CreateBuckets(uint bucketSize)
	{
		Bucket* pBuckets = _NEW Bucket[bucketSize];
		_LOOPi(bucketSize)
		{
			pBuckets[i].pNext = __DCHM_BUCKET_UNUSED;
		}
		
		return pBuckets;
	}
	
	Bucket*	CreateChain(uint chainSize)
	{
		//if(chainSize > MAX_CHAIN_SIZE)
		//	chainSize = MAX_CHAIN_SIZE;
		//if(chainSize < MIN_CHAIN_SIZE)
		//	chainSize = MIN_CHAIN_SIZE;
				
		Bucket* pChained = _NEW Bucket[chainSize];

		_LOOPi(chainSize-1)
		{
			pChained[i].pNext	= &(pChained[i+1]);
		}

		pChained[chainSize-1].pNext = NULL;
		return pChained;
	}
	
	inline uint GetHash(const Key& key)
	{
		return m_Hash(key) % (m_NumOfBuckets);
	}
	
	inline void FreeChainBucket(Bucket* pToFree)
	{
		// Add to the head of the chain
		pToFree->pNext		= m_pFreeChain;
		m_pFreeChain		= pToFree;
		
		m_ChainUsed--;
	}
	
	inline void AllocChainBucket(Bucket* pBucket, const Key& key, const Value& val)
	{
		_DEBUG_ASSERT(m_pFreeChain);
		
		// Allocate the next free bucket
		pBucket->pNext		= m_pFreeChain;
		m_pFreeChain->key	= key;
		m_pFreeChain->val	= val;

		Bucket* pNextChain = m_pFreeChain->pNext;
		m_pFreeChain->pNext	= NULL;
		
		m_pFreeChain = pNextChain;
				
		m_ChainUsed++;
	}
	
	inline Bucket* FindNextBucketWithKey(Bucket* pBucket, const Key& key)
	{
		while(pBucket->pNext)
		{
			pBucket = pBucket->pNext;
			if(pBucket->key == key)
				return pBucket;
		}
		
		return NULL;
	}
	
	inline boolean IsChainAvailable()
	{
		return m_pFreeChain != NULL;
	}
	
	void Resize(uint newSize)
	{		
		_DEBUG_ASSERT(newSize > MIN_BUCKET_SIZE);
		
		TheHashMap cloned(newSize, m_ExpandRate);
		_LOOPi(m_NumOfBuckets)
		{
			Bucket* pBucket = &(m_pBuckets[i]);
			if(pBucket->pNext != __DCHM_BUCKET_UNUSED)
			{
				cloned.Add(pBucket->key, pBucket->val);
				while(pBucket->pNext)
				{
					pBucket = pBucket->pNext;
					cloned.Add(pBucket->key, pBucket->val);
				}
			}
		}
		
		// Swap data over
		Bucket* pNewBuckets = cloned.m_pBuckets;
		Bucket* pNewChain	= cloned.m_pChained;
		cloned.m_pBuckets	= m_pBuckets;
		cloned.m_pChained	= m_pChained;
		
		m_pBuckets			= pNewBuckets;
		m_pChained			= pNewChain;		
		m_pFreeChain		= cloned.m_pFreeChain;
		m_NumOfBuckets		= cloned.m_NumOfBuckets;
		m_NumOfChain		= cloned.m_NumOfChain;
		m_Occupancy			= cloned.m_Occupancy;
		m_ChainUsed			= cloned.m_ChainUsed;
	}
	
	inline void ExpandIfNeeded()
	{
		if( (m_Occupancy > (uint)(m_NumOfBuckets * m_ExpandRate)) ||
			(m_ChainUsed >= m_NumOfChain) )
		{
			Resize(m_NumOfBuckets << 1);
		}
	}
	
	//inline void ShrinkIfNeeded()
	//{
	//	if( (m_Occupancy < (uint)(m_NumOfBuckets * OCCUPANCY_SHRINK_RATE)) &&
	//		m_Occupancy <= m_NumOfChain )
	//	{
	//		
	//	}
	//}
	
public:
	void	Initialize(uint initSize, float expandRate)
	{
		if(m_pBuckets)
			_DELETE_ARRAY(m_pBuckets);
		
		if(m_pChained)
			_DELETE_ARRAY(m_pChained);

		m_Occupancy	= 0;
		m_ChainUsed	= 0;
		m_ExpandRate = expandRate;
		
		if(initSize < MIN_BUCKET_SIZE)
			initSize = MIN_BUCKET_SIZE;

		uint log2Buckets = GetLog2(initSize);
		m_NumOfBuckets	= FindNextPrime(log2Buckets); //NextPowOf2(initSize);
							
		m_pBuckets		= CreateBuckets(m_NumOfBuckets);
		
		m_NumOfChain	= m_NumOfBuckets / 7;
		if(m_NumOfChain < 8)
			m_NumOfChain = 8;

		m_pChained		= CreateChain(m_NumOfChain);
				
		m_pFreeChain	= m_pChained;
	}
	
	void	Reset()
	{
		m_Occupancy	= 0;
		m_ChainUsed	= 0;

		_LOOPi(m_NumOfBuckets)
		{
			m_pBuckets[i].pNext = __DCHM_BUCKET_UNUSED;
		}

		_LOOPi(m_NumOfChain-1)
		{
			m_pChained[i].pNext	= &(m_pChained[i+1]);
		}

		m_pChained[m_NumOfChain-1].pNext = NULL;
		m_pFreeChain	= m_pChained;
	}

	uint	GetNumOfElements()
	{
		return m_Occupancy;
	}

	Iterator	BeginIter() { return Iterator(this); }

	Value*	Get(const Key& key)
	{
		// Search through bucket and chained list
		uint hashVal		= GetHash(key);
		Bucket* pBucket		= &(m_pBuckets[hashVal]);
		
		if(pBucket->pNext != __DCHM_BUCKET_UNUSED)
		{
			if(pBucket->key == key)
				return &(pBucket->val);
			
			while(pBucket->pNext )
			{
				pBucket = pBucket->pNext;
				if(pBucket->key == key)
					return &(pBucket->val);
			}
		}
		
		return NULL;		
	}
	
	Value*	AddDeferred(const Key& key)
	{
		uint hashVal	= GetHash(key);
		Bucket* pBucket	= &(m_pBuckets[hashVal]);

		if(pBucket->pNext == __DCHM_BUCKET_UNUSED)
		{
			pBucket->pNext		= NULL;
			pBucket->key		= key;
			
			m_Occupancy++;
			return &(pBucket->val);
		}

		if(pBucket->key == key)
		{
			// Replace with the existing
			return &(pBucket->val);
		}
		
		while(pBucket->pNext)
		{
			pBucket = pBucket->pNext;
			if(pBucket->key == key)
			{
				return &(pBucket->val);
			}
		}
		
		m_Occupancy++;
		
		_DEBUG_ASSERT(m_pFreeChain);
		
		// Allocate the next free bucket
		pBucket->pNext		= m_pFreeChain;
		m_pFreeChain->key	= key;

		Bucket* pNextChain = m_pFreeChain->pNext;
		m_pFreeChain->pNext	= NULL;
		
		m_pFreeChain = pNextChain;
				
		m_ChainUsed++;
		
		return &(pBucket->pNext->val);
	}
	
	void	FinishedAdding()
	{
		ExpandIfNeeded();
	}
	
	void	Add(const Key& key, const Value& val)
	{
		Value* pVal = AddDeferred(key);
		*pVal = val;
		FinishedAdding();
	}
	
	void	Remove(const Key& key)
	{
		uint hashVal	= GetHash(key);
		Bucket* pBucket	= &(m_pBuckets[hashVal]);
		
		if(pBucket->pNext != __DCHM_BUCKET_UNUSED)
		{
			if(pBucket->key == key)
			{
				// Found, so replace the main bucket with the first in
				// the chain, if applicable
				if(pBucket->pNext)
				{
					Bucket* pNext		= pBucket->pNext;
					pBucket->pNext		= pNext->pNext;
					pBucket->key		= pNext->key;
					pBucket->val		= pNext->val;
					
					FreeChainBucket(pNext);	
				}
				else
				{
					pBucket->pNext	= __DCHM_BUCKET_UNUSED;
				}
				
				m_Occupancy--;				
			}
			else
			{
				while(pBucket->pNext)
				{
					Bucket* pNext = pBucket->pNext;
					if(pNext->key == key)
					{
						// Found so remove the bucket from the chain
						// Link the prev bucket to the next in the chain
						pBucket->pNext = pNext->pNext;
						FreeChainBucket(pNext);
						
						m_Occupancy--;
						break;
					}
					
					pBucket = pNext;
				}	
			}
		}
	}
	
};

_NAMESPACE_END
