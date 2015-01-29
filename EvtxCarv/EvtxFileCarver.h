/**
	@file EvtxFileCarver.h
	@brief EVTX파일을 카빙하기 위한 클래스의 정의
*/

#ifndef _EVTX_FILE_CARVER_H_
#define _EVTX_FILE_CARVER_H_

#include <Windows.h>
#include <string>
#include "BinaryFile.h"

/**
	@class EvtxFileCarver
	@brief EVTX파일 카빙을 위한 클래스
*/
class EvtxFileCarver
{
	public:
		EvtxFileCarver();

		void setDumpPathName(const std::wstring& inDumpPathName);
		void setDumpFilePrefix(const std::wstring& inDumpFilePrefix);

		void setCarveFile(BinaryFile& inCarveFile);

		bool startCarving(CarvedAreas& outCarvedArea);

	private:
		bool dumpToEvtxFile(uint64_t inFilePos, uint64_t& outFileSize);

		std::wstring dumpPathName_;  ///< 카빙한 파일들의 내용을 내보낼 디렉토리
		std::wstring dumpFilePrefix_;  ///< 카빙할 파일들의 내용을 내보낼 파일의 접두사

		BinaryFile carveFile_;  ///< 카빙할 파일

		int fileIndex_; ///< 내보낼 파일의 번호
};

#endif /* _EVTX_FILE_CARVER_H_ */