/**
	@file BinaryFile.cpp
	@brief ���̳ʸ� ������ �ٷ�� ���� Ŭ������ ����
*/

#include "BinaryFile.h"
#include "FileMap.h"
#include <Windows.h>
#include <tchar.h>

/**
	@brief ������ �б��������� ����
	@param inFilePath ���� ���
	@return ���� ����
*/
bool BinaryFile::openForRead(const std::wstring& inFilePath)
{
	fileHandle_ = ::CreateFile(inFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (inFilePath.length() > 4 && inFilePath.substr(0, 4) == _T("\\\\.\\"))
	{
		isDeviceFile_ = true;
	}
	else
	{
		isDeviceFile_ = false;
	}

	if (fileHandle_ != INVALID_HANDLE_VALUE)
	{
		DWORD fileSizeHigh;
		DWORD fileSizeLow;

		mmapHandle_ = ::CreateFileMapping(fileHandle_, NULL, PAGE_READONLY, 0, 0, NULL);

		fileSizeLow = ::GetFileSize(fileHandle_, &fileSizeHigh);

		fileSize_ = (((uint64_t)fileSizeHigh) << 32) | ((uint64_t)fileSizeLow);
	}

	return (fileHandle_ != INVALID_HANDLE_VALUE) && (mmapHandle_ != INVALID_HANDLE_VALUE);
}

/**
	@brief ������ ������������ ����
	@param inFilePath ���� ���
	@return ���� ����
*/
bool BinaryFile::openForWrite(const std::wstring& inFilePath)
{
	fileHandle_ = ::CreateFile(inFilePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	mmapHandle_ = INVALID_HANDLE_VALUE;

	return fileHandle_ != INVALID_HANDLE_VALUE;
}

/**
	@brief ������ �����͸� �̵��Ѵ�
	@param inPos �̵��� ��ġ
*/
void BinaryFile::setFilePointer64(uint64_t inPos)
{
	LONG seekMoveHigh = inPos >> 32;

	::SetFilePointer(fileHandle_, inPos & 0xffffffff, &seekMoveHigh, SEEK_SET);
}

/**
	@brief ������ ũ�⸦ ���´�
	@return ������ ũ��
*/
uint64_t BinaryFile::getFileSize()
{
	return fileSize_;
}

/**
	@brief ������ ������ �о�´�
	
	@param inPos ���� ��ġ
	@param outBuffer ���� ������ ��� ����
	@param inReadSize ���� ������

	@return ���� ������
*/
DWORD BinaryFile::readData(uint64_t inPos, void* outBuffer, DWORD inReadSize)
{
	DWORD readSize = 0;

	setFilePointer64(inPos);

	::ReadFile(fileHandle_, outBuffer, inReadSize, &readSize, NULL);

	return readSize;
}

/**
	@brief ���Ͽ� ������ ����Ѵ�
	
	@param inPos ����� ��ġ
	@param inBuffer ����� ������ ��� ����
	@param inWriteSize ����� ������

	@return ���� ����� ������
*/
DWORD BinaryFile::writeData(uint64_t inPos, const void* inBuffer, DWORD inWriteSize)
{
	DWORD writeSize = 0;

	setFilePointer64(inPos);

	::WriteFile(fileHandle_, inBuffer, inWriteSize, &writeSize, NULL);

	return writeSize;
}

/**
	@brief ������ �ݴ´�
	@return ���� ����
*/
bool BinaryFile::closeFile()
{
	if (mmapHandle_ != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(mmapHandle_);
		return  ::CloseHandle(fileHandle_) == TRUE;
		
	}
	else
	{
		return ::CloseHandle(fileHandle_) == TRUE;
	}
}

/**
	@brief ���Ͽ� ���� �޸� ���� �����

	@param inPos �޸� ���� ���۵� ��ġ
	@param inSize �޸� ���� ������

	@return ������ ���� �� Ŭ����
*/
FileMap BinaryFile::createFileMap(uint64_t inPos, uint64_t inSize)
{
	if (isDeviceFile_)
	{
		if (inPos + inSize > fileSize_)
		{
			return FileMap(fileHandle_, inPos, fileSize_ - inPos, true);
		}

		return FileMap(fileHandle_, inPos, inSize, true);
	}
	
	if (inPos + inSize > fileSize_)
	{
		return FileMap(mmapHandle_, inPos, fileSize_ - inPos);
	}

	return FileMap(mmapHandle_, inPos, inSize);
}