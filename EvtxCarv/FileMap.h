/**
	@file FileMap.h
	@brief �޸� �� ������ ǥ���ϴ� Ŭ������ ����
*/

#ifndef _FILE_MAP_H_
#define _FILE_MAP_H_

#include <Windows.h>
#include "DataType.h"

/**
	@class FileMap
	@brief �޸� �� IO������ ������ Ŭ����
*/
class FileMap
{
	public:
		FileMap();
		FileMap(HANDLE inMapHandle, uint64_t inFilePos, uint64_t inSize, bool inIsDeviceFile = false);

		uint8_t* getData();
		uint64_t getFilePos();
		uint64_t getSize();

		bool isValidMap();

		void closeMap();
	private:
		HANDLE mapHandle_;
		void* mappedAddress_;
		void* userAddress_;
		uint64_t filePos_;
		uint64_t size_;
		bool isDeviceFile_;
};

#endif /* _FILE_MAP_H_ */