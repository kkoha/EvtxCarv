/**
	@file EvtxRecordCarver.h
	@brief EVTX레코드를 카빙하기 위한 클래스의 정의
*/

#ifndef _EVTX_RECORD_CARVER_H_
#define _EVTX_RECORD_CARVER_H_

#include <Windows.h>
#include "BinaryFile.h"
#include "BinXmlStream.h"

/**
	@class EvtxRecordCarver
	@brief EVTX 레코드 카빙을 위한 클래스
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

		std::wstring dumpPathName_;  ///< 카빙한 레코드들의 내용을 내보낼 디렉토리
		std::wstring dumpFilePrefix_;  ///< 카빙할 레코드들의 내용을 내보낼 파일의 접두사

		BinaryFile carveFile_;  ///< 카빙할 파일
};

#endif /* _EVTX_RECORD_CARVER_H_ */