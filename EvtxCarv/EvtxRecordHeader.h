/**
	@file EvtxRecordHeader.h
	@brief EVTX레코드의 헤더 구조와 관련된 정의/구조체들
*/

#ifndef _EVTX_RECORD_HEADER_
#define _EVTX_RECORD_HEADER_

#include <Windows.h>
#include "DataType.h"

#define EVTX_RECORD_HEADER_MAGIC 0x00002A2A /* "**",00,00 */ ///< EVTX레코드 헤더으 매직값 **,0,0
#define EVTX_RECORD_HEADER_SIZE 0x18 ///< EVTX레코드 헤더의 사이즈

#pragma pack(1)

/**
	@class EVTX_RECORD_HEADER
	@brief EVTX 파일의 레코드 헤더 구조
*/
struct EVTX_RECORD_HEADER
{
	uint32_t magic;  ///< 매직값
	uint32_t length1;  ///< 레코드의 길이
	uint64_t numLogRecord;  ///< 레코드 번호
	FILETIME timeCreated;  ///< 레코드가 생성된 시간
};

#pragma pack()

#endif /* _EVTX_RECORD_HEADER_ */