/**
	@file SignatureFinder.h
	@brief 파일에서 특정 패턴을 찾는 클래스의 정의
*/

#ifndef _SIGNATURE_FINDER_H_
#define _SIGNATURE_FINDER_H_

#include "BinaryFile.h"
#include "FileMap.h"

/**
	@class SignatureFinder
	@brief 특정 파일에서 특정 바이트배열을 찾는 클래스, 스킵할 영역 및 다음에 스킵될 영역을 지정할 수 있다
*/
class SignatureFinder
{
	public:
		SignatureFinder(BinaryFile& inBinaryFile, uint64_t inSignature, int inSignatureSize, int inStep, const CarvedAreas* inSkipAreas, int inDataBufferSize = 0);

		uint8_t* getNext(uint64_t* optOutFilePos);

		void addCarvedArea(const std::pair<uint64_t, uint64_t>& carvedArea);
		CarvedAreas* getCarvedArea();

		void closeMap();
		void resetFinder();

	private:
		uint64_t currentFilePos_;
		uint64_t currentBufPos_;
		uint64_t signature_;
		int signatureSize_;
		int step_;
		int dataBufferSize_;
		BinaryFile binaryFile_;
		FileMap fileMap_;
		const CarvedAreas* skipAreas_;
		CarvedAreas carvedAreas_;
		CarvedAreas::const_iterator currentSkipPos_;
};

#endif /* _SIGNATURE_FINDER_H_ */