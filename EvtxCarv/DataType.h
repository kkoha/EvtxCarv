/**
	@file DataType.h
	@brief ���� ������ Ÿ������ �����س��� ����
*/

#ifndef _DATA_TYPE_H_
#define _DATA_TYPE_H_

#include <map>
#include <list>
#include <hash_map>
#include <hash_set>

typedef unsigned __int64 uint64_t;  ///< ��ȣ���� 64��Ʈ ����
typedef __int64 int64_t;  ///< ��ȣ�ִ� 64��Ʈ ����
typedef unsigned __int32 uint32_t;  ///< ��ȣ���� 32��Ʈ ����
typedef __int32 int32_t;  ///< ��ȣ�ִ� 32��Ʈ ����
typedef unsigned __int16 uint16_t;  ///< ��ȣ���� 16��Ʈ ����
typedef __int16 int16_t;  ///< ��ȣ�ִ� 16��Ʈ ����
typedef unsigned __int8 uint8_t;  ///< ��ȣ���� 8��Ʈ ����
typedef __int8 int8_t;  ///< ��ȣ�ִ� 8��Ʈ ����

/**
	@class CHUNK_INFO_FILE_TYPE
	@brief ûũ�� �����Ͱ� �Է� ���Ͽ��� �Դ���, �����յ� ���Ͽ��� �Դ���
*/
enum CHUNK_INFO_FILE_TYPE
{
	FILE_TYPE_NORMAL = 0,  ///< ûũ�� �����Ͱ� �Է� ���Ͽ��� ����
	FILE_TYPE_FRAGMENTED = 1,  ///< ûũ�� �����Ͱ� �����յ� ���Ͽ��� ����
};

/**
	@class ChunkInfo
	@brief ���յ� ûũ�� ����
*/
class ChunkInfo
{
	public:
		ChunkInfo(CHUNK_INFO_FILE_TYPE inFileType, uint64_t inChunkPos, uint64_t inFirstRecord, uint64_t inLastRecord,
			const std::wstring& inComputerName, const std::wstring& inChannelName)
		{
			fileType = inFileType;
			chunkPos = inChunkPos;
			firstRecord = inFirstRecord;
			lastRecord = inLastRecord;
			computerName = inComputerName;
			channelName = inChannelName;
		}

		CHUNK_INFO_FILE_TYPE fileType;  ///< ûũ�� �����Ͱ� �Է��������� �����յ� ��������
		std::wstring computerName;  ///< ûũ�� ��ǻ�� �̸�
		std::wstring channelName;  ///< ûũ�� ä�� �̸�
		uint64_t chunkPos;  ///< ûũ�� ���Ͽ����� ��ġ
		uint64_t firstRecord;  ///< ûũ�� ������ �ִ� ù ��° ���ڵ� ��ȣ
		uint64_t lastRecord;  ///< ûũ�� ������ �ִ� ������ ���ڵ� ��ȣ
};

/**
	@class CLUSTER_INFO
	@brief Ư�� Ŭ�������� ����(Ŭ������ ��ȣ / Ŭ������ �������� crc)
*/
struct CLUSTER_INFO
{
	CLUSTER_INFO(uint64_t inClusterNo, uint32_t inClusterCrc)
	{
		clusterNo = inClusterNo;
		clusterCrc = inClusterCrc;
	}

	uint64_t clusterNo;  ///< Ŭ������ ��ȣ
	uint32_t clusterCrc;  ///< Ŭ�������� CRC
};

/**
	@class SUBSTITUTION_INFO
	@brief ���ø�/�ν��Ͻ����� Substitution ���� ����
*/
struct SUBSTITUTION_INFO
{
	SUBSTITUTION_INFO(uint8_t inValueType, uint8_t inIsOptional)
	{
		valueType = inValueType;
		isOptional = inIsOptional;
	}

	bool operator==(const SUBSTITUTION_INFO& oth) const
	{
		return valueType == oth.valueType && isOptional == oth.isOptional;
	}

	uint8_t valueType;  ///< Substitution�� �� Ÿ��
	uint8_t isOptional;  ///< Optional Substitution���� ����
};

/**
	@class TemplateSubstitutionMap
	@brief Substitution ID�� ������ ������ �̷���� ��(���ø� ���ǿ��� ���)
*/
typedef std::map<uint32_t, SUBSTITUTION_INFO> TemplateSubstitutionMap;

/**
	@class TemplateInstanceInfos
	@brief Substitution ������ ����(���ø� �ν��Ͻ����� ���)
*/
typedef std::vector<SUBSTITUTION_INFO> TemplateInstanceInfos;

class BinXmlTemplate;

/**
	@class TemplateList
	@brief ���ø� Substitution ������ ���ڵ� ���̵� ������� ���ø����� ��Ƴ��� �÷���
*/
typedef std::vector<std::pair<TemplateSubstitutionMap, std::map<uint64_t, BinXmlTemplate> > > TemplateList;

/**
	@class SortedChunk
	@brief ûũ�� ���ڵ� ��ȣ �������� ���ĵ� ûũ�� ����
*/
typedef std::multimap<uint64_t, ChunkInfo> SortedChunk;

/**
	@class ChunkInfos
	@brief ûũ�� ������ ��Ƴ��� ����Ʈ
*/
typedef std::list<ChunkInfo> ChunkInfos;

/**
	@class CarvedAreas
	@brief �̹� ī���� ����
*/
typedef std::list<std::pair<uint64_t, uint64_t> > CarvedAreas;

/**
	@class SortedRecord
	@brief ��ȣ�� ������� ���ĵ� ���ڵ�
*/
typedef std::multimap<uint64_t, uint64_t> SortedRecord;

/**
	@class RecordPoses
	@brief ���ڵ��� ���Ͽ����� ��ġ��
*/
typedef std::list<uint64_t> RecordPoses;

/**
	@class RecordInfoMap
	@brief ���ڵ��ȣ -> Ŭ������ ������ �ٲ��ִ� ��
*/
typedef std::map<uint64_t, std::vector<CLUSTER_INFO> > RecordInfoMap;

/**
	@class GroupedChunk
	@brief ���� �з��� ûũ�� ����(��ǻ�� �̸�/ä���̸� ������� �׷���)
*/
class GroupedChunk
{
	public:
		std::wstring computerName;  ///< �׷��� ��ǻ�� �̸�
		std::wstring channelName;  ///< �׷��� ä�� �̸�
		SortedChunk sortedChunks;  ///< �׷쳻�� ûũ��
};

struct EVTX_CHUNK_HEADER;

/**
	@class GroupedChunks
	@brief ûũ �׷��� ���
*/
typedef std::list<GroupedChunk> GroupedChunks;

/**
	@class ClusterChunkPairs
	@brief Ŭ������ ��ȣ�� ûũ�� �����ϴ� �ڷᱸ��
*/
typedef std::vector<std::pair<uint64_t, EVTX_CHUNK_HEADER> > ClusterChunkPairs;

typedef void (*CarveInfoCallback)(const std::wstring& inCarveInfo);

#define DISK_SECTOR_SIZE 512  ///< ��ũ ������ �ּ� ����
#define MAX_PATH_SIZE 4096  ///< ����� �ִ� ����
#define MMAP_SIZE (256*1024*1024)  ///< SignatureFinder���� �޸� ���� ����
#define CLUSTER_SIZE 4096  ///< Ŭ�������� ũ��
#define CLUSTER_NUM_NULL 0xFFFFFFFFFFFFFFFFll  ///< �������� ���� Ŭ������ ��ȣ

#endif /* _DATA_TYPE_H_ */
