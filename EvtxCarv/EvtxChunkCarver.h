/**
	@file EvtxChunkCarver.h
	@brief EVTXûũ�� ī���ϱ� ���� Ŭ������ ����
*/

#ifndef _EVTX_CHUNK_CARVER_H_
#define _EVTX_CHUNK_CARVER_H_

#include <Windows.h>
#include <string>
#include "BinaryFile.h"
#include "DataType.h"

/**
	@class EvtxChunkCarver
	@brief EVTXûũ ī���� ���� Ŭ����
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

		std::wstring dumpPathName_;  ///< ī���� ûũ���� ������ ������ ���丮
		std::wstring dumpFilePrefix_;  ///< ī���� ûũ���� ������ ������ ������ ���λ�

		BinaryFile carveFile_;  ///< ī���� ����(����)
		BinaryFile fragmentedFile_;  ///< ī���� ����(������)

		int fragmentSize_;  ///< �� ���̷� �� ûũ�� �ִ� ����
		int fileIndex_; ///< ������ ������ ��ȣ
};

#endif /* _EVTX_CHUNK_CARVER_H_ */