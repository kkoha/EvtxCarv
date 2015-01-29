/**
	@file FileMap.h
	@brief 메모리 맵 구조를 표현하는 클래스의 정의
*/

#ifndef _FILE_MAP_H_
#define _FILE_MAP_H_

#include <Windows.h>
#include "DataType.h"

/**
	@class FileMap
	@brief 메모리 맵 IO구조를 래핑한 클래스
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