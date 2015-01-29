/**
	@file BinaryFile.h
	@brief 이진 파일을 다루기 위한 클래스의 정의
*/

#ifndef _FILE_TOOL_H_
#define _FILE_TOOL_H_

#include <Windows.h>
#include <string>
#include "DataType.h"
#include "FileMap.h"

/**
	@class BinaryFile
	@brief 이진 파일을 좀더 편하게 다루기 위한 클래스
*/
class BinaryFile
{
	public:
		bool openForRead(const std::wstring& inFilePath);
		bool openForWrite(const std::wstring& inFilePath);

		void setFilePointer64(uint64_t inPos);

		uint64_t getFileSize();

		DWORD readData(uint64_t inPos, void* outBuffer, DWORD inReadSize);
		DWORD writeData(uint64_t inPos, const void* inBuffer, DWORD inWriteSize);

		bool closeFile();

		FileMap createFileMap(uint64_t inPos, uint64_t inSize);

	private:
		HANDLE mmapHandle_;  ///< 메모리 맵 핸들
		HANDLE fileHandle_;  ///< 파일의 핸들
		uint64_t fileSize_;  ///< 파일의 크기
		bool isDeviceFile_;
};

#endif /* _FILE_TOOL_H_ */