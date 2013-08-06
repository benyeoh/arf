//==============================================================================
//
//		CBFXBaseFX.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/23/2008
//
//==============================================================================

#include "BaseFXFramework.h"

_NAMESPACE_BEGIN

boolean 
CBFXBaseFX::Initialize(IRRenderer& renderer, IMGMaterialGen& matGen, IAppCallback& callback)
{
	// TODO: Should we re-initialize it every time?
	if(!m_pRenderer)
	{
		m_pRenderer = &renderer;
		m_pCallback = &callback;
		m_pMatGen = &matGen;

		m_ResMgr.SetBaseFX(this);
		//m_ParamPool.SetBaseFX(this);
		//SetParamCallback(&m_ParamPool, BFX_EFFECT_PARAM_OFFSET);
	
		//m_Pipeline.SetBaseFX(this);
		//m_Pipeline.Initialize();
		//SetRenderCallback(&m_Pipeline, BFX_MAT_BASE_LIGHTING);
		
		m_Debug.SetBaseFX(this);
		m_Debug.Initialize();
		
		return TRUE;
	}
	
	return FALSE;	
}
//
//void
//CBFXBaseFX::GetDynamicParams(IREffect* pEffect, uint tech, REffectParam* pStore)
//{
//    IREffectTemplate* pTemplate = pEffect->GetTemplate();
//    uint numDynamicParams = pTemplate->GetNumOfDynamicParams(tech);
//    _LOOPi(numDynamicParams)
//    {
//        uint semantic		= pTemplate->GetDynamicParamSemantic(tech, i);
//		uint callbackIndex	= semantic >> BFX_MAX_SEMANTICS_SHIFT;
//
//		IBFXParamCallback* pParamCallback = m_pParamCallback[callbackIndex];
//		pParamCallback->GetParam(semantic, &pStore[i]);
//    }
//}

void CBFXBaseFX::Render(BFXMesh& mesh, float detailLevel, uint matType, uint pipelineMatType, BFXParamContainer& paramContainer, BFXRenderContainer& renderContainer, IBFXMaterialCreateCallback* pCreateCallback)
{
	CBFXMaterialGroup* pGroup = (CBFXMaterialGroup*)(IBFXMaterialGroup*) mesh.pMatGroup;

	_DEBUG_ASSERT(pGroup->IsMatTypeValid(matType));

	//if(pGroup->IsMatTypeValid(matType))
	{
		uint type = matType;//pGroup->GetCurrentMatType();
		//m_MaterialRenderCallbacks[type]->BeginRenderEffect(mesh);

		uint usedCallbackFlags[4];
		usedCallbackFlags[0] = usedCallbackFlags[1] = 0;
		usedCallbackFlags[2] = usedCallbackFlags[3] = 0;

		IBFXParamCallback* pAllUsedCallbacks[BFX_MAX_PARAM_CALLBACKS];
		uint numUsedCallbacks = 0;

		BFXRenderContext context;
		context.matType		= pipelineMatType;
		context.pMesh		= &mesh;
		//context.iUserData	= 0;
		context.pContainer	= &paramContainer;
		context.numIters	= 0;

		uint renderLib = _GET_LIB_INDEX_FROM_MAT_TYPE(pipelineMatType);

		// Keep rendering the same object if it requires more passes
		// This is determined by the param callback
		boolean isMorePassesRequired = FALSE;
		do 
		{
			isMorePassesRequired = FALSE;
			REffectParam params[RRenderOp::MAX_NUM_PARAMS];

			// Resolve the effect

			// Find all volatile parameters and fetch the effect
			uint availabilityFlag = pGroup->GetParamAvailability(type, paramContainer);
			IREffect* pEffect = pGroup->GetEffect(type, availabilityFlag);
			if(!pEffect)
			{
				if(pCreateCallback)
					pEffect = pCreateCallback->CreateEffect(pGroup, type, availabilityFlag);
				else
				{
					// Immediately create effect
					pEffect = pGroup->CreateEffect(type, availabilityFlag);
				}
			}

			//_DEBUG_ASSERT(pEffect);

			if(pEffect)
			{
				// Resolve all dynamic params for this effect
				uint tech = (uint) FastTruncate((pEffect->GetNumOfTechs()-1) * detailLevel + 0.5f);
				uint numDynamicParams = pEffect->GetTemplate()->GetNumOfDynamicParams(tech);
				_LOOPi(numDynamicParams)
				{
					uint semantic	= pEffect->GetTemplate()->GetDynamicParamSemantic(tech, i);
					uint libIndex	= _GET_LIB_INDEX(semantic);
					IBFXParamCallback* pParamCallback = paramContainer.GetParamCallback(libIndex);
					pParamCallback->GetParam(semantic, &params[i]);

					// Store temp parameter callbacks for each parameter
					// (To reset later)					
					uint flagOffset		= 1 << (libIndex & 31);
					uint indexOffset	= libIndex >> 5;
					if((usedCallbackFlags[indexOffset] & flagOffset) == 0)
					{
						usedCallbackFlags[indexOffset]	|= flagOffset;
						pAllUsedCallbacks[numUsedCallbacks] = pParamCallback;
						numUsedCallbacks++;
					}
				}

				// Then resolve all instanced params
				uint numInstancedParams = pEffect->GetTemplate()->GetNumOfInstancedParams(tech);
				_LOOPi(numInstancedParams)
				{
					uint semantic	= pEffect->GetTemplate()->GetInstancedParamSemantic(tech, i);
					uint libIndex	= _GET_LIB_INDEX(semantic);
					IBFXParamCallback* pParamCallback = paramContainer.GetParamCallback(libIndex);
					pParamCallback->GetParam(semantic, &params[i+numDynamicParams]);

					uint flagOffset		= 1 << (libIndex & 31);
					uint indexOffset	= libIndex >> 5;
					if((usedCallbackFlags[indexOffset] & flagOffset) == 0)
					{
						usedCallbackFlags[indexOffset]	|= flagOffset;
						pAllUsedCallbacks[numUsedCallbacks] = pParamCallback;
						numUsedCallbacks++;
					}
				}

				// Finally add the object to the render pipeline
				context.pEffect			= pEffect;
				context.tech			= tech;
				context.pDynamicParams	= params;
				context.numParams		= numDynamicParams + numInstancedParams;

				_DEBUG_ASSERT(numUsedCallbacks <= BFX_MAX_PARAM_CALLBACKS);
				_DEBUG_ASSERT(context.numParams <= RRenderOp::MAX_NUM_PARAMS);

				IBFXRenderCallback* pRenderCallback = renderContainer.GetRenderCallback(renderLib);
				pRenderCallback->RenderEffect(context);
			}

			// For each callback used, check to see if more passes are required for this object
			_LOOPi(numUsedCallbacks)
			{
				if(pAllUsedCallbacks[i]->IsMorePassesRequired())
				{
					isMorePassesRequired = TRUE;
					break;
				}
			}

			context.numIters++;
		} while(isMorePassesRequired);

		// Notify all param callbacks that effect is finished resolving for all passes
		// (To reset states if needed)
		_LOOPi(numUsedCallbacks)
		{
			pAllUsedCallbacks[i]->FinishedResolvingEffect();
		}
	}
}

//void
//CBFXBaseFX::Render(BFXMesh& mesh, float detailLevel, uint matType, uint libIndex,
//				   BFXParamContainer& paramContainer, BFXRenderContainer& renderContainer,
//				   uint* pUncachedAvailFlags, uint& numUncachedFlags)
//{
//	CBFXMaterialGroup* pGroup = (CBFXMaterialGroup*)(IBFXMaterialGroup*) mesh.pMatGroup;
//
//	uint uncachedFlagSize = pUncachedAvailFlags ? numUncachedFlags : 0;
//
//	_DEBUG_ASSERT(pGroup->IsMatTypeValid(matType));
//
//	//if(pGroup->IsMatTypeValid(matType))
//	{
//		uint type = matType;//pGroup->GetCurrentMatType();
//		//m_MaterialRenderCallbacks[type]->BeginRenderEffect(mesh);
//	
//		uint usedCallbackFlags[4];
//		usedCallbackFlags[0] = usedCallbackFlags[1] = 0;
//		usedCallbackFlags[2] = usedCallbackFlags[3] = 0;
//		
//		IBFXParamCallback* pAllUsedCallbacks[BFX_MAX_PARAM_CALLBACKS];
//		uint numUsedCallbacks = 0;
//
//		BFXRenderContext context;
//		context.matType		= type;
//		context.pMesh		= &mesh;
//		//context.iUserData	= 0;
//		context.pContainer	= &paramContainer;
//		context.numIters	= 0;
//
//		uint renderLib = libIndex;
//
//		// Keep rendering the same object if it requires more passes
//		// This is determined by the param callback
//		boolean isMorePassesRequired = FALSE;
//		do 
//		{
//			isMorePassesRequired = FALSE;
//			REffectParam params[RRenderOp::MAX_NUM_PARAMS];
//			
//			// Resolve the effect
//
//			// Find all volatile parameters and fetch the effect
//			uint availabilityFlag = pGroup->GetParamAvailability(type, paramContainer);
//			IREffect* pEffect = pGroup->GetEffect(type, availabilityFlag);
//
//			if(pEffect != NULL)
//			{
//				// Resolve all dynamic params for this effect
//				uint tech = (uint) FastTruncate((pEffect->GetNumOfTechs()-1) * detailLevel + 0.5f);
//				uint numDynamicParams = pEffect->GetTemplate()->GetNumOfDynamicParams(tech);
//				_LOOPi(numDynamicParams)
//				{
//					uint semantic	= pEffect->GetTemplate()->GetDynamicParamSemantic(tech, i);
//					uint libIndex	= _GET_LIB_INDEX(semantic);
//					IBFXParamCallback* pParamCallback = paramContainer.GetParamCallback(libIndex);
//					pParamCallback->GetParam(semantic, &params[i]);
//
//					// Store temp parameter callbacks for each parameter
//					// (To reset later)					
//					uint flagOffset		= 1 << (libIndex & 31);
//					uint indexOffset	= libIndex >> 5;
//					if((usedCallbackFlags[indexOffset] & flagOffset) == 0)
//					{
//						usedCallbackFlags[indexOffset]	|= flagOffset;
//						pAllUsedCallbacks[numUsedCallbacks] = pParamCallback;
//						numUsedCallbacks++;
//					}
//				}
//
//				// Then resolve all instanced params
//				uint numInstancedParams = pEffect->GetTemplate()->GetNumOfInstancedParams(tech);
//				_LOOPi(numInstancedParams)
//				{
//					uint semantic	= pEffect->GetTemplate()->GetInstancedParamSemantic(tech, i);
//					uint libIndex	= _GET_LIB_INDEX(semantic);
//					IBFXParamCallback* pParamCallback = paramContainer.GetParamCallback(libIndex);
//					pParamCallback->GetParam(semantic, &params[i+numDynamicParams]);
//
//					uint flagOffset		= 1 << (libIndex & 31);
//					uint indexOffset	= libIndex >> 5;
//					if((usedCallbackFlags[indexOffset] & flagOffset) == 0)
//					{
//						usedCallbackFlags[indexOffset]	|= flagOffset;
//						pAllUsedCallbacks[numUsedCallbacks] = pParamCallback;
//						numUsedCallbacks++;
//					}
//				}
//
//				// Finally add the object to the render pipeline
//				context.pEffect			= pEffect;
//				context.tech			= tech;
//				context.pDynamicParams	= params;
//				context.numParams		= numDynamicParams + numInstancedParams;
//
//				_DEBUG_ASSERT(numUsedCallbacks <= BFX_MAX_PARAM_CALLBACKS);
//				_DEBUG_ASSERT(context.numParams <= RRenderOp::MAX_NUM_PARAMS);
//
//				IBFXRenderCallback* pRenderCallback = renderContainer.GetRenderCallback(renderLib);
//				pRenderCallback->RenderEffect(context);
//			}
//			else
//			{
//				// The effect is not yet compiled/cached
//				if(uncachedFlagSize > 0)
//				{
//					// Return the flags for which there is no effect shader
//					pUncachedAvailFlags[numUncachedFlags - uncachedFlagSize] = availabilityFlag;
//					uncachedFlagSize--;
//				}
//			}
//
//			// For each callback used, check to see if more passes are required for this object
//			_LOOPi(numUsedCallbacks)
//			{
//				if(pAllUsedCallbacks[i]->IsMorePassesRequired())
//				{
//					isMorePassesRequired = TRUE;
//					break;
//				}
//			}
//			
//			context.numIters++;
//		} while(isMorePassesRequired);
//
//		// Notify all param callbacks that effect is finished resolving for all passes
//		// (To reset states if needed)
//		_LOOPi(numUsedCallbacks)
//		{
//			pAllUsedCallbacks[i]->FinishedResolvingEffect();
//		}
//	}
//
//	numUncachedFlags = numUncachedFlags - uncachedFlagSize;
//}

//void 
//CBFXBaseFX::SetParamCallback(IBFXParamCallback* pModule, uint offsetStart)
//{
//	uint index = offsetStart >> BFX_MAX_SEMANTICS_SHIFT;
//	m_pParamCallback[index] = pModule;
//
//	uint size = (uint) m_AllParamCallbacks.size();
//	_LOOPi(size)
//	{
//		if(m_AllParamCallbacks[i] == pModule)
//			return;
//	}
//
//	m_AllParamCallbacks.push_back(pModule);
//}

//void 
//CBFXBaseFX::SetRenderCallback(IBFXRenderCallback* pModule, uint materialType)
//{
//	m_pMaterialRenderCallbacks[materialType] = pModule;	
//}

//IBFXParamCallback* 
//CBFXBaseFX::GetParamCallback(uint semantic)
//{
//	uint index = semantic >> BFX_MAX_SEMANTICS_SHIFT;
//	return m_pParamCallback[index];
//}

//void 
//CBFXBaseFX::BeginRender()
//{
//	//uint size = (uint) m_AllParamCallbacks.size();
//	//_LOOPi(size)
//	//{
//	//	m_AllParamCallbacks[i]->ResetParams();
//	//}
//}

//void
//CBFXBaseFX::EndRender()
//{
//	//IRPipeline& pipeline = m_pRenderer->GetRPipeline();
//
//	//// Setup pipeline so the offscreen buffer is cleared first	
//	//uint prelimGroup = pipeline.CreateNewRenderGroup(BFX_GROUP_MAIN_SETUP, NULL, m_Pipeline);
//	//////pipeline.AddRenderTarget(prelimGroup, m_pMainRT);
//	//pipeline.SetClearColor(prelimGroup, gmtl::Vec4f(0.35f, 0.35f, 0.35f, 1.0f));
//	//if(m_Pipeline.GetMainGroupDepthClear())
//	//	pipeline.SetClearDepth(prelimGroup, 1.0f);
//
//	//// Reset every frame
//	//m_Pipeline.SetMainGroupDepthClear(TRUE);
//}


_NAMESPACE_END