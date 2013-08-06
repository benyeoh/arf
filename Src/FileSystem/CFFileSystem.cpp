//==============================================================================
//
//		CFFileSystem.cpp
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/23/2007
//
//==============================================================================

#include "FileSystemFramework.h"

_NAMESPACE_BEGIN

const wchar* 
CFFileSystem::FindMnemonic(const wchar* pMnemonic)
{
	std::map<wstring, wstring>::iterator itr = m_Mnemonics.find(pMnemonic);
	if(itr != m_Mnemonics.end())
	{
		return (*itr).second.c_str();
	}
	
	return NULL;
}

void 
CFFileSystem::ResolveMnemonics(const wchar* pPath, wstring& toStore)
{
	_DEBUG_ASSERT(pPath);
	toStore.clear();

	boolean startDelimiter = FALSE;
	
	uint i = 0;
	wstring currentMnemonic;
		
	while(pPath[i])
	{
		wchar c = pPath[i];

		if(startDelimiter)
		{
			// Keep searching for the end delimiter	
			
			if(c == MNEMONIC_END)
			{
				// Found...
				startDelimiter = FALSE;
				const wchar* pActual = FindMnemonic(currentMnemonic.c_str());
				if(pActual)
					toStore += pActual;
				
				currentMnemonic.clear();
			}
			else
			{
				// Add to the current mnemonic to resolve
				currentMnemonic.append(1, c);
			}
		}
		else
		if(c == MNEMONIC_START)
			startDelimiter = TRUE;
		else
			toStore.append(1, c);
		
		++i;
	}
}


void 
CFFileSystem::AddMnemonic(const wchar* pMnemonic, const wchar* pPath)
{	
	std::map<wstring, wstring>::iterator itr = m_Mnemonics.find(pMnemonic);
	if(itr == m_Mnemonics.end())
	{
		m_Mnemonics.insert(std::pair<wstring, wstring>(pMnemonic, pPath));
	}
	else
	{
		(*itr).second = pPath;
	}
}

void 
CFFileSystem::RemoveMnemonic(const wchar* pMnemonic)
{
	std::map<wstring, wstring>::iterator itr = m_Mnemonics.find(pMnemonic);
	if(itr != m_Mnemonics.end())
	{
		m_Mnemonics.erase(itr);
	}
}

void 
CFFileSystem::ClearMnemonics()
{
	m_Mnemonics.clear();
}

uint 
CFFileSystem::NormalizePath(const wchar* pFullPath, wchar* pNormPath, uint length)
{
	wstring culledPath = pFullPath;
	wstring toLowerCasePath = pFullPath;
	std::transform(toLowerCasePath.begin(), toLowerCasePath.end(), toLowerCasePath.begin(), towlower);

	wstring resourceLower = m_ResourcePath;
	std::transform(resourceLower.begin(), resourceLower.end(), resourceLower.begin(), towlower);
	
	// Cull out the application root
	int offset = (int) toLowerCasePath.find(resourceLower);
	if(offset != wstring::npos)
	{
		culledPath = culledPath.substr(offset + m_ResourcePath.size() + 1);
		toLowerCasePath = culledPath;
		std::transform(toLowerCasePath.begin(), toLowerCasePath.end(), toLowerCasePath.begin(), towlower);
	}

	// Cull out the mnemonic
	typedef std::map<wstring, wstring> MapString;	
	int curOffset = 0;
	int curLength = 0;
	wstring toReplace;
	
	_ITER_STLi(MapString, m_Mnemonics)
	{
		wstring mnemonic = (*iitr).first;
		wstring mnemonicPath = (*iitr).second;
		std::transform(mnemonicPath.begin(), mnemonicPath.end(), mnemonicPath.begin(), towlower);
		
		offset = (int) toLowerCasePath.find(mnemonicPath);
		if(offset != wstring::npos)
		{
			if((int) mnemonicPath.size() > curLength)
			{
				curOffset = offset;
				toReplace = mnemonic;
				curLength = (int) mnemonicPath.size();
				//break;
			}
//			culledPath.replace(offset, (*iitr).second.size(), MNEMONIC_START + mnemonic + MNEMONIC_END);
//			break;
		}		
	}
	
	if(curLength > 0)
	{
		culledPath.replace(curOffset, curLength, MNEMONIC_START + toReplace + MNEMONIC_END);	
	}
	
	length = (length == 0 || length > (uint) culledPath.size()) ? (uint) culledPath.size() : length;	
	memcpy(pNormPath, culledPath.c_str(), length * sizeof(wchar));
	pNormPath[length] = 0;
	return length;
}


boolean 
CFFileSystem::Initialize()
{
	return DoInitialize();
}

boolean 
CFFileSystem::Open(const wchar* pResourcePath)
{
	if(!m_IsOpen)
	{
		if(pResourcePath)
			m_ResourcePath = pResourcePath;
		else
			m_ResourcePath.clear();

		m_IsOpen = DoOpen();
		if(!m_IsOpen)
			m_ResourcePath.clear();
	}

	return m_IsOpen;
}

IFFile* 
CFFileSystem::GetFile(const wchar* pFileName)
{
	IFFile* toReturn = NULL;

	wstring resolvedName;
	ResolveMnemonics(pFileName, resolvedName);

	if(!m_IsOpen)
	{
		if(Open(NULL))
		{
			toReturn = DoGetFile(resolvedName.c_str());
			Close();
		}
	}
	else
		toReturn = DoGetFile(resolvedName.c_str());

	return toReturn;
}

void 
CFFileSystem::Close()
{
	if(m_IsOpen)
	{
		DoClose();
		m_IsOpen = FALSE;
		m_ResourcePath.clear();
	}
}

boolean
CFFileSystem::CheckFileExists(const wchar* pFileName)
{
	boolean toReturn = FALSE;

	wstring resolvedName;
	ResolveMnemonics(pFileName, resolvedName);
	
	if(!m_IsOpen)
	{
		if(Open(NULL))
		{
			toReturn = DoCheckFileExists(resolvedName.c_str());
			Close();
		}
	}
	else
		toReturn = DoCheckFileExists(resolvedName.c_str());

	return toReturn;

}

IFFileEnumerator*
CFFileSystem::GetFiles(const wchar* pHierarchyPath, const wchar* pFileFilter, boolean isIncludeSubHierachies)
{
	IFFileEnumerator* pToReturn = NULL;
	
	wstring resolvedName;
	ResolveMnemonics(pHierarchyPath, resolvedName);

	if(!m_IsOpen)
	{
		if(Open(NULL))
		{
			pToReturn = DoGetFiles(resolvedName.c_str(), pFileFilter, isIncludeSubHierachies);
			Close();
		}
	}
	else
		pToReturn = DoGetFiles(resolvedName.c_str(), pFileFilter, isIncludeSubHierachies);

	return pToReturn;	
}

const wchar * 
CFFileSystem::GetResourcePath() const
{
	return m_ResourcePath.c_str();
}


_NAMESPACE_END