// TestChainedHashMap.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

_USE_NAMESPACE

//#define _TEST_REMOVAL
#define _HASH_OBJ	HashObjWeak
#define MAX_AMOUNT	INT_MAX

const static uint	INIT_ADD_SIZE	= 2000000;
const static uint	OP_SIZE			= 16384;
const static uint	SAMPLE_SIZE		= 128;

const static float	ADD_CHANCE		= 0.0f;
const static float	DEL_CHANCE		= 0.0f;

const static float	EXP_RATE		= 0.667f;

uint MurmurHashPos(uint* gridPos)
//const void* key, int len, unsigned int seed )
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const uint m = 0x5bd1e995;
	const int r = 24;
	const int init = 1234567 ^ 12;

	// Initialize the hash to a 'random' value
	uint h = init;
	
	// Mix 4 bytes at a time into the hash
	//const unsigned char * data = (const unsigned char *)key;

	//_LOOPi(3)
	{
		uint k = gridPos[0]; // *(unsigned int *)data;
		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;

		k = gridPos[1];
		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;

		k = gridPos[2];		
		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;
		//data += 4;
		//len -= 4;
	}
	
	// Handle the last few bytes of the input array

	//switch(len)
	//{
	//case 3: h ^= data[2] << 16;
	//case 2: h ^= data[1] << 8;
	//case 1: h ^= data[0];
	//        h *= m;
	//};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
} 

uint MurmurHashUint(uint value)
//const void* key, int len, unsigned int seed )
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const uint m = 0x5bd1e995;
	const int r = 24;
	const int init = 1234567 ^ 12;

	// Initialize the hash to a 'random' value
	uint h = init;

	{
		uint k = value; // *(unsigned int *)data;

		k *= m; 
		k ^= k >> r; 
		k *= m; 
		
		h *= m; 
		h ^= k;
	}

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
} 

enum 
{
	ADD_OP,
	REMOVE_OP,
	GET_OP
};

struct Key
{
	uint actual[3];
	
	Key()
	{
	}
	
	bool operator == (const Key& toCompare) const
	{
		return actual[0] == toCompare.actual[0] && actual[1] == toCompare.actual[1] && actual[2] == toCompare.actual[2];
	}
};

struct Op
{
	Key key;
	uint type;
};

struct HashInt
{
	HashInt() {}
	uint operator()(const uint& val) const 
	{ 
		return val; 
	}
};

struct HashObj
{
	HashObj() {}
	uint operator()(const Key& val) const 
	{ 
		//if(!val.isUsed)
		{
			//((Key*)(&val))->hashVal = MurmurHashPos((uint*) val.actual);//MurmurHashUint(val.actual);
			//((Key*)(&val))->isUsed = TRUE;
		}
		
		return MurmurHashPos((uint*) val.actual);//val.hashVal; 
	}
};

struct HashObjLookup
{
	HashObjLookup() {}
	uint operator()(const Key& val) const 
	{ 
		//if(!val.isUsed)
		{
			//((Key*)(&val))->hashVal = Lookup3HashFn(val.actual, 3, 7);
			//((Key*)(&val))->isUsed = TRUE;
		}
		
		return Lookup3HashFn(val.actual, 3, 7);//val.hashVal; 
	}
};

struct HashObjWeak
{
	HashObjWeak() {}
	uint operator()(const Key& val) const 
	{ 
		//if(!val.isUsed)
		{
			//((Key*)(&val))->hashVal = val.actual[0] * 831177173 + val.actual[2] * 128812212 + val.actual[3] * 739921;
			//((Key*)(&val))->isUsed = TRUE;
		}
		
		return val.actual[0] * 831177173 + val.actual[1] * 12881221 + val.actual[2] * 739921;//val.hashVal; 
	}
};

struct HashObjSuperWeak
{
	HashObjSuperWeak() {}
	uint operator()(const Key& val) const 
	{ 
		//if(!val.isUsed)
		{
			//((Key*)(&val))->hashVal = val.actual[0] + val.actual[1] + val.actual[2];
			//((Key*)(&val))->isUsed = TRUE;
		}
		
		return val.actual[0] + val.actual[1] + val.actual[2];//val.hashVal; 
		
	}
};

ICMMemAllocatorPtr g_pAllocator;

_DECL_CORE_MEM_ALLOC_FNS(g_pAllocator)

int _tmain(int argc, _TCHAR* argv[])
{
	Module hCoreMem	= InitializeModule(_W("CoreMemory.dll"), NULL);
	
	CreateMemAllocatorFn CallCreateMemAllocator = (CreateMemAllocatorFn) GetFunction(hCoreMem, "CreateMemAllocator");
	g_pAllocator = CallCreateMemAllocator();

	_DEBUG_ASSERT(g_pAllocator);
	MemAllocFns allocFns = GetCoreMemoryBindings();
	_BIND_TO_GLOBAL_ALLOC(allocFns);

	Module hPlatform	= InitializeModule(_W("PlatformWin32.dll"), &allocFns);
	
	CreatePlatformFn CreatePlatform = (CreatePlatformFn) GetFunction(hPlatform, "CreatePlatform");
	IPPlatformPtr pPlatform = CreatePlatform();

	{			
		gmtl::Math::seedRandom( (uint) pPlatform->GetTimer().GetTime() );
		std::vector<Op> store;

		dense_hash_map<Key, uint, _HASH_OBJ> denseMap;
		Key empty;
		empty.actual[0] = 0xFFFFFFFF;
		empty.actual[1] = 0xFFFFFFFF;
		empty.actual[2] = 0xFFFFFFFF;
		denseMap.set_empty_key(empty);
		empty.actual[0] = 0xFFFFFFFE;
		empty.actual[1] = 0xFFFFFFFE;
		empty.actual[2] = 0xFFFFFFFE;
		denseMap.set_deleted_key(empty);
		
		DenseChainedHashMap<Key, uint, _HASH_OBJ> chainMap(32, EXP_RATE);
		
		_LOOPi(INIT_ADD_SIZE)
		{
			_HASH_OBJ hasher;
			Op op;
			op.key.actual[0] = (uint)(gmtl::Math::unitRandom() * MAX_AMOUNT);
			op.key.actual[1] = (uint)(gmtl::Math::unitRandom() * MAX_AMOUNT);
			op.key.actual[2] = (uint)(gmtl::Math::unitRandom() * MAX_AMOUNT);
			hasher(op.key);
			op.type = ADD_OP;
			store.push_back(op);
		}
		
		_TRACE(_W("------------------------- Adding ----------------------- \n"));
		
		pPlatform->GetTimer().BeginSample();
		_LOOPi(INIT_ADD_SIZE)
		{
			chainMap.Add(store[i].key, store[i].key.actual[0]);
		}
		pPlatform->GetTimer().EndSample();

#ifdef _TEST_REMOVAL
		_TRACE(_W("------------------------- Removing ----------------------- \n\n"));
		_LOOPi(INIT_ADD_SIZE-2)
		{
			chainMap.Remove(store[i]);
		}		

		DenseChainedHashMap<Key, uint, _HASH_OBJ>::Iterator iter = chainMap.BeginIter();
		uint i = 0;
		while(iter.IsValid())
		{
			_TRACE(_W("i: %d, hash: %d, actual: %d, val: %d\n"), i, (iter.GetKey())->hashVal, (iter.GetKey())->actual, *(iter.GetValue()));
			iter.Next();
			i++;
		}
#else
		_TRACE(_W("chainMap >>>>>>>>>>>>>>>>>> Total Time Elapsed: %f\n"), pPlatform->GetTimer().GetTimeElapsed());
		
		pPlatform->GetTimer().BeginSample();
		_LOOPi(INIT_ADD_SIZE)
		{
			denseMap[store[i].key] = store[i].key.actual[0];
		}

		pPlatform->GetTimer().EndSample();
		_TRACE(_W("denseMap !!!!!!!!!!!!!!!!! Total Time Elapsed: %f\n\n"), pPlatform->GetTimer().GetTimeElapsed());
		
		double timeChain = 0.0;
		double timeDense = 0.0;
		uint checkSumChain	= 0;
		uint checkSumDense	= 0;
		_LOOPk(SAMPLE_SIZE)
		{
			std::vector<Op> ops;
			//store.clear();
			_LOOPi(OP_SIZE)
			{
				_HASH_OBJ hasher;
				Op op;
				
				float chance = gmtl::Math::unitRandom();
				if(chance < DEL_CHANCE)
				{
					uint index = (uint)(gmtl::Math::unitRandom() * store.size());
					if(index < 0)
						index = 0;
					if(index >= store.size())
						index = store.size() - 1;
					Op& storeOp = store[index];
					op.key.actual[0] = storeOp.key.actual[0];
					op.key.actual[1] = storeOp.key.actual[1];
					op.key.actual[2] = storeOp.key.actual[2];
					hasher(op.key);
					op.type = REMOVE_OP;
					
					//store.erase( store.begin() + index );
					//op.key.actual[0] = (uint)(gmtl::Math::unitRandom() * MAX_AMOUNT);
					//op.key.actual[1] = (uint)(gmtl::Math::unitRandom() * MAX_AMOUNT);
					//op.key.actual[2] = (uint)(gmtl::Math::unitRandom() * MAX_AMOUNT);
					//hasher(op.key);
					//op.type = REMOVE_OP;
				}
				else
				if(chance < (DEL_CHANCE + ADD_CHANCE))
				{
					op.key.actual[0] = (uint)(gmtl::Math::unitRandom() * MAX_AMOUNT);
					op.key.actual[1] = (uint)(gmtl::Math::unitRandom() * MAX_AMOUNT);
					op.key.actual[2] = (uint)(gmtl::Math::unitRandom() * MAX_AMOUNT);
					hasher(op.key);
					op.type = ADD_OP;
					
					//store.push_back(op);
				}
				else
				{
					uint index = (uint)(gmtl::Math::unitRandom() * store.size());
					if(index < 0)
						index = 0;
					if(index >= store.size())
						index = store.size() - 1;
					Op& storeOp = store[index];
					op.key.actual[0] = storeOp.key.actual[0];
					op.key.actual[1] = storeOp.key.actual[1];
					op.key.actual[2] = storeOp.key.actual[2];
					hasher(op.key);
					op.type = GET_OP;
				}	
				
				ops.push_back(op);
			}
			
			_TRACE(_W("------------------------- Ops ----------------------- \n"));
			uint fakeSum = 0;
			
			pPlatform->GetTimer().BeginSample();
			_LOOPi(OP_SIZE)
			{
				Op& op = ops[i];
				switch(op.type)
				{
					case ADD_OP:	chainMap.Add(op.key, op.key.actual[0]);	break;
					case REMOVE_OP:	chainMap.Remove(op.key);				break;
					case GET_OP:
					{
						uint* pVal = chainMap.Get(op.key);
						if(pVal)
							fakeSum += *pVal;
						break;
					}	
				}
			}
			
			pPlatform->GetTimer().EndSample();
			_TRACE(_W("chainMap >>>>>>>>>>>>>>>>>> Total Time Elapsed: %f\n"), pPlatform->GetTimer().GetTimeElapsed());
			_TRACE(_W("chainMap >>>>>>>>>>>>>>>>>> fakeSum: %d\n"), fakeSum);
			
			timeChain += pPlatform->GetTimer().GetTimeElapsed();
			checkSumChain += fakeSum;
			
			fakeSum = 0;
			pPlatform->GetTimer().BeginSample();
			_LOOPi(OP_SIZE)
			{
				Op& op = ops[i];
				switch(op.type)
				{
					case ADD_OP:	denseMap[op.key] = op.key.actual[0];	break;
					case REMOVE_OP:	denseMap.erase(op.key);					break;
					case GET_OP:
					{
						dense_hash_map<Key, uint, _HASH_OBJ>::iterator iter = denseMap.find(op.key);
						if(iter != denseMap.end())
						{
							fakeSum += iter->second;
						}
						break;
					}	
				}
			}
			
			pPlatform->GetTimer().EndSample();
			_TRACE(_W("denseMap !!!!!!!!!!!!!!!!!! Total Time Elapsed: %f\n"), pPlatform->GetTimer().GetTimeElapsed());
			_TRACE(_W("denseMap !!!!!!!!!!!!!!!!!! fakeSum: %d\n\n"), fakeSum);	
			
			timeDense += pPlatform->GetTimer().GetTimeElapsed();
			checkSumDense += fakeSum;
		}	
		
		_TRACE(_W("------------------------- SUMMARY ----------------------- \n"));
		_TRACE(_W("chainMap >>>>>>>>>>>>>>>>>> Average: %f\n"), timeChain / SAMPLE_SIZE);
		_TRACE(_W("chainMap >>>>>>>>>>>>>>>>>> checkSum: %d\n"), checkSumChain);
		_TRACE(_W("denseMap !!!!!!!!!!!!!!!!!! Average: %f\n"), timeDense / SAMPLE_SIZE);
		_TRACE(_W("denseMap !!!!!!!!!!!!!!!!!! checkSum: %d\n\n"), checkSumDense);	
			
#endif				
		
		//getchar();
	}		

	pPlatform = NULL;
	
	g_pAllocator->DumpLeaks();
	g_pAllocator = NULL;

	ShutdownModule(hPlatform);
	ShutdownModule(hCoreMem);
	
	_DUMP_LEAKS;
	
	return 0;
}

