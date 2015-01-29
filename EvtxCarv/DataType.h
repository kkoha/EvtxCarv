/**
	@file DataType.h
	@brief 각종 데이터 타입을을 정의해놓은 파일
*/

#ifndef _DATA_TYPE_H_
#define _DATA_TYPE_H_

#include <map>
#include <list>
#include <hash_map>
#include <hash_set>

typedef unsigned __int64 uint64_t;  ///< 부호없는 64비트 정수
typedef __int64 int64_t;  ///< 부호있는 64비트 정수
typedef unsigned __int32 uint32_t;  ///< 부호없는 32비트 정수
typedef __int32 int32_t;  ///< 부호있는 32비트 정수
typedef unsigned __int16 uint16_t;  ///< 부호없는 16비트 정수
typedef __int16 int16_t;  ///< 부호있는 16비트 정수
typedef unsigned __int8 uint8_t;  ///< 부호없는 8비트 정수
typedef __int8 int8_t;  ///< 부호있는 8비트 정수

/**
	@class CHUNK_INFO_FILE_TYPE
	@brief 청크의 데이터가 입력 파일에서 왔는지, 재조합된 파일에서 왔는지
*/
enum CHUNK_INFO_FILE_TYPE
{
	FILE_TYPE_NORMAL = 0,  ///< 청크의 데이터가 입력 파일에서 왔음
	FILE_TYPE_FRAGMENTED = 1,  ///< 청크의 데이터가 재조합된 파일에서 왔음
};

/**
	@class ChunkInfo
	@brief 조합될 청크의 정보
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

		CHUNK_INFO_FILE_TYPE fileType;  ///< 청크의 데이터가 입력파일인지 재조합된 파일인지
		std::wstring computerName;  ///< 청크의 컴퓨터 이름
		std::wstring channelName;  ///< 청크의 채널 이름
		uint64_t chunkPos;  ///< 청크의 파일에서의 위치
		uint64_t firstRecord;  ///< 청크가 가지고 있는 첫 번째 레코드 번호
		uint64_t lastRecord;  ///< 청크가 가지고 있는 마지막 레코드 번호
};

/**
	@class CLUSTER_INFO
	@brief 특정 클러스터의 정보(클러스터 번호 / 클러스터 데이터의 crc)
*/
struct CLUSTER_INFO
{
	CLUSTER_INFO(uint64_t inClusterNo, uint32_t inClusterCrc)
	{
		clusterNo = inClusterNo;
		clusterCrc = inClusterCrc;
	}

	uint64_t clusterNo;  ///< 클러스터 번호
	uint32_t clusterCrc;  ///< 클러스터의 CRC
};

/**
	@class SUBSTITUTION_INFO
	@brief 템플릿/인스턴스에서 Substitution 관련 정보
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

	uint8_t valueType;  ///< Substitution의 값 타입
	uint8_t isOptional;  ///< Optional Substitution인지 여부
};

/**
	@class TemplateSubstitutionMap
	@brief Substitution ID와 정보의 쌍으로 이루어진 맵(템플릿 정의에서 사용)
*/
typedef std::map<uint32_t, SUBSTITUTION_INFO> TemplateSubstitutionMap;

/**
	@class TemplateInstanceInfos
	@brief Substitution 정보의 모음(템플릿 인스턴스에서 사용)
*/
typedef std::vector<SUBSTITUTION_INFO> TemplateInstanceInfos;

class BinXmlTemplate;

/**
	@class TemplateList
	@brief 템플릿 Substitution 정보와 레코드 아이디 기반으로 템플릿들을 모아놓은 컬렉션
*/
typedef std::vector<std::pair<TemplateSubstitutionMap, std::map<uint64_t, BinXmlTemplate> > > TemplateList;

/**
	@class SortedChunk
	@brief 청크의 레코드 번호 기준으로 정렬된 청크의 집합
*/
typedef std::multimap<uint64_t, ChunkInfo> SortedChunk;

/**
	@class ChunkInfos
	@brief 청크의 정보를 모아놓는 리스트
*/
typedef std::list<ChunkInfo> ChunkInfos;

/**
	@class CarvedAreas
	@brief 이미 카빙된 영역
*/
typedef std::list<std::pair<uint64_t, uint64_t> > CarvedAreas;

/**
	@class SortedRecord
	@brief 번호를 기반으로 정렬된 레코드
*/
typedef std::multimap<uint64_t, uint64_t> SortedRecord;

/**
	@class RecordPoses
	@brief 레코드의 파일에서의 위치들
*/
typedef std::list<uint64_t> RecordPoses;

/**
	@class RecordInfoMap
	@brief 레코드번호 -> 클러스터 정보로 바꿔주는 맵
*/
typedef std::map<uint64_t, std::vector<CLUSTER_INFO> > RecordInfoMap;

/**
	@class GroupedChunk
	@brief 같은 분류의 청크의 묶음(컴퓨터 이름/채널이름 기반으로 그루핑)
*/
class GroupedChunk
{
	public:
		std::wstring computerName;  ///< 그룹의 컴퓨터 이름
		std::wstring channelName;  ///< 그룹의 채널 이름
		SortedChunk sortedChunks;  ///< 그룹내의 청크들
};

struct EVTX_CHUNK_HEADER;

/**
	@class GroupedChunks
	@brief 청크 그룹의 목록
*/
typedef std::list<GroupedChunk> GroupedChunks;

/**
	@class ClusterChunkPairs
	@brief 클러스터 번호를 청크로 매핑하는 자료구조
*/
typedef std::vector<std::pair<uint64_t, EVTX_CHUNK_HEADER> > ClusterChunkPairs;

typedef void (*CarveInfoCallback)(const std::wstring& inCarveInfo);

#define DISK_SECTOR_SIZE 512  ///< 디스크 접근의 최소 단위
#define MAX_PATH_SIZE 4096  ///< 경로의 최대 길이
#define MMAP_SIZE (256*1024*1024)  ///< SignatureFinder에서 메모리 매핑 단위
#define CLUSTER_SIZE 4096  ///< 클러스터의 크기
#define CLUSTER_NUM_NULL 0xFFFFFFFFFFFFFFFFll  ///< 유요하지 않은 클러스터 번호

#endif /* _DATA_TYPE_H_ */
