/**
	@file EvtxCarver.h
	@brief EVTX카빙과 관련된 기능들을 모아놓은 래퍼 클래스의 정의
*/

#ifndef _EVTX_CARVER_H_
#define _EVTX_CARVER_H_

#include <windows.h>
#include <functional>
#include <stdio.h>
#include <tchar.h>
#include <list>
#include "EvtxChunkHeader.h"
#include "EvtxChunkCarver.h"
#include "EvtxRecordCarver.h"
#include "EvtxFileCarver.h"
#include "BinaryFile.h"
#include "ClusterReassembler.h"
#include <string>

/**
	@class EvtxCarver
	@brief EVTX카빙과 관련된 기능들을 모아놓은 래퍼 클래스
*/
class EvtxCarver
{
	public:
		enum CarveLevel
		{
			CARVE_LEVEL_RECORD = 0,  ///< 레코드 단위 카빙
			CARVE_LEVEL_CHUNK = 1,  ///< 청크 단위 카빙
		};

		bool setCarveFile(const std::wstring& inFileName);
		void setDumpPathName(const std::wstring& inDumpPathName);
		void setCarveLevel(CarveLevel inCarveLevel);
		void closeCarveFile();

		void setCarveInfoCallback(CarveInfoCallback inCarveInfoCallback);

		bool startCarving();

	private:
		void sendCarveInfo(const std::wstring& inMessage);

		std::wstring dumpPathName_;  ///< 카빙한 내용들의 내용을 내보낼 디렉토리
		CarveLevel carveLevel_;  ///< 카빙할 수준(파일 및 청크 / 레코드)
		BinaryFile carveFile_;  ///< 카빙할 파일
		EvtxFileCarver fileCarver_;  ///< 파일 카빙용 객체
		EvtxChunkCarver chunkCarver_;  ///< 청크 카빙용 객체
		EvtxRecordCarver recordCarver_;  ///< 레코드 카빙용 객체

		ClusterReassembler clusterReassembler_;  ///< 조각난 데이터를 재조립하기 위한 객체

		EvtxChunkCarver reassembledChunkCarver_;  ///< 재조립된 청크를 카빙하기 위한 객체
		EvtxRecordCarver reassembledRecordCarver_;  ///< 재조립된 레코드를 카빙하기 위한 객체

		CarveInfoCallback carveInfoCallback_;  ///< 카빙중인 정보를 표기하기 위한 콜백
};

#endif /* _EVTX_CARVER_H_ */