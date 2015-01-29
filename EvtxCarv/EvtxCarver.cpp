/**
	@file EvtxCarver.cpp
	@brief EVTXī���� ���õ� ��ɵ��� ��Ƴ��� ���� Ŭ������ ����
*/

#include "EvtxCarver.h"
#include "DataType.h"
#include <Windows.h>

/**
	@brief ī���� ������ ���Ѵ�
	@return ���� ���� ��������
*/
bool EvtxCarver::setCarveFile(const std::wstring& inFileName)
{
	return carveFile_.openForRead(inFileName.c_str());
}

/**
	@brief ���α׷��� ����ǰ� �ִ� ������ �ޱ����� �ݹ��� �����Ѵ�
	@param inCarveInfoCallback ������ ���� �ݹ�
*/
void EvtxCarver::setCarveInfoCallback(CarveInfoCallback inCarveInfoCallback)
{
	carveInfoCallback_ = inCarveInfoCallback;
}

/**
	@brief ���α׷��� ����ǰ� �ִ� ������ �ݹ鿡 ������
	@param inMessage �ݹ鿡 ���� ����
*/
void EvtxCarver::sendCarveInfo(const std::wstring& inMessage)
{
	if (carveInfoCallback_ != NULL)
	{
		carveInfoCallback_(inMessage);
	}
}

/**
	@brief ī���� ������ ����� ������ �����Ѵ�
	@param inDumpPathName ��� ������
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
	@brief ī���� ������ ���Ѵ�(����+ûũ, ���ڵ�)
	@param inCarveLevel ī���� ����
*/
void EvtxCarver::setCarveLevel(CarveLevel inCarveLevel)
{
	carveLevel_ = inCarveLevel;
}

/**
	@brief ī���� �����Ѵ�
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

	// ���� ī��
	sendCarveInfo(_T("    Carving File..."));
	fileCarver_.startCarving(fileCarvedArea);
	sendCarveInfo(_T("    Carving File Complete\n"));

	if (carveLevel_ == CARVE_LEVEL_CHUNK || carveLevel_ == CARVE_LEVEL_RECORD)
	{
		// ûũ ī��(����� �����ʰ� ��ŵ�� ������ �����Ѵ�)
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

	// ûũ/���ڵ� ���� ī�������� �������� �����Ѵ�
	if (carveLevel_ == CARVE_LEVEL_CHUNK || carveLevel_ == CARVE_LEVEL_RECORD)
	{
		// �������� ���� �ӽ� ����
		tmpFile.openForWrite(dumpPathName_ + _T("\\_tmpFile_"));

		clusterReassembler_.setFragmentedFile(carveFile_);
		clusterReassembler_.setOutputFile(tmpFile);
		

		// ī������ ���� �������� �������Ѵ�
		sendCarveInfo(_T("\n    Reassembling Clusters..."));
		clusterReassembler_.reassembleCluster(chunkCarvedArea);
		sendCarveInfo(_T("    Reassembling Clusters Complete\n"));
		
		tmpFile.closeFile();
		
		tmpFile.openForRead(dumpPathName_ + _T("\\_tmpFile_"));

		reassembledChunkCarver_.setCarveFile(carveFile_);
		reassembledChunkCarver_.setFragmentedFile(tmpFile);
		reassembledRecordCarver_.setCarveFile(tmpFile);

		// �������� ���� �� ���� ���Ͽ��� ûũ���� ī���Ѵ�(��� ������)
		sendCarveInfo(_T("        Carving Chunk..."));
		reassembledChunkCarver_.startCarving(fileCarvedArea, true, true, chunkCarvedArea, reassembledChunkCarvedArea);
		sendCarveInfo(_T("        Carving Chunk Complete\n"));

		if (carveLevel_ == CARVE_LEVEL_RECORD)
		{
			// �׷��� ī������ ���� ������ ���Ͽ� ���ڵ� �ؼ��� �õ��Ѵ�
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
	@brief ī���� ������ �ݴ´�
*/
void EvtxCarver::closeCarveFile()
{
	carveFile_.closeFile();
}