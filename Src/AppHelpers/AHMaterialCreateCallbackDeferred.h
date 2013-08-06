//================================================================================
//
//		AHMaterialCreateCallbackDeferred.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		11/23/2012
//
//================================================================================

#pragma once

#if defined(_IS_USE_APPHELPER_LIB_BASIC)

_NAMESPACE_BEGIN

class AHMaterialCreateCallbackDeferred : public CRefCounted<IRefCounted>, public IBFXMaterialCreateCallback
{
	struct UncachedMat
	{
		IBFXMaterialGroup* pMatGroup;
		uint matType;
		uint uncachedFlags;

		uint hash;

		void SetHash()
		{
			hash = MurmurHash((uint*)(this), 3); 
		}

		bool operator==(const UncachedMat& other) const
		{
			return pMatGroup == other.pMatGroup && matType == other.matType && uncachedFlags == other.uncachedFlags;
		}
	};

	struct UncachedMatHash
	{
		inline uint operator() (const UncachedMat& key) { return key.hash; }
	};

	typedef DenseChainedHashMap<UncachedMat, boolean, UncachedMatHash> UncachedMatMap;

private:
	Mutex						m_UncachedMatMutex;
	UncachedMatMap				m_UncachedMatMap;
	std::vector<UncachedMat>	m_UncachedMatList;

public:
	AHMaterialCreateCallbackDeferred()
	{
		m_UncachedMatMutex.Initialize();
		m_UncachedMatMap.Initialize(128, 0.67f);
	}

	~AHMaterialCreateCallbackDeferred()
	{
	}

public:
	void Reset();

	uint GetNumUncachedMats()							{ return m_UncachedMatList.size(); }
	uint GetUncachedMatFlag(uint index)					{ return m_UncachedMatList[index].uncachedFlags; }
	uint GetUncachedMatType(uint index)					{ return m_UncachedMatList[index].matType; }
	IBFXMaterialGroup* GetUncachedMatGroup(uint index)	{ return m_UncachedMatList[index].pMatGroup; }

	IREffect* CreateEffect(IBFXMaterialGroup* pGroup, uint type, uint availability);
};

_DECLARE_SMARTPTR(AHMaterialCreateCallbackDeferred)

_NAMESPACE_END

#endif