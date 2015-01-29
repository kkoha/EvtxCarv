/**
	@file EvtxCarver.cpp
	@brief EVTX카빙과 관련된 기능들을 모아놓은 래퍼 클래스의 구현
*/

#include "EvtxCarver.h"
#include "DataType.h"
#include <Windows.h>

/**
	@brief 카빙할 파일을 정한다
	@return 파일 열기 성공여부
*/
bool EvtxCarver::setCarveFile(const std::wstring& inFileName)
{
	return carveFile_.openForRead(inFileName.c_str());
}

/**
	@brief 프로그램이 실행되고 있는 정보를 받기위한 콜백을 설정한다
	@param inCarveInfoCallback 정보를 받을 콜백
*/
void EvtxCarver::setCarveInfoCallback(CarveInfoCallback inCarveInfoCallback)
{
	carveInfoCallback_ = inCarveInfoCallback;
}

/**
	@brief 프로그램이 실행되고 있는 정보를 콜백에 보낸다
	@param inMessage 콜백에 보낼 정보
*/
void EvtxCarver::sendCarveInfo(const std::wstring& inMessage)
{
	if (carveInfoCallback_ != NULL)
	{
		carveInfoCallback_(inMessage);
	}
}

/**
	@brief 카빙된 내용을 출력할 폴더를 설정한다
	@param inDumpPathName 출력 폴더명
*/
void EvtxCarver::setDumpPathName(const std::wstring& inDumpPathName)
{
	dumpPathName_ = inDumpPathName;
	uint32_t dirAttr = 0;

	if (dumpPathName_[dumpPathName_.length() - 1] == '\\')
	{
		dumpPathName_ = dumpPathName_.substr(0, dumpPathName_.length() - 1);
	}

	dirAttr = GetFileAttributes(dumpPathName_.c_str());

	if (dirAttr == INVALID_FILE_ATTRIBUTES)
	{
		::CreateDirectory(dumpPathName_.c_str(), NULL);
	}
}

/**
	@brief 카빙할 수준을 정한다(파일+청크, 레코드)
	@param inCarveLevel 카빙할 수준
*/
void EvtxCarver::setCarveLevel(CarveLevel inCarveLevel)
{
	carveLevel_ = inCarveLevel;
}

/**
	@brief 카빙을 시작한다
*/
bool EvtxCarver::startCarving()
{
	CarvedAreas nullCarvedArea;
	CarvedAreas fileCarvedArea;
	CarvedAreas chunkCarvedArea;
	CarvedAreas reassembledChunkCarvedArea;
	BinaryFile tmpFile;

	fileCarver_.setCarveFile(carveFile_);
	chunkCarver_.setCarveFile(carveFile_);

	recordCarver_.setCarveFile(carveFile_);

	fileCarver_.setDumpPathName(dumpPathName_);
	chunkCarver_.setDumpPathName(dumpPathName_);
	recordCarver_.setDumpPathName(dumpPathName_);
	reassembledChunkCarver_.setDumpPathName(dumpPathName_);
	reassembledRecordCarver_.setDumpPathName(dumpPathName_);

	fileCarver_.setDumpFilePrefix(_T("File_"));
	chunkCarver_.setDumpFilePrefix(_T("Chunk_"));
	recordCarver_.setDumpFilePrefix(_T("Record"));
	reassembledChunkCarver_.setDumpFilePrefix(_T("Chunk_"));
	reassembledRecordCarver_.setDumpFilePrefix(_T("Record"));

	chunkCarver_.setFragmentSize(65535);
	reassembledChunkCarver_.setFragmentSize(65535);

	// 파일 카빙
	sendCarveInfo(_T("    Carving File..."));
	fileCarver_.startCarving(fileCarvedArea);
	sendCarveInfo(_T("    Carving File Complete\n"));

	if (carveLevel_ == CARVE_LEVEL_CHUNK || carveLevel_ == CARVE_LEVEL_RECORD)
	{
		// 청크 카빙(출력은 내지않고 스킵할 영역만 추출한다)
		sendCarveInfo(_T("    Carving Chunk..."));
		chunkCarver_.startCarving(fileCarvedArea, false, false, chunkCarvedArea, nullCarvedArea);
		sendCarveInfo(_T("    Carving Chunk Complete\n"));
	}
	
	if (carveLevel_ == CARVE_LEVEL_RECORD)
	{
		sendCarveInfo(_T("    Collecting Name, Templates..."));
		recordCarver_.startCarving(nullCarvedArea, true);
		sendCarveInfo(_T("    Collecting Name, Templates Complete\n"));
	}

	// 청크/레코드 단위 카빙에서만 재조립을 수행한다
	if (carveLevel_ == CARVE_LEVEL_CHUNK || carveLevel_ == CARVE_LEVEL_RECORD)
	{
		// 재조립을 위한 임시 파일
		tmpFile.openForWrite(dumpPathName_ + _T("\\_tmpFile_"));

		clusterReassembler_.setFragmentedFile(carveFile_);
		clusterReassembler_.setOutputFile(tmpFile);
		

		// 카빙되지 않은 영역들을 재조립한다
		sendCarveInfo(_T("\n    Reassembling Clusters..."));
		clusterReassembler_.reassembleCluster(chunkCarvedArea);
		sendCarveInfo(_T("    Reassembling Clusters Complete\n"));
		
		tmpFile.closeFile();
		
		tmpFile.openForRead(dumpPathName_ + _T("\\_tmpFile_"));

		reassembledChunkCarver_.setCarveFile(carveFile_);
		reassembledChunkCarver_.setFragmentedFile(tmpFile);
		reassembledRecordCarver_.setCarveFile(tmpFile);

		// 재조립된 파일 및 원본 파일에서 청크들을 카빙한다(출력 내보냄)
		sendCarveInfo(_T("        Carving Chunk..."));
		reassembledChunkCarver_.startCarving(fileCarvedArea, true, true, chunkCarvedArea, reassembledChunkCarvedArea);
		sendCarveInfo(_T("        Carving Chunk Complete\n"));

		if (carveLevel_ == CARVE_LEVEL_RECORD)
		{
			// 그래도 카빙되지 않은 영역에 대하여 레코드 해석을 시도한다
			sendCarveInfo(_T("        Carving Record..."));
			reassembledRecordCarver_.startCarving(reassembledChunkCarvedArea);
			sendCarveInfo(_T("        Carving Record Complete\n"));
		}

		tmpFile.closeFile();

		::DeleteFile((dumpPathName_ + _T("\\_tmpFile_")).c_str());
	}

	return true;
}

/**
	@brief 카빙할 파일을 닫는다
*/
void EvtxCarver::closeCarveFile()
{
	carveFile_.closeFile();
}