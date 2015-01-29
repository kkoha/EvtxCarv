/**
	@file EvtxRecordHeader.h
	@brief EVTX���ڵ��� ��� ������ ���õ� ����/����ü��
*/

#ifndef _EVTX_RECORD_HEADER_
#define _EVTX_RECORD_HEADER_

#include <Windows.h>
#include "DataType.h"

#define EVTX_RECORD_HEADER_MAGIC 0x00002A2A /* "**",00,00 */ ///< EVTX���ڵ� ����� ������ **,0,0
#define EVTX_RECORD_HEADER_SIZE 0x18 ///< EVTX���ڵ� ����� ������

#pragma pack(1)

/**
	@class EVTX_RECORD_HEADER
	@brief EVTX ������ ���ڵ� ��� ����
*/
struct EVTX_RECORD_HEADER
{
	uint32_t magic;  ///< ������
	uint32_t length1;  ///< ���ڵ��� ����
	uint64_t numLogRecord;  ///< ���ڵ� ��ȣ
	FILETIME timeCreated;  ///< ���ڵ尡 ������ �ð�
};

#pragma pack()

#endif /* _EVTX_RECORD_HEADER_ */