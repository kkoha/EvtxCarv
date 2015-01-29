/**
	@file SignatureFinder.cpp
	@brief ���Ͽ��� Ư�� ������ ã�� Ŭ������ ����
*/

#include "SignatureFinder.h"

/**
	@brief ���� �� ���� ã�� Ŭ���� �ʱ�ȭ

	@param inBinaryFile ������ ã�� ����
	@param inSignature ã�� ����
	@param inSignatureSize ã�� ������ ����Ʈ ��
	@param inStep ������ ã�� �� �ѹ��� �ǳʶٴ� ����Ʈ ��
	@param inSkipAreas �˻����� ������ ����
	@param inDataBufferSize ������ ã���� �� ������ ������ ����� ��� �ʿ��� ������ ����Ʈ ��
*/
SignatureFinder::SignatureFinder(BinaryFile& inBinaryFile, uint64_t inSignature, int inSignatureSize, int inStep, const CarvedAreas* inSkipAreas, int inDataBufferSize)
{
	binaryFile_ = inBinaryFile;
	signature_ = inSignature;
	signatureSize_ = inSignatureSize;
	step_ = inStep;
	skipAreas_ = inSkipAreas;
	dataBufferSize_ = inDataBufferSize;
	currentFilePos_ = 0;
	currentBufPos_ = 0;
	if (inSkipAreas != NULL)
	{
		currentSkipPos_ = inSkipAreas->begin();
	}
	carvedAreas_.clear();

	fileMap_ = binaryFile_.createFileMap(0, MMAP_SIZE + inDataBufferSize);
}

/**
	@brief �˻� ��ġ�� ó������ ������
*/
void SignatureFinder::resetFinder()
{
	currentFilePos_ = 0;
	currentBufPos_ = 0;
	if (skipAreas_ != NULL)
	{
		currentSkipPos_ = skipAreas_->begin();
	}
	carvedAreas_.clear();

	fileMap_.closeMap();
	fileMap_ = binaryFile_.createFileMap(0, MMAP_SIZE + dataBufferSize_);
}

/**
	@brief ���� ��ġ���� ���� ������ ã�´�
	@param optOutFilePos ������ ã�� ���� ��ġ(null�� ��� ����)
	@return ������ ã�� ���� ������ �޸� �ּ�(������ ã�� ���� ��� null)
*/
uint8_t* SignatureFinder::getNext(uint64_t* optOutFilePos)
{
	uint8_t* result = NULL;
	bool found = false;

	while (true)
	{
		found = false;

		if (skipAreas_ != NULL && !skipAreas_->empty() && currentSkipPos_ != skipAreas_->end())
		{
			if (currentFilePos_ >= currentSkipPos_->first && currentFilePos_ <= currentSkipPos_->second)
			{
				currentBufPos_ += (currentSkipPos_->second - currentFilePos_);

				carvedAreas_.push_back(std::make_pair(currentFilePos_, currentSkipPos_->second));

				currentFilePos_ = currentSkipPos_->second;

				++currentSkipPos_;

				continue;
			}
		}

		if (currentFilePos_ + dataBufferSize_ >= binaryFile_.getFileSize())
		{
			return NULL;
		}

		if (currentBufPos_ >= fileMap_.getSize() - dataBufferSize_)
		{
			fileMap_.closeMap();
			fileMap_ = binaryFile_.createFileMap(currentFilePos_, MMAP_SIZE + dataBufferSize_);
			currentBufPos_ = 0;
		}

		if (!fileMap_.isValidMap()) { break; }

		if (signatureSize_ == 4)
		{
			if (*(uint32_t*)(fileMap_.getData() + currentBufPos_) == signature_) { found = true; }
		}
		else if (signatureSize_ == 8)
		{
			if (*(uint64_t*)(fileMap_.getData() + currentBufPos_) == signature_) { found = true; }
		}

		if (found)
		{
			if (optOutFilePos != NULL) { *optOutFilePos = currentFilePos_; }
			result = fileMap_.getData() + currentBufPos_;
		}

		currentBufPos_ += step_ - (currentBufPos_ % step_);
		currentFilePos_ += step_  - (currentBufPos_ % step_);

		if (found) break;
	}

	return result;
}

/**
	@brief �������� ��ŵ�� ������ �߰��Ѵ�
*/
void SignatureFinder::addCarvedArea(const std::pair<uint64_t, uint64_t>& carvedArea)
{
	carvedAreas_.push_back(carvedArea);
}

/**
	@brief �̹��� ��ŵ�� ���� + �������� ��ŵ�� ������ ���´�
	@return ��ŵ�� ����
*/
CarvedAreas* SignatureFinder::getCarvedArea()
{
	return &carvedAreas_;
}

/**
	@brief �˻��� ���Ͽ� ������ ���� �޸𸮸� �ݴ´�
*/
void SignatureFinder::closeMap()
{
	fileMap_.closeMap();
}