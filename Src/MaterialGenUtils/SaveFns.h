//==============================================================================
//
//		LoadSaveFns.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/13/2008
//
//==============================================================================

#pragma once 

_NAMESPACE_BEGIN

uint 
SaveMaterialTemplate(IMGMaterialTemplate* pTemplate, IByteBuffer* pBuffer);

uint
SaveLocal(IMGLocal* pLocal, IByteBuffer* pBuffer);

uint
SaveGlobal(IMGGlobal* pGlobal, IByteBuffer* pBuffer);

uint
SaveTechnique(IMGTechnique* pTech, IByteBuffer* pBuffer);

uint
SavePass(IMGPass* pPass, IByteBuffer* pBuffer);

uint
SaveVertexShader(IMGVertexShader* pShader, IByteBuffer* pBuffer);

uint
SavePixelShader(IMGPixelShader* pShader, IByteBuffer* pBuffer);

uint
SaveRenderState(IMGRenderState* pRenderState, IByteBuffer* pBuffer);

uint
SaveFunction(IMGFunction* pFunction, IByteBuffer* pBuffer);

uint
SaveFunctionOp(IMGFunctionOp* pOp, IByteBuffer* pBuffer);

uint
SaveVertexIn(IMGVertexIn* pIn, IByteBuffer* pBuffer);

uint
SaveVertexOut(IMGVertexOut* pOut, IByteBuffer* pBuffer);

uint
SavePixelIn(IMGPixelIn* pIn, IByteBuffer* pBuffer);

uint
SavePixelOut(IMGPixelOut* pOut, IByteBuffer* pBuffer);

_NAMESPACE_END