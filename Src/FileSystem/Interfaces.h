//==============================================================================
//
//		Interfaces.h
//
//			A description here
//
//		Author:		Ben Yeoh
//		Date:		4/23/2007
//
//==============================================================================

#pragma once 


_NAMESPACE_BEGIN

const static wchar MNEMONIC_START = '[';
const static wchar MNEMONIC_END = ']';

enum eFileOpen
{
	FO_READ_ONLY,
	FO_WRITE_ONLY,
	FO_READ_WRITE,
};

enum eFileSeek
{
	FS_BEGIN,
	FS_END,
	FS_CUR,
};

struct IFFile : public IRefCounted
{
	_PURE( boolean Open(eFileOpen flag) )

	_PURE( uint Read(void* dst, uint length) )
	_PURE( uint Write(const void* src, uint length) )
	_PURE( uint Length() )
	_PURE( boolean Seek(uint offset, eFileSeek origin) )
	_PURE( uint Tell() )
	_PURE( const wchar* GetFileName() )
	_PURE( void Close() )
};

struct IFFileEnumerator : public IRefCounted
{
	_PURE( IFFile* GetNextFile() )
};

struct IFFileSystem : public IRefCounted
{
	_PURE( boolean Initialize() )
	
	_PURE( void AddMnemonic(const wchar* pMnemonic, const wchar* pPath) )
	_PURE( void RemoveMnemonic(const wchar* pMnemonic) )
	_PURE( void ClearMnemonics() )
	_PURE( uint NormalizePath(const wchar* pFullPath, wchar* pNormPath, uint length) )
	_PURE( uint ResolvePath(const wchar* pResourcePath, wchar* pFullPath, uint length) )

	_PURE( const wchar * GetResourcePath() const );
	
	_PURE( IFFile* GetFile(const wchar* pFileName) )

	_PURE( IFFileEnumerator* GetFiles(const wchar* pHierarchyPath, const wchar* pFileFilter, boolean isIncludeSubHierachies) )

	//_PURE( IFFileEnumerator* GetFiles(const wchar* pFilePath) )
	_PURE( boolean CheckFileExists(const wchar* pFileName) )
	_PURE( boolean Open(const wchar* pResourcePath) )	
	_PURE( void Close() )
};


//////////////////////////////////////////////////////////////////////////
//	
//	Smart Pointers
//
//////////////////////////////////////////////////////////////////////////

_DECLARE_SMARTPTR(IFFile)
_DECLARE_SMARTPTR(IFFileSystem)
_DECLARE_SMARTPTR(IFFileEnumerator)


//////////////////////////////////////////////////////////////////////////
//	
//	Inline Functions
//
//////////////////////////////////////////////////////////////////////////

template<> inline uint
AppendData<IFFile*>(IByteBuffer* pBuffer, IFFile* pFile)
{
	uint fileSize = pFile->Length();

	byte* pBufData = pBuffer->GetData();
	uint bufferSize = pBuffer->GetBufferSize();
	uint dataLength = pBuffer->GetDataLength();
	uint spaceLeft = bufferSize - dataLength;
	if(fileSize > spaceLeft)
	{
		uint newSize = NextPowOf2((fileSize - spaceLeft) + bufferSize);
		pBuffer->ResizeBuffer(newSize);	
		pBufData = pBuffer->GetData();
	}

	uint bytesRead = pFile->Read(pBufData + dataLength, fileSize);
	pBuffer->SetDataLength(dataLength+bytesRead);

	return bytesRead;
}

template<> inline uint
AppendData<IFFilePtr>(IByteBuffer* pBuffer, IFFilePtr pFile)
{
	return AppendData(pBuffer, pFile.GetPtr());
}

_NAMESPACE_END