/**
	@file EvtxChunkCarver.h
	@brief EVTX청크를 카빙하기 위한 클래스의 정의
*/

#ifndef _EVTX_CHUNK_CARVER_H_
#define _EVTX_CHUNK_CARVER_H_

#include <Windows.h>
#include <string>
#include "BinaryFile.h"
#include "DataType.h"

/**
	@class EvtxChunkCarver
	@brief EVTX청크 카빙을 위한 클래스
*/
class EvtxChunkCarver
{
	public:
		EvtxChunkCarver();

		void setDumpPathName(const std::wstring& inDumpPathName);
		void setDumpFilePrefix(const std::wstring& inDumpFilePrefix);

		void setCarveFile(BinaryFile& inFile);
		void setFragmentedFile(BinaryFile& inFile);

		void setFragmentSize(int inFragmentSize);

		bool startCarving(const CarvedAreas& inCarvedAreas, bool inUseFragmentedFile, bool inWriteOutput, CarvedAreas& outCarvedAreas, CarvedAreas& outFragmentedCarvedAreas);

	private:
		bool dumpToEvtxFile(const ChunkInfos& inChunkInfos);
		void insertChunkIntoGroup(GroupedChunks& inOutGroupedChunk, const ChunkInfo& inChunkInfo);

		std::wstring dumpPathName_;  ///< 카빙한 청크들의 내용을 내보낼 디렉토리
		std::wstring dumpFilePrefix_;  ///< 카빙할 청크들의 내용을 내보낼 파일의 접두사

		BinaryFile carveFile_;  ///< 카빙할 파일(원본)
		BinaryFile fragmentedFile_;  ///< 카빙할 파일(재조립)

		int fragmentSize_;  ///< 한 파이렝 들어갈 청크의 최대 개수
		int fileIndex_; ///< 내보낼 파일의 번호
};

#endif /* _EVTX_CHUNK_CARVER_H_ */