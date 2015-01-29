/**
	@file ClusterReassembler.cpp
	@brief ����ȭ�� Ŭ������ �������� ���� Ŭ������ ����
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
	@brief �������� ������ ���Ѵ�
	@param inFragmentedFile ������ ����
*/
void ClusterReassembler::setFragmentedFile(BinaryFile& inFragmentedFile)
{
	fragmentedFile_ = inFragmentedFile;
}

/**
	@brief �������� ������ ������ ��µ� ������ ���Ѵ�
	@param inOutFile ����� ����
*/
void ClusterReassembler::setOutputFile(BinaryFile& inOutFile)
{
	outFile_ = inOutFile;
}

/**
	@brief ûũ/���ڵ带 �⺻���� Ŭ�����͵��� �ùٸ� ������� �ִ��� �������Ѵ�

	@param inCarvedAreas �������� ������ ���� �� ��ŵ�� ����
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

	// ����� ��ȿ������ ������ ī������ ���� ûũ(�Ƹ��� ������)�鿡 ���Ͽ�
	for (uint32_t clusterChunkPairIndex = 0; clusterChunkPairIndex < clusterChunkPairs.size(); ++clusterChunkPairIndex)
	{
		std::vector<uint64_t> chunkClusters;
		std::vector<uint64_t> nextClusters;
		std::vector<CLUSTER_INFO> clusterCandidates;
		bool alreadyAddedCluster = false;

		chunkClusters.clear();

		// ûũ ����� �� ���ڵ庰��
		for (uint64_t recordId = clusterChunkPairs[clusterChunkPairIndex].second.numLogRecFirst;
			recordId <= clusterChunkPairs[clusterChunkPairIndex].second.numLogRecLast; ++recordId)
		{
			uint64_t clusterRecordId = 0;

			// �ش� ���ڵ� ���̵� ������ Ŭ������(�ڿ� �߰��� Ŭ������ �ĺ���)���� ã�´�
			clusterCandidates = findClusterCandidates(recordInfoMap, recordId, clusterRecordId);

			if (!clusterCandidates.empty())
			{
				if (chunkClusters.empty())
				{
					chunkClusters.push_back(clusterChunkPairs[clusterChunkPairIndex].first);
				}

				// �̹��� ������� Ŭ������ �ĺ��� �߿� ���� ������ Ŭ�����͸� ã�´�
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

		// ������ Ŭ�����͵��� ���Ͽ� ����Ѵ�
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

			// ���� �˰��򿡼� ó������ ���� Ŭ�����͵���
			// (ûũ ����� ���ư��� ���ɼ� Ȥ�� �߰��� ���ư� ���� ���ڵ�� �������� ����������)
			if (processedClusters.find(recordInfoIterator->second[clusterIndex].clusterNo) == processedClusters.end())
			{
				chunkClusters.push_back(recordInfoIterator->second[clusterIndex].clusterNo);
				
				for (candidateRecordInfoIterator = recordInfoIterator; candidateRecordInfoIterator != recordInfoMap.end(); ++candidateRecordInfoIterator)
				{
					// �ٽ� ������ Ŭ�����͵��� ã�ƺ���
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
					// ���Ͽ� ����Ѵ�(�� �� �ߺ��� ���ϱ� ���� �̹� ó���� Ŭ�����ʹ� ���� �ʴ´�)
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
	@brief ���Ϸκ��� ûũ�� �������� ������

	@param inChunkCarvedAreas ��ŵ�� ����
	@param outClusterChunkPairs Ŭ������ ��ȣ/ûũ���� �� ������ �� ����Ʈ
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
	@brief ���Ϸκ��� ���ڵ��� �������� ������

	@param inRecordCarvedAreas ��ŵ�� ����
	@param outRecordInfoMap Ŭ������ ��ȣ / ���ڵ����� ������ �� ��
	@param outClustersHaveRecord ���ڵ带 ������ �ִ� Ŭ�����͵�
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

			// ���ڵ�� Ŭ�����͸� ������ �ߺ��� ���ڵ� ���̵� ������
			if (outRecordInfoMap.find(evtxRecordHeader->numLogRecord) != outRecordInfoMap.end())
			{
				bool crcFound = false;

				// Ŭ�������� crc�� ���Ͽ� ���� crc�̸� �ش� Ŭ�����͸� ���ڵ� ���̵��� Ŭ������ �ĺ����� �߰�
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
	@brief Ŭ�������� �ĺ��� �� ���� Ŭ�����Ϳ� ���� �����ϰ� ����� Ŭ�����͸� ã�´�

	@param inClusterNo ���� Ŭ������ ��ȣ
	@param inClusterCandidates �ڿ� ����� Ŭ�������� �ĺ�
	@param inClustersHaveRecord ���ڵ带 ������ �ִ� Ŭ������
	@param outAlreadyAddedCluster Ŭ������ �ĺ����߿� �̹� �߰��Ǳ�� ������ Ŭ�����Ͱ� �ִ��� ����
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

		// ���� Ŭ�����Ϳ���(1Ŭ������) ���� ������ ���ڵ带 ã�´�
		for (recordPos = 0; recordPos < CLUSTER_SIZE - EVTX_RECORD_HEADER_SIZE; recordPos += 8)
		{
			if (EvtxRecordValidator::isValidRecordHeader(clusterBuffer + recordPos))
			{
				lastRecordPos = recordPos;
				break;
			}
		}

		lastRecord = (EVTX_RECORD_HEADER*)(clusterBuffer + lastRecordPos);

		// ���� ������ ���ڵ尡 3�� �̻��� Ŭ�����Ϳ� ������ ������
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

			// ��¿�� ���� ���ڵ��� Ŭ�����͵��� ���ӵǾ� �ִٰ� �����ϰ� ��ó���� �������� �� �߰� Ŭ������ �����͵��� ���Ѵ�
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

		// �̾���� Ŭ������ �ĺ����� ���Ͽ�
		for (clusterCandidateIndex = 0; clusterCandidateIndex < inClusterCandidates.size(); ++clusterCandidateIndex)
		{
			uint32_t mergedLastRecordPos = 0;
			EVTX_RECORD_HEADER* mergedLastRecord = NULL;

			// ������ Ŭ�����͵��� �����Ͽ� ����
			fragmentedFile_.readData(inClusterCandidates[clusterCandidateIndex].clusterNo * CLUSTER_SIZE, (mergedClusterBuffer + nextClusterBufferPos), CLUSTER_SIZE);

			for (recordPos = 0; recordPos < CLUSTER_SIZE; recordPos += 8)
			{
				if (EvtxRecordValidator::isValidRecordHeader(mergedClusterBuffer + recordPos))
				{
					mergedLastRecordPos = recordPos;
					break;
				}
			}

			// �ùٸ� ���ڵ尡 �Ǵ��� Ȯ���Ͽ� �ùٸ� ���ڵ尡 �Ǹ� ���� ������ Ŭ�����Ͷ� ���Ѵ�
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
	@brief Ư�� ���ڵ� ���̵� �� �����Ǵ� Ŭ�������� �ĺ����� ��´�

	@param inRecordClusterMap ���ڵ� ���̵� / Ŭ������ �ĺ��� ���� ��
	@param inRecordId ���ڵ� ���̵�
	@param outClusterRecordId ���� Ŭ������ �ĺ����� ã�� ���ڵ� ���̵�
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