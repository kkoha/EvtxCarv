/**
	@file EvtxRecordValidator.cpp
	@brief EVTX�� ���ڵ� ���/�������� ��ȿ���� �����ϱ� ���� Ŭ������ ����
*/

#include "EvtxRecordHeader.h"
#include "EvtxRecordValidator.h"
#include "EvtxChunkHeader.h"
#include <stdio.h>

/**
	@brief �ش��ϴ� ���۰� ��ȿ�� ���ڵ����� Ȯ���Ѵ�

	@param arg �޸� ����
	@param bufferSize ������ ũ��

	@return ��ȿ�� ����
*/
bool EvtxRecordValidator::isValidRecord(void* arg, uint64_t bufferSize)
{
	EVTX_RECORD_HEADER* recordHeader = (EVTX_RECORD_HEADER*)arg;
	uint8_t* recordData = (uint8_t*)arg;

	// ���ڵ��� magic�� Ȯ��
	if (recordHeader->magic != EVTX_RECORD_HEADER_MAGIC) return false;

	// ���ڵ��� ũ�Ⱑ ûũ ũ�⺸�� Ŭ �� ����
	if (recordHeader->length1 > EVTX_CHUNK_SIZE - EVTX_CHUNK_HEADER_SIZE) return false;

	// ���ڵ��� ũ�Ⱑ �⺻ ���ũ�⺸�� ���� �� ����
	if (recordHeader->length1 < 28) return false;

	// ���ڵ��� ũ�Ⱑ ���� Ȯ�ΰ����� ����ũ�⺸�� ũ�� xml��Ʈ���� ���� ���θ� Ȯ���Ѵ�
	// �ƴϸ� ���ڵ��� ���� ���ڵ��� ����� ��Ȯ�ϰ� �������� Ȯ���Ѵ�
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
	@brief �ش��ϴ� ���۰� ��ȿ�� ���ڵ� ������� �Ǻ��Ѵ�
	@param arg �޸� ����
	@return ��ȿ�� ����
*/
bool EvtxRecordValidator::isValidRecordHeader(void* arg)
{
	EVTX_RECORD_HEADER* recordHeader = (EVTX_RECORD_HEADER*)arg;
	uint8_t* recordData = (uint8_t*)arg;

	// ���ڵ��� magic�� Ȯ��
	if (recordHeader->magic != EVTX_RECORD_HEADER_MAGIC) return false;

	// ���ڵ��� ũ�Ⱑ ûũ ũ�⺸�� Ŭ �� ����
	if (recordHeader->length1 > EVTX_CHUNK_SIZE - EVTX_CHUNK_HEADER_SIZE) return false;

	// ���ڵ��� ũ�Ⱑ �⺻ ���ũ�⺸�� ���� �� ����
	if (recordHeader->length1 < 28) return false;

	// xml��Ʈ���� ���� ���� Ȯ��
	return isValidBinXmlPreamble(recordData + EVTX_RECORD_HEADER_SIZE);
}

/**
	@brief �ش��ϴ� ���۰� ���̳ʸ� xml�� ���������� Ȯ���Ѵ�
	@param arg �޸� ����
	@return ��ȿ�� ����
*/
bool EvtxRecordValidator::isValidBinXmlPreamble(void* arg)
{
	uint8_t* xmlData = (uint8_t*)arg;

	// BinXmlPI
	if (xmlData[0] == 0x0a || xmlData[0] == 0x0b) { return true; }

	// Fragment
	return (xmlData[0] == 0x0f && xmlData[1] == 0x01 && xmlData[2] == 0x01 && xmlData[3] == 0x00);
}