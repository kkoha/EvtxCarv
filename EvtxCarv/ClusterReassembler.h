/**
	@file ClusterReassembler.h
	@brief 파편화된 클러스터 재조립을 위한 클래스의 정의
*/

#ifndef _CHUNK_REASSEMBLER_H_
#define _CHUNK_REASSEMBLER_H_

#include "EvtxChunkHeader.h"
#include "EvtxChunkValidator.h"
#include "EvtxRecordHeader.h"
#include "EvtxRecordValidator.h"

#include "BinaryFile.h"

#include <tchar.h>
#include <string>
#include <hash_set>

/**
	@class ClusterReassembler
	@brief 파편화된 클러스터들을 붙혀주는 클래스(청크 기반)
*/
class ClusterReassembler
{
	public:
		void setFragmentedFile(BinaryFile& inFragmentedFile);
		void setOutputFile(BinaryFile& inOutFile);

		bool reassembleCluster(CarvedAreas& inCarvedAreas);

	private:
		void buildChunkInfos(CarvedAreas& inChunkCarvedAreas, ClusterChunkPairs& outClusterChunkPairs);
		void buildRecordInfoMap(CarvedAreas& inRecordCarvedAreas, RecordInfoMap& outRecorInfoMap, stdext::hash_set<uint64_t>& outClustersHaveRecord);
		std::vector<uint64_t> findBestMatchingClusters(uint64_t inClusterNo, std::vector<CLUSTER_INFO>& inClusterCandidates, const stdext::hash_set<uint64_t>& inClustersHaveRecord, bool& outAlreadyAddedCluster);
		std::vector<CLUSTER_INFO> findClusterCandidates(RecordInfoMap& inRecordClusterMap, uint64_t inRecordId, uint64_t& outClusterRecordId);

		BinaryFile fragmentedFile_;  ///< 조각난 파일(원본)
		BinaryFile outFile_;  ///< 조각난 데이터가 재조립된 파일
};

#endif /* _CHUNK_REASSEMBLER_H_ */