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
	std::vector<SGSParameter> m_Parameters;

public:
	SGSScript();
	~SGSScript();

public:
	uint GetNumOfShaders() const { return m_Shaders.size(); }
	SGSShader* GetShader(uint index) { return &m_Shaders[index]; }
	const SGSShader* GetShader(uint index) const { return &m_Shaders[index]; }
	void AddShader(SGSShader& shader) { m_Shaders.push_back(shader); }

	uint GetNumOfTechniques() const { return m_Techniques.size(); }
	SGSTechnique* GetTechnique(uint index) { return &m_Techniques[index]; }
	const SGSTechnique* GetTechnique(uint index) const { return &m_Techniques[index]; }
	void AddTechnique(SGSTechnique& tech) { m_Techniques.push_back(tech); }

	uint GetNumOfParameters() const { return m_Parameters.size(); }
	SGSParameter* GetParameter(uint index) { return &m_Parameters[index]; }
	const SGSParameter* GetParameter(uint index) const { return &m_Parameters[index]; }
	void AddParameter(SGSParameter& param) { m_Parameters.push_back(param); }
};

_NAMESPACE_END