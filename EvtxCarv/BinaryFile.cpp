/**
	@file BinaryFile.cpp
	@brief 바이너리 파일을 다루기 위한 클래스의 구현
*/

#include "BinaryFile.h"
#include "FileMap.h"
#include <Windows.h>
#include <tchar.h>

/**
	@brief 파일을 읽기전용으로 연다
	@param inFilePath 파일 경로
	@return 성공 여부
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
	@brief 파일을 쓰기전용으로 연다
	@param inFilePath 파일 경로
	@return 성공 여부
*/
bool BinaryFile::openForWrite(const std::wstring& inFilePath)
{
	fileHandle_ = ::CreateFile(inFilePath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	mmapHandle_ = INVALID_HANDLE_VALUE;

	return fileHandle_ != INVALID_HANDLE_VALUE;
}

/**
	@brief 파일의 포인터를 이동한다
	@param inPos 이동할 위치
*/
void BinaryFile::setFilePointer64(uint64_t inPos)
{
	LONG seekMoveHigh = inPos >> 32;

	::SetFilePointer(fileHandle_, inPos & 0xffffffff, &seekMoveHigh, SEEK_SET);
}

/**
	@brief 파일의 크기를 얻어온다
	@return 파일의 크기
*/
uint64_t BinaryFile::getFileSize()
{
	return fileSize_;
}

/**
	@brief 파일의 내용을 읽어온다
	
	@param inPos 읽을 위치
	@param outBuffer 읽은 내용이 담길 버퍼
	@param inReadSize 읽을 사이즈

	@return 읽은 사이즈
*/
DWORD BinaryFile::readData(uint64_t inPos, void* outBuffer, DWORD inReadSize)
{
	DWORD readSize = 0;

	setFilePointer64(inPos);

	::ReadFile(fileHandle_, outBuffer, inReadSize, &readSize, NULL);

	return readSize;
}

/**
	@brief 파일에 내용을 기록한다
	
	@param inPos 기록할 위치
	@param inBuffer 기록할 내용이 담긴 버퍼
	@param inWriteSize 기록할 사이즈

	@return 실제 기록한 사이즈
*/
DWORD BinaryFile::writeData(uint64_t inPos, const void* inBuffer, DWORD inWriteSize)
{
	DWORD writeSize = 0;

	setFilePointer64(inPos);

	::WriteFile(fileHandle_, inBuffer, inWriteSize, &writeSize, NULL);

	return writeSize;
}

/**
	@brief 파일을 닫는다
	@return 성공 여부
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
	@brief 파일에 대한 메모리 맵을 만든다

	@param inPos 메모리 맵이 시작될 위치
	@param inSize 메모리 맵의 사이즈

	@return 생성된 파일 맵 클래스
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