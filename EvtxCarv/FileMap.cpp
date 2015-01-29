/**
	@file FileMap.cpp
	@brief �޸� �� ������ ǥ���ϴ� Ŭ������ ����
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
	@brief ������ Ư�� ������ �޸� �����Ѵ�

	@param inMapHandle �޸� �� �ڵ�
	@param inFilePos ������ ������ ������
	@param inSize ������ ������
	@param inIsDeviceFile ����̽� �������� ����
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
	@brief ���ε� �޸��� �ּҸ� ���´�
	@return ���ε� �޸� �ּ�
*/
uint8_t* FileMap::getData()
{
	return (uint8_t*)userAddress_;
}

/**
	@brief ���ε� ������ ��ġ�� ���´�
	@return ���ε� ������ ��ġ
*/
uint64_t FileMap::getFilePos()
{
	return filePos_;
}

/**
	@brief ���ε� ũ�⸦ ���´�
	@return ���ε� ũ��
*/
uint64_t FileMap::getSize()
{
	return size_;
}

/**
	@brief �޸� ���� �ݴ´�
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
	@brief ��ȿ�� �޸� ������ Ȯ���Ѵ�
	@return ��ȿ ����
*/
bool FileMap::isValidMap()
{
	return mappedAddress_ != NULL;
}