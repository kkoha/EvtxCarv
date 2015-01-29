/**
	@file FileMap.cpp
	@brief 메모리 맵 구조를 표현하는 클래스의 구현
*/

#include "FileMap.h"

FileMap::FileMap()
{
	mapHandle_ = INVALID_HANDLE_VALUE;
	filePos_ = 0;
	size_ = 0;
	mappedAddress_ = NULL;
	userAddress_ = NULL;
}

/**
	@brief 파일의 특정 영역을 메모리 매핑한다

	@param inMapHandle 메모리 맵 핸들
	@param inFilePos 매핑할 파일의 시작점
	@param inSize 매핑할 사이즈
	@param inIsDeviceFile 디바이스 파일인지 여부
*/
FileMap::FileMap(HANDLE inMapHandle, uint64_t inFilePos, uint64_t inSize, bool inIsDeviceFile)
{
	isDeviceFile_ = inIsDeviceFile;
	mapHandle_ = inMapHandle;
	filePos_ = inFilePos;
	size_ = inSize;

	if (inIsDeviceFile)
	{
		uint64_t alignedFilePos = inFilePos - (inFilePos % 4096);

		LONG seekMoveHigh = alignedFilePos >> 32;

		::SetFilePointer(inMapHandle, alignedFilePos & 0xffffffff, &seekMoveHigh, SEEK_SET);

		mappedAddress_ = new uint8_t[inSize + (inFilePos % 4096)];
		DWORD readSize = 0;
		DWORD remainSize = inSize + (inFilePos % 4096);
		DWORD bufferPos = 0;
		
		if (mappedAddress_ != NULL)
		{
			while (remainSize != 0)
			{
				::ReadFile(mapHandle_, ((uint8_t*)mappedAddress_) + bufferPos, remainSize, &readSize, NULL);
				if (readSize == 0) break;

				remainSize -= readSize;
				bufferPos += readSize;
			}

			userAddress_ = ((uint8_t*)mappedAddress_) + (inFilePos % 4096);
		}
	}
	else
	{
		uint64_t alignedFilePos = inFilePos - (inFilePos % 4096);

		mappedAddress_ = ::MapViewOfFile(mapHandle_, FILE_MAP_READ, alignedFilePos >> 32, alignedFilePos & 0xFFFFFFFF, (size_t)(inSize + (inFilePos % 4096)));

		userAddress_ = ((uint8_t*)mappedAddress_) + (inFilePos % 4096);
	}
}

/**
	@brief 매핑된 메모리의 주소를 얻어온다
	@return 매핑된 메모리 주소
*/
uint8_t* FileMap::getData()
{
	return (uint8_t*)userAddress_;
}

/**
	@brief 매핑된 파일의 위치를 얻어온다
	@return 매핑된 파일의 우치
*/
uint64_t FileMap::getFilePos()
{
	return filePos_;
}

/**
	@brief 매핑된 크기를 얻어온다
	@return 매핑된 크기
*/
uint64_t FileMap::getSize()
{
	return size_;
}

/**
	@brief 메모리 맵을 닫는다
*/
void FileMap::closeMap()
{
	if (isDeviceFile_)
	{
		if (mappedAddress_ != NULL)
		{
			delete [] mappedAddress_;
		}
	}
	else
	{
		::UnmapViewOfFile(mappedAddress_);
	}

	mappedAddress_ = NULL;
}

/**
	@brief 유효한 메모리 맵인지 확인한다
	@return 유효 여부
*/
bool FileMap::isValidMap()
{
	return mappedAddress_ != NULL;
}