/**
	@file EvtxFileCarver.cpp
	@brief EVTX������ ī���ϱ� ���� Ŭ������ ����
*/

#include "EvtxFileCarver.h"
#include "BinaryFile.h"
#include "EvtxFileHeader.h"
#include "EvtxFileValidator.h"
#include "EvtxChunkHeader.h"
#include "DataType.h"
#include <tchar.h>
#include "string.h"
#include <stdio.h>
#include <string>
#include "SignatureFinder.h"
#include "ChunkInfoParser.h"
#include "EvtxChunkValidator.h"

EvtxFileCarver::EvtxFileCarver()
{
	;
}

/**
	@brief ī���� ������ �����Ѵ�
	@param inCarveFile ī���� ����
*/
void EvtxFileCarver::setCarveFile(BinaryFile& inCarveFile)
{
	carveFile_ = inCarveFile;
}

/**
	@brief ī���� ������ ����� ���� �̸��� ���Ѵ�
	@param inDumpFilePrefix ��� ���ϸ��� ���λ�
*/
void EvtxFileCarver::setDumpFilePrefix(const std::wstring& inDumpFilePrefix)
{
	dumpFilePrefix_ = inDumpFilePrefix;
}

/**
	@brief ī���� ������ ����� ������ �����Ѵ�
	@param inDumpPathName ��� ������
*/
void EvtxFileCarver::setDumpPathName(const std::wstring& inDumpPathName)
{
	dumpPathName_ = inDumpPathName;
}

/**
	@brief ���ڵ� ī���� �����Ѵ�

	@param outCarvedArea ���� ī������ ��ŵ�� ����
*/
bool EvtxFileCarver::startCarving(CarvedAreas& outCarvedArea)
{
	uint64_t filePos = 0;
	uint64_t dumpedFileSize = 0;

	outCarvedArea.clear();

	if (carveFile_.getFileSize() < EVTX_CHUNK_SIZE)
	{
		return true;
	}

	SignatureFinder signatureFinder(carveFile_, EVTX_FILE_HEADER_MAGIC, 8, 512, NULL, EVTX_CHUNK_SIZE);

	fileIndex_ = 0;

	// ��ȿ�� evtx ������ ã�Ƽ�
	while (signatureFinder.getNext(&filePos) != NULL)
	{
		if (EvtxFileValidator::isValidEvtxFile(carveFile_, filePos))
		{
			// evtx���Ϸ� �ٷ� �����Ѵ�
			if (dumpToEvtxFile(filePos, dumpedFileSize))
			{
				outCarvedArea.push_back(std::make_pair(filePos, filePos + dumpedFileSize));
				filePos += dumpedFileSize;
				continue;
			}
		}
	}

	signatureFinder.closeMap();

	return true;
}

/**
	@brief Ư�� ��ġ�� evtx���Ϸ� �����Ѵ�
	@param inFilePos ������ ��ġ
	@param outFileSize ������ ������
*/
bool EvtxFileCarver::dumpToEvtxFile(uint64_t inFilePos, uint64_t& outFileSize)
{
	uint8_t evtxFileHeaderBuffer[EVTX_FILE_HEADER_SIZE] = {0,};
	uint8_t evtxChunkDataBuffer[EVTX_CHUNK_SIZE] = {0,};

	EVTX_FILE_HEADER* evtxFileHeader = (EVTX_FILE_HEADER*)evtxFileHeaderBuffer;
	BinaryFile dumpFile;

	DWORD chunkNum = 0;
	
	wchar_t outputFileName[MAX_PATH_SIZE] = {0,};

	outFileSize = 0;

	carveFile_.readData(inFilePos + EVTX_FILE_HEADER_SIZE, evtxChunkDataBuffer, EVTX_CHUNK_SIZE);

	ChunkInfoParser chunkInfoParser(evtxChunkDataBuffer);

	// ù ��° ûũ�� ù ���� ���ڵ忡 ��ϵǾ��ִ� ��ǻ��/ä���̸� ������� ��� ���� �̸��� ���Ѵ�
	wsprintf(outputFileName, _T("%s\\%s_%s_%s%05d.evtx"), dumpPathName_.c_str(), chunkInfoParser.getComputerName().c_str(),
		chunkInfoParser.getChannelName().c_str(), dumpFilePrefix_.c_str(), fileIndex_);

	dumpFile.openForWrite(outputFileName);

	if (carveFile_.readData(inFilePos, evtxFileHeaderBuffer, EVTX_FILE_HEADER_SIZE) != EVTX_FILE_HEADER_SIZE)
	{
		return false;
	}

	outFileSize += EVTX_FILE_HEADER_SIZE;

	// evtx����� �����Ѵ�
	dumpFile.writeData(0, evtxFileHeaderBuffer, EVTX_FILE_HEADER_SIZE);

	// evtx����� �����L�� ûũ�� ����ŭ
	for (chunkNum = 0; chunkNum < evtxFileHeader->chunkCount; ++chunkNum)
	{
		uint64_t chunkPos = inFilePos + EVTX_FILE_HEADER_SIZE + EVTX_CHUNK_SIZE * chunkNum;

		if (carveFile_.readData(chunkPos, evtxChunkDataBuffer, EVTX_CHUNK_SIZE) != EVTX_CHUNK_SIZE)
		{
			break;
		}

		if (!EvtxChunkValidator::isValidChunk(evtxChunkDataBuffer))
		{
			dumpFile.closeFile();
			::DeleteFile(outputFileName);
		}

		outFileSize += EVTX_CHUNK_SIZE;

		// ûũ�� �о �����Ѵ�
		dumpFile.writeData(EVTX_FILE_HEADER_SIZE + EVTX_CHUNK_SIZE * chunkNum, evtxChunkDataBuffer, EVTX_CHUNK_SIZE);
	}

	dumpFile.closeFile();

	++fileIndex_;

	return chunkNum == evtxFileHeader->chunkCount;
}