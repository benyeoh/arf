//==============================================================================
//
//		HelperFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		7/11/2008
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

uint 
GetCommonParams(IBFXMaterialGroupTemplate* pTemplate, uint* pParamIndices, uint* pMatIndices, uint paramBufferSize=0);

void
AppendVertexDesc(IBFXMaterialGroupTemplate* pTemplate, RVertexDesc* pDesc);

void
ComputeBFXMatGroupTemplateDataKey(IBFXMaterialGroupTemplate* pTemplate, REffectParam* pParams, uint numParams, CRCDataKey& key);

_NAMESPACE_END