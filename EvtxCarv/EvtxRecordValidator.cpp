/**
	@file EvtxRecordValidator.cpp
	@brief EVTX의 레코드 헤더/데이터의 유효성을 검증하기 위한 클래스의 구현
*/

#include "EvtxRecordHeader.h"
#include "EvtxRecordValidator.h"
#include "EvtxChunkHeader.h"
#include <stdio.h>

/**
	@brief 해당하는 버퍼가 유효한 레코드인지 확인한다

	@param arg 메모리 버퍼
	@param bufferSize 버퍼의 크기

	@return 유효성 여부
*/
bool EvtxRecordValidator::isValidRecord(void* arg, uint64_t bufferSize)
{
	EVTX_RECORD_HEADER* recordHeader = (EVTX_RECORD_HEADER*)arg;
	uint8_t* recordData = (uint8_t*)arg;

	// 레코드의 magic값 확인
	if (recordHeader->magic != EVTX_RECORD_HEADER_MAGIC) return false;

	// 레코드의 크기가 청크 크기보다 클 수 없다
	if (recordHeader->length1 > EVTX_CHUNK_SIZE - EVTX_CHUNK_HEADER_SIZE) return false;

	// 레코드의 크기가 기본 헤더크기보다 작을 수 없다
	if (recordHeader->length1 < 28) return false;

	// 레코드의 크기가 현재 확인가능한 버퍼크기보다 크면 xml스트림의 시작 여부만 확인한다
	// 아니면 레코드의 끝에 레코드의 사이즈가 정확하게 나오는지 확인한다
	if (recordHeader->length1 > bufferSize)
	{
		return isValidBinXmlPreamble(recordData + EVTX_RECORD_HEADER_SIZE);
	}
	else if (*((uint32_t*)(recordData + recordHeader->length1 - 4)) != recordHeader->length1)
	{
		return false;
	}

	return isValidBinXmlPreamble(recordData + EVTX_RECORD_HEADER_SIZE);
}

/**
	@brief 해당하는 버퍼가 유효한 레코드 헤더인지 판별한다
	@param arg 메모리 버퍼
	@return 유효성 여부
*/
bool EvtxRecordValidator::isValidRecordHeader(void* arg)
{
	EVTX_RECORD_HEADER* recordHeader = (EVTX_RECORD_HEADER*)arg;
	uint8_t* recordData = (uint8_t*)arg;

	// 레코드의 magic값 확인
	if (recordHeader->magic != EVTX_RECORD_HEADER_MAGIC) return false;

	// 레코드의 크기가 청크 크기보다 클 수 없다
	if (recordHeader->length1 > EVTX_CHUNK_SIZE - EVTX_CHUNK_HEADER_SIZE) return false;

	// 레코드의 크기가 기본 헤더크기보다 작을 수 없다
	if (recordHeader->length1 < 28) return false;

	// xml스트림의 시작 여부 확인
	return isValidBinXmlPreamble(recordData + EVTX_RECORD_HEADER_SIZE);
}

/**
	@brief 해당하는 버퍼가 바이너리 xml의 시작점인지 확인한다
	@param arg 메모리 버퍼
	@return 유효성 여부
*/
bool EvtxRecordValidator::isValidBinXmlPreamble(void* arg)
{
	uint8_t* xmlData = (uint8_t*)arg;

	// BinXmlPI
	if (xmlData[0] == 0x0a || xmlData[0] == 0x0b) { return true; }

	// Fragment
	return (xmlData[0] == 0x0f && xmlData[1] == 0x01 && xmlData[2] == 0x01 && xmlData[3] == 0x00);
}