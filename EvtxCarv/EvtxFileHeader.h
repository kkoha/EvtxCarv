/**
	@file EvtxFileHeader.h
	@brief EVTX������ ��� ������ ���õ� ����/����ü��
*/

#ifndef _EVTX_FILE_HEADER_H_
#define _EVTX_FILE_HEADER_H_

#define EVTX_FILE_HEADER_MAGIC 0x00656C6946666C45LL /* "ElfFile", 00 */ ///< EVTX��������� ������ ElfFile,0
#define EVTX_FILE_HEADER_SIZE 0x1000 ///< EVTX���� ����� ũ��

#pragma pack(1)

/**
	@class EVTX_FILE_HEADER
	@brief EVTX ������ ���� ��� ����
*/
struct EVTX_FILE_HEADER
{
	uint64_t magic;  ///< ������
	int64_t oldestChunk;  ///< oldestChunk
	int64_t currentChunkNum;  ///< chrrentChunkNum
	int64_t nextRecordNum;  ///< ���� ���ڵ��� ��ȣ
	uint32_t headerPart1Len;  ///< ���� ����� ����(�ǹ��ִ� ������)
	unsigned __int16 minorVersion;  ///< ���� ����� ����(major)
	unsigned __int16 majorVersion;  ///< ���� ����� ����(minor)
	unsigned __int16 headerSize;  ///< ��� ��ü�� ����
	unsigned __int16 chunkCount;  ///< ������ �ִ� ûũ�� ��
	char unknown[76];
	uint32_t flags; ///< �÷���
	uint32_t checkSum;  ///< ����� üũ��
};

#pragma pack()

#endif /* _EVTX_FILE_HEADER_H_ */