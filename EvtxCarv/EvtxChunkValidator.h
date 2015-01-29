/**
	@file EvtxChunkValidator.h
	@brief EVTX�� ûũ�� ���/�������� ��ȿ���� �����ϱ� ���� Ŭ������ ����
*/

#ifndef _EVTX_CHUNK_VALIDATOR_H_
#define _EVTX_CHUNK_VALIDATOR_H_

/**
	@class EvtxChunkValidator
	@brief EVTXûũ�� ���/������ ��ȿ���� �����ϱ� ���� Ŭ����
*/
class EvtxChunkValidator
{
	public:
		static bool isValidChunk(void* arg);
		static bool isValidChunkHeader(void* arg);
};

#endif _EVTX_CHUNK_VALIDATOR_H_