/**
	@file EvtxRecordValidator.h
	@brief EVTX�� ���ڵ� ���/�������� ��ȿ���� �����ϱ� ���� Ŭ������ ����
*/

#ifndef _EVTX_RECORD_VALIDATOR_H_
#define _EVTX_RECORD_VALIDATOR_H_

/**
	@class EvtxRecordValidator
	@brief EVTX������ ���ڵ� ���/�������� ��ȿ���� �����ϱ� ���� Ŭ����
*/
class EvtxRecordValidator
{
	public:
		static bool isValidRecord(void* arg, uint64_t bufferSize);
		static bool isValidRecordHeader(void* arg);
		static bool isValidBinXmlPreamble(void* arg);
};

#endif /* _EVTX_RECORD_VALIDATOR_H_ */