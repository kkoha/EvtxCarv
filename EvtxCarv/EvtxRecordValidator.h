/**
	@file EvtxRecordValidator.h
	@brief EVTX의 레코드 헤더/데이터의 유효성을 검증하기 위한 클래스의 정의
*/

#ifndef _EVTX_RECORD_VALIDATOR_H_
#define _EVTX_RECORD_VALIDATOR_H_

/**
	@class EvtxRecordValidator
	@brief EVTX파일의 레코드 헤더/데이터의 유효성을 검증하기 위한 클래스
*/
class EvtxRecordValidator
{
	public:
		static bool isValidRecord(void* arg, uint64_t bufferSize);
		static bool isValidRecordHeader(void* arg);
		static bool isValidBinXmlPreamble(void* arg);
};

#endif /* _EVTX_RECORD_VALIDATOR_H_ */