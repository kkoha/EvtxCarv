/**
	@file EvtxChunkHeader.h
	@brief EVTXûũ�� ��� ������ ���õ� ����/����ü��
*/

#ifndef _EVTX_CHUNK_HEADER_
#define _EVTX_CHUNK_HEADER_

#include "DataType.h"

#define EVTX_CHUNK_HEADER_MAGIC 0x006B6E6843666C45LL /* "ElfChnk", 00 */ ///< EVTXûũ����� ������ ElfChnk,0
#define EVTX_CHUNK_SIZE 0x10000  ///< EVTXûũ�� ��ü ������
#define EVTX_CHUNK_HEADER_SIZE 0x200  ///< EVTXûũ ����� ������

#pragma pack(1)

/**
	@class EVTX_CHUNK_HEADER
	@brief EVTX ������ ûũ ��� ����
*/
struct EVTX_CHUNK_HEADER
{
	uint64_t magic;  ///< ������
	uint64_t numLogRecFirst;  ///< ûũ ���� ���ڵ��� ��ȣ
	uint64_t numLogRecLast;  ///< ûũ�� ������ ���ڵ��� ��ȣ
	uint64_t numFileRecFirst;  ///< ûũ ���� ���ڵ��� ��ȣ
	uint64_t numFileRecLast;  ///< ûũ�� ������ ���ڵ��� ��ȣ
	uint32_t ofsTables;  ///< ��Ʈ��/���ø� ���̺��� ������
	uint32_t ofsRecLast;  ///< ������ ���ڵ��� ������
	uint32_t ofsRecNext;  ///< ������ ���ڵ尡 ������ ������
	uint32_t dataCRC;  ///< ûũ �������� üũ��
	char unknown[68];
	uint32_t headerCRC;  ///< ûũ ����� üũ��
	uint32_t stringTable[64];  ///< ��Ʈ���� ������ ���̺�
	uint32_t templateTable[32];  ///< ���ø��� ������ ���̺�
};

#pragma pack()

#endif _EVTX_CHUNK_HEADER_