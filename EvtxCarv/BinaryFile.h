/**
	@file BinaryFile.h
	@brief ���� ������ �ٷ�� ���� Ŭ������ ����
*/

#ifndef _FILE_TOOL_H_
#define _FILE_TOOL_H_

#include <Windows.h>
#include <string>
#include "DataType.h"
#include "FileMap.h"

/**
	@class BinaryFile
	@brief ���� ������ ���� ���ϰ� �ٷ�� ���� Ŭ����
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
		HANDLE mmapHandle_;  ///< �޸� �� �ڵ�
		HANDLE fileHandle_;  ///< ������ �ڵ�
		uint64_t fileSize_;  ///< ������ ũ��
		bool isDeviceFile_;
};

#endif /* _FILE_TOOL_H_ */