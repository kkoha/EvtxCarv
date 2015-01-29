/**
	@file EvtxRecordCarver.h
	@brief EVTX���ڵ带 ī���ϱ� ���� Ŭ������ ����
*/

#ifndef _EVTX_RECORD_CARVER_H_
#define _EVTX_RECORD_CARVER_H_

#include <Windows.h>
#include "BinaryFile.h"
#include "BinXmlStream.h"

/**
	@class EvtxRecordCarver
	@brief EVTX ���ڵ� ī���� ���� Ŭ����
*/
class EvtxRecordCarver
{
	public:
		EvtxRecordCarver();

		void setDumpPathName(const std::wstring& inDumpPathName);
		void setDumpFilePrefix(const std::wstring& inDumpFilePrefix);

		void setCarveFile(BinaryFile& inFile);

		bool startCarving(const CarvedAreas& inCarvedAreas, bool inJustCollectNameTemplate = false);

	private:
		bool dumpToCsvFile(const RecordPoses& inRecordPoses);
		void collectDatas(const SortedRecord& inSortedRecords, BinXmlStream::BinXmlProcessType inProcessType);

		std::wstring dumpPathName_;  ///< ī���� ���ڵ���� ������ ������ ���丮
		std::wstring dumpFilePrefix_;  ///< ī���� ���ڵ���� ������ ������ ������ ���λ�

		BinaryFile carveFile_;  ///< ī���� ����
};

#endif /* _EVTX_RECORD_CARVER_H_ */