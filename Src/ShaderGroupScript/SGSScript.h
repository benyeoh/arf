//==============================================================================
//
//		Script.h
//
//			A description here
//
//		Author: 	Ben Yeoh
//		Date:		Sunday, 13 April, 2014
//
//==============================================================================

#pragma once

_NAMESPACE_BEGIN

class SGSScript
{
private:
	std::vector<SGSShader> m_Shaders;
	std::vector<SGSTechnique> m_Techniques;
	std::vector<SGSAnnotation> m_Annotations;

public:
	SGSScript();
	~SGSScript();

public:
	uint GetNumOfShaders() { return m_Shaders.size(); }
	SGSShader* GetShader(uint index) { return &m_Shaders[index]; }
	void AddShader(SGSShader& shader) { m_Shaders.push_back(shader); }

	uint GetNumOfTechniques() { return m_Techniques.size(); }
	SGSTechnique* GetTechnique(uint index) { return &m_Techniques[index]; }
	void AddTechnique(SGSTechnique& tech) { m_Techniques.push_back(tech); }

	uint GetNumOfAnnotations() { return m_Annotations.size(); }
	SGSAnnotation* GetAnnotation(uint index) { return &m_Annotations[index]; }
	void AddAnnotation(SGSAnnotation& annot) { m_Annotations.push_back(annot); }
};

_NAMESPACE_END