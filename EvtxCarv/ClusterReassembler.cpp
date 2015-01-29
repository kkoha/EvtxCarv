/**
	@file ClusterReassembler.cpp
	@brief 파편화된 클러스터 재조립을 위한 클래스의 구현
*/

#include "ClusterReassembler.h"
#include "DataType.h"
#include "SignatureFinder.h"
#include "crc32.h"
#include "BinXmlStream.h"
#include <hash_map>
#include <hash_set>
#include <vector>

/**
	@brief 재조립할 파일을 정한다
	@param inFragmentedFile 조각난 파일
*/
void ClusterReassembler::setFragmentedFile(BinaryFile& inFragmentedFile)
{
	fragmentedFile_ = inFragmentedFile;
}

/**
	@brief 재조립된 파일의 내용이 출력될 파일을 정한다
	@param inOutFile 출력할 파일
*/
void ClusterReassembler::setOutputFile(BinaryFile& inOutFile)
{
	outFile_ = inOutFile;
}

/**
	@brief 청크/레코드를 기본으로 클러스터들을 올바른 순서대로 최대한 재조립한다

	@param inCarvedAreas 재조립할 정보를 얻을 때 스킵할 영역
*/
bool ClusterReassembler::reassembleCluster(CarvedAreas& inCarvedAreas)
{
	stdext::hash_set<uint64_t> clustersHaveRecord;
	stdext::hash_set<uint64_t> processedClusters;
	ClusterChunkPairs clusterChunkPairs;

	RecordInfoMap recordInfoMap;
	RecordInfoMap::iterator recordInfoIterator;
	RecordInfoMap::iterator candidateRecordInfoIterator;
	uint8_t clusterBuffer[CLUSTER_SIZE] = {0,};
	uint64_t outputFilePos = 0;

	buildChunkInfos(inCarvedAreas, clusterChunkPairs);

	buildRecordInfoMap(inCarvedAreas, recordInfoMap, clustersHaveRecord);

	// 헤더는 유효하지만 이전에 카빙되지 않은 청크(아마도 조각난)들에 대하여
	for (uint32_t clusterChunkPairIndex = 0; clusterChunkPairIndex < clusterChunkPairs.size(); ++clusterChunkPairIndex)
	{
		std::vector<uint64_t> chunkClusters;
		std::vector<uint64_t> nextClusters;
		std::vector<CLUSTER_INFO> clusterCandidates;
		bool alreadyAddedCluster = false;

		chunkClusters.clear();

		// 청크 헤더의 각 레코드별로
		for (uint64_t recordId = clusterChunkPairs[clusterChunkPairIndex].second.numLogRecFirst;
			recordId <= clusterChunkPairs[clusterChunkPairIndex].second.numLogRecLast; ++recordId)
		{
			uint64_t clusterRecordId = 0;

			// 해당 레코드 아이디를 가지는 클러스터(뒤에 추가될 클러스터 후보군)들을 찾는다
			clusterCandidates = findClusterCandidates(recordInfoMap, recordId, clusterRecordId);

			if (!clusterCandidates.empty())
			{
				if (chunkClusters.empty())
				{
					chunkClusters.push_back(clusterChunkPairs[clusterChunkPairIndex].first);
				}

				// 이번에 집어넣을 클러스터 후보군 중에 가장 적합한 클러스터를 찾는다
				nextClusters = findBestMatchingClusters(chunkClusters.back(), clusterCandidates, clustersHaveRecord, alreadyAddedCluster);

				chunkClusters.insert(chunkClusters.end(), nextClusters.begin(), nextClusters.end());

				uint32_t chunkClusterIndex = 0;
				for (chunkClusterIndex = 0; chunkClusterIndex < chunkClusters.size(); ++chunkClusterIndex)
				{
					if (processedClusters.find(chunkClusters[chunkClusterIndex]) != processedClusters.end())
					{
						break;
					}
				}

				if (chunkClusterIndex != chunkClusters.size()) { break; }
			}
		}

		// 조립된 클러스터들을 파일에 기록한다
		for (uint32_t chunkClusterIndex = 0; chunkClusterIndex < chunkClusters.size(); ++chunkClusterIndex)
		{
			if (processedClusters.find(chunkClusters[chunkClusterIndex]) == processedClusters.end())
			{
				processedClusters.insert(chunkClusters[chunkClusterIndex]);

				fragmentedFile_.readData(chunkClusters[chunkClusterIndex] * CLUSTER_SIZE, clusterBuffer, CLUSTER_SIZE);
				outFile_.writeData(outputFilePos, clusterBuffer, CLUSTER_SIZE);
				outputFilePos += CLUSTER_SIZE;
			}
		}
	}

	for (recordInfoIterator = recordInfoMap.begin(); recordInfoIterator != recordInfoMap.end(); ++recordInfoIterator)
	{
		std::vector<uint64_t> nextClusters;
		std::vector<uint64_t> chunkClusters;
		std::vector<CLUSTER_INFO> clusterCandidates;
		uint64_t clusterRecordId = 0;
		bool alreadyAddedCluster = false;
		uint32_t testedRecordCount = 0;

		for (uint32_t clusterIndex = 0; clusterIndex < recordInfoIterator->second.size(); ++clusterIndex)
		{
			chunkClusters.clear();

			// 앞의 알고리즘에서 처리되지 못한 클러스터들은
			// (청크 헤더가 날아갔을 가능성 혹은 중간이 날아가 앞의 레코드와 조립되지 못했을수도)
			if (processedClusters.find(recordInfoIterator->second[clusterIndex].clusterNo) == processedClusters.end())
			{
				chunkClusters.push_back(recordInfoIterator->second[clusterIndex].clusterNo);
				
				for (candidateRecordInfoIterator = recordInfoIterator; candidateRecordInfoIterator != recordInfoMap.end(); ++candidateRecordInfoIterator)
				{
					// 다시 적합한 클러스터들을 찾아보고
					clusterCandidates = findClusterCandidates(recordInfoMap, candidateRecordInfoIterator->first, clusterRecordId);
					nextClusters = findBestMatchingClusters(chunkClusters.back(), clusterCandidates, clustersHaveRecord, alreadyAddedCluster);

					//printf("Record %llu %d\n", candidateRecordInfoIterator->first, chunkClusters.size());

					if (!alreadyAddedCluster && nextClusters.size() == 0)
					{
						break;
					}

					chunkClusters.insert(chunkClusters.end(), nextClusters.begin(), nextClusters.end());

					uint32_t chunkClusterIndex = 0;
					for (chunkClusterIndex = 0; chunkClusterIndex < chunkClusters.size(); ++chunkClusterIndex)
					{
						if (processedClusters.find(chunkClusters[chunkClusterIndex]) != processedClusters.end())
						{
							break;
						}
					}

					if (chunkClusterIndex != chunkClusters.size()) { break; }
				}

				for (uint32_t chunkClusterIndex = 0; chunkClusterIndex < chunkClusters.size(); ++chunkClusterIndex)
				{
					// 파일에 기록한다(이 때 중복을 피하기 위해 이미 처리된 클러스터는 넣지 않는다)
					if (processedClusters.find(chunkClusters[chunkClusterIndex]) == processedClusters.end())
					{
						processedClusters.insert(chunkClusters[chunkClusterIndex]);
						fragmentedFile_.readData(chunkClusters[chunkClusterIndex] * CLUSTER_SIZE, clusterBuffer, CLUSTER_SIZE);
						outFile_.writeData(outputFilePos, clusterBuffer, CLUSTER_SIZE);
						outputFilePos += CLUSTER_SIZE;
					}
				}
			}
		}
	}

	return true;
}

/**
	@brief 파일로부터 청크의 정보들을 모은다

	@param inChunkCarvedAreas 스킵할 영역
	@param outClusterChunkPairs 클러스터 번호/청크정보 의 쌍으로 된 리스트
*/
void ClusterReassembler::buildChunkInfos(CarvedAreas& inCarvedAreas, ClusterChunkPairs& outClusterChunkPairs)
{
	SignatureFinder chunkHeaderFinder(fragmentedFile_, EVTX_CHUNK_HEADER_MAGIC, 8, 16, &inCarvedAreas, EVTX_CHUNK_HEADER_SIZE);
	uint8_t* chunkBuffer = NULL;
	EVTX_CHUNK_HEADER* evtxChunkHeader = NULL;
	uint64_t filePos = 0;

	while ((chunkBuffer = chunkHeaderFinder.getNext(&filePos)) != NULL)
	{
		evtxChunkHeader = (EVTX_CHUNK_HEADER*)chunkBuffer;

		if (EvtxChunkValidator::isValidChunkHeader(chunkBuffer))
		{
			outClusterChunkPairs.push_back(std::make_pair(filePos / CLUSTER_SIZE, *evtxChunkHeader));
		}
	}

	chunkHeaderFinder.closeMap();
}

/**
	@brief 파일로부터 레코드의 정보들을 모은다

	@param inRecordCarvedAreas 스킵할 영역
	@param outRecordInfoMap 클러스터 번호 / 레코드정보 쌍으로 된 맵
	@param outClustersHaveRecord 레코드를 가지고 있는 클러스터들
*/
void ClusterReassembler::buildRecordInfoMap(CarvedAreas& inCarvedAreas, RecordInfoMap& outRecordInfoMap, stdext::hash_set<uint64_t>& outClustersHaveRecord)
{
	SignatureFinder recordHeaderFinder(fragmentedFile_, EVTX_RECORD_HEADER_MAGIC, 4, 8, &inCarvedAreas, EVTX_RECORD_HEADER_SIZE);
	uint8_t* recordBuffer = NULL;
	EVTX_RECORD_HEADER* evtxRecordHeader = NULL;
	uint64_t foundFilePos = 0;
	uint64_t clusterNumber = 0;
	uint32_t clusterCrc = 0;
	uint8_t clusterBuffer[CLUSTER_SIZE] = {0,};
	uint32_t clusterIndex = 0;

	outClustersHaveRecord.clear();

	while ((recordBuffer = recordHeaderFinder.getNext(&foundFilePos)) != NULL)
	{
		evtxRecordHeader = (EVTX_RECORD_HEADER*)recordBuffer;

		if (EvtxRecordValidator::isValidRecordHeader(recordBuffer))
		{
			clusterNumber = foundFilePos / CLUSTER_SIZE;
			fragmentedFile_.readData(clusterNumber * CLUSTER_SIZE, clusterBuffer, CLUSTER_SIZE);

			clusterCrc = update_crc32(0, clusterBuffer, CLUSTER_SIZE);

			if (outClustersHaveRecord.find(clusterNumber) == outClustersHaveRecord.end())
			{
				outClustersHaveRecord.insert(clusterNumber);
			}

			// 레코드와 클러스터를 매핑중 중복된 레코드 아이디가 있으면
			if (outRecordInfoMap.find(evtxRecordHeader->numLogRecord) != outRecordInfoMap.end())
			{
				bool crcFound = false;

				// 클러스터의 crc를 비교하여 없는 crc이면 해당 클러스터를 레코드 아이디의 클러스터 후보군에 추가
				for (clusterIndex = 0; clusterIndex < outRecordInfoMap[evtxRecordHeader->numLogRecord].size(); ++clusterIndex)
				{
					if (outRecordInfoMap[evtxRecordHeader->numLogRecord][clusterIndex].clusterCrc == clusterCrc)
					{
						crcFound = true;
						break;
					}
				}

				if (!crcFound)
				{
					outRecordInfoMap[evtxRecordHeader->numLogRecord].push_back(CLUSTER_INFO(clusterNumber, clusterCrc));
				}
			}
			else
			{
				std::vector<CLUSTER_INFO> clusterInfos;

				clusterInfos.push_back(CLUSTER_INFO(clusterNumber, clusterCrc));
				outRecordInfoMap[evtxRecordHeader->numLogRecord] = clusterInfos;
			}
		}
	}

	recordHeaderFinder.closeMap();
}

/**
	@brief 클러스터의 후보들 중 현재 클러스터에 가장 적합하게 연결될 클러스터를 찾는다

	@param inClusterNo 현재 클러스터 번호
	@param inClusterCandidates 뒤에 연결될 클러스터의 후보
	@param inClustersHaveRecord 레코드를 가지고 있는 클러스터
	@param outAlreadyAddedCluster 클러스터 후보들중에 이미 추가되기로 결정된 클러스터가 있는지 여부
*/
std::vector<uint64_t> ClusterReassembler::findBestMatchingClusters(uint64_t inClusterNo, std::vector<CLUSTER_INFO>& inClusterCandidates, const stdext::hash_set<uint64_t>& inClustersHaveRecord, bool& outAlreadyAddedCluster)
{
	std::vector<uint64_t> resultClusters;
	uint8_t clusterBuffer[CLUSTER_SIZE] = {0,};
	uint8_t* mergedClusterBuffer = NULL;
	uint32_t clusterCandidateIndex = 0;
	uint32_t recordPos = 0;
	uint32_t nextClusterBufferPos = 0;

	outAlreadyAddedCluster = false;

	for (clusterCandidateIndex = 0; clusterCandidateIndex < inClusterCandidates.size(); ++clusterCandidateIndex)
	{
		if (inClusterNo == inClusterCandidates[clusterCandidateIndex].clusterNo)
		{
			outAlreadyAddedCluster = true;
			break;
		}
	}

	if (!outAlreadyAddedCluster)
	{
		nextClusterBufferPos = CLUSTER_SIZE;
		fragmentedFile_.readData(inClusterNo * CLUSTER_SIZE, clusterBuffer, CLUSTER_SIZE);
		uint32_t lastRecordPos = 0;
		EVTX_RECORD_HEADER* lastRecord = NULL;

		// 현재 클러스터에서(1클러스터) 가장 마지막 레코드를 찾는다
		for (recordPos = 0; recordPos < CLUSTER_SIZE - EVTX_RECORD_HEADER_SIZE; recordPos += 8)
		{
			if (EvtxRecordValidator::isValidRecordHeader(clusterBuffer + recordPos))
			{
				lastRecordPos = recordPos;
				break;
			}
		}

		lastRecord = (EVTX_RECORD_HEADER*)(clusterBuffer + lastRecordPos);

		// 가장 마지막 레코드가 3개 이상의 클러스터에 걸쳐져 있으면
		if (lastRecordPos + lastRecord->length1 > CLUSTER_SIZE * 2)
		{
			uint32_t needClusterCount = 0;
			uint64_t checkClusterNo = 0;
			bool clustersHaveNoRecord = true;

			if (lastRecordPos + lastRecord->length1 % CLUSTER_SIZE == 0)
			{
				needClusterCount = ((lastRecordPos + lastRecord->length1) - CLUSTER_SIZE) / CLUSTER_SIZE;
			}
			else
			{
				needClusterCount = ((lastRecordPos + lastRecord->length1) - CLUSTER_SIZE) / CLUSTER_SIZE + 1;
			}

			mergedClusterBuffer = new uint8_t[CLUSTER_SIZE * (needClusterCount + 2)];

			for (checkClusterNo = inClusterNo + 1; checkClusterNo < inClusterNo + 1 + needClusterCount; ++checkClusterNo)
			{
				if (inClustersHaveRecord.find(checkClusterNo) != inClustersHaveRecord.end())
				{
					clustersHaveNoRecord = false;
					break;
				}
			}

			// 어쩔수 없이 레코드의 클러스터들이 연속되어 있다고 가정하고 맨처음과 마지막을 뺀 중간 클러스터 데이터들을 정한다
			if (clustersHaveNoRecord)
			{
				for (uint64_t insertClusterNo = inClusterNo + 1; insertClusterNo < inClusterNo + 1 + needClusterCount; ++insertClusterNo)
				{
					resultClusters.push_back(insertClusterNo);
					fragmentedFile_.readData(insertClusterNo * CLUSTER_SIZE, mergedClusterBuffer + nextClusterBufferPos, CLUSTER_SIZE);
					nextClusterBufferPos += CLUSTER_SIZE;
				}
			}
		}
		else
		{
			mergedClusterBuffer = new uint8_t[CLUSTER_SIZE * 2];
		}

		memcpy(mergedClusterBuffer, clusterBuffer, CLUSTER_SIZE);

		// 이어붙일 클러스터 후보군에 대하여
		for (clusterCandidateIndex = 0; clusterCandidateIndex < inClusterCandidates.size(); ++clusterCandidateIndex)
		{
			uint32_t mergedLastRecordPos = 0;
			EVTX_RECORD_HEADER* mergedLastRecord = NULL;

			// 실제로 클러스터들을 조립하여 보고
			fragmentedFile_.readData(inClusterCandidates[clusterCandidateIndex].clusterNo * CLUSTER_SIZE, (mergedClusterBuffer + nextClusterBufferPos), CLUSTER_SIZE);

			for (recordPos = 0; recordPos < CLUSTER_SIZE; recordPos += 8)
			{
				if (EvtxRecordValidator::isValidRecordHeader(mergedClusterBuffer + recordPos))
				{
					mergedLastRecordPos = recordPos;
					break;
				}
			}

			// 올바른 레코드가 되는지 확인하여 올바른 레코드가 되면 가장 적합한 클러스터라 정한다
			if (EvtxRecordValidator::isValidRecord(mergedClusterBuffer + mergedLastRecordPos, nextClusterBufferPos + CLUSTER_SIZE - mergedLastRecordPos))
			{
				mergedLastRecord = (EVTX_RECORD_HEADER*)(mergedClusterBuffer + mergedLastRecordPos);
				uint32_t parseSize = 0;

				BinXmlStream xmlStream(mergedClusterBuffer + mergedLastRecordPos + EVTX_RECORD_HEADER_SIZE,
					(mergedLastRecord->length1 - 0x1C > nextClusterBufferPos) ? nextClusterBufferPos : (mergedLastRecord->length1 - 0x1C), mergedLastRecord->numLogRecord);

				xmlStream.parse(&parseSize);

				if (parseSize > nextClusterBufferPos || mergedLastRecord->length1 - 0x1C - parseSize < 8)
				{
					resultClusters.push_back(inClusterCandidates[clusterCandidateIndex].clusterNo);
					break;
				}
			}
		}

		if (mergedClusterBuffer != NULL)
		{
			delete [] mergedClusterBuffer;
		}
	}

	return resultClusters;
}

/**
	@brief 특정 레코드 아이디 에 대응되는 클러스터의 후보군을 얻는다

	@param inRecordClusterMap 레코드 아이디 / 클러스터 후보군 대응 맵
	@param inRecordId 레코드 아이디
	@param outClusterRecordId 실제 클러스터 후보군을 찾은 레코드 아이디
*/
std::vector<CLUSTER_INFO> ClusterReassembler::findClusterCandidates(RecordInfoMap& inRecordClusterMap, uint64_t inRecordId, uint64_t& outClusterRecordId)
{
	std::vector<CLUSTER_INFO> clusterCandidates;

	if (inRecordClusterMap.find(inRecordId) != inRecordClusterMap.end())
	{
		clusterCandidates = inRecordClusterMap[inRecordId];
		outClusterRecordId = inRecordId;
	}
	else
	{
		if (inRecordClusterMap.find(inRecordId + 1) != inRecordClusterMap.end())
		{
			clusterCandidates = inRecordClusterMap[inRecordId + 1];
			outClusterRecordId = inRecordId + 1;
		}
	}

	return clusterCandidates;
}