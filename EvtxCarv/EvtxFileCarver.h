/**
	@file EvtxFileCarver.h
	@brief EVTX������ ī���ϱ� ���� Ŭ������ ����
*/

#ifndef _EVTX_FILE_CARVER_H_
#define _EVTX_FILE_CARVER_H_

#include <Windows.h>
#include <string>
#include "BinaryFile.h"

/**
	@class EvtxFileCarver
	@brief EVTX���� ī���� ���� Ŭ����
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

		std::wstring dumpPathName_;  ///< ī���� ���ϵ��� ������ ������ ���丮
		std::wstring dumpFilePrefix_;  ///< ī���� ���ϵ��� ������ ������ ������ ���λ�

		BinaryFile carveFile_;  ///< ī���� ����

		int fileIndex_; ///< ������ ������ ��ȣ
};

#endif /* _EVTX_FILE_CARVER_H_ */