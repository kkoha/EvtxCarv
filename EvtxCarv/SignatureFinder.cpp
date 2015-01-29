/**
	@file SignatureFinder.cpp
	@brief 파일에서 특정 패턴을 찾는 클래스의 구현
*/

#include "SignatureFinder.h"

/**
	@brief 파일 내 패턴 찾기 클래스 초기화

	@param inBinaryFile 내용을 찾을 파일
	@param inSignature 찾을 내용
	@param inSignatureSize 찾을 내용의 바이트 수
	@param inStep 내용을 찾을 때 한번에 건너뛰는 바이트 수
	@param inSkipAreas 검색에서 제외할 영역
	@param inDataBufferSize 패턴을 찾았을 때 이후의 내용을 사용할 경우 필요한 내용의 바이트 수
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
	@brief 검색 위치를 처음으로 돌린다
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
	@brief 현재 위치에서 다음 패턴을 찾는다
	@param optOutFilePos 패턴을 찾은 파일 위치(null일 경우 무시)
	@return 패턴을 찾은 곳의 버퍼의 메모리 주소(패턴을 찾지 못할 경우 null)
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
	@brief 다음번에 스킵할 영역을 추가한다
*/
void SignatureFinder::addCarvedArea(const std::pair<uint64_t, uint64_t>& carvedArea)
{
	carvedAreas_.push_back(carvedArea);
}

/**
	@brief 이번에 스킵한 영역 + 다음번에 스킵할 영역을 얻어온다
	@return 스킵할 영역
*/
CarvedAreas* SignatureFinder::getCarvedArea()
{
	return &carvedAreas_;
}

/**
	@brief 검색을 위하여 열었던 파일 메모리를 닫는다
*/
void SignatureFinder::closeMap()
{
	fileMap_.closeMap();
}