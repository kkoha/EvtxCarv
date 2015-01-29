/**
	@file SignatureFinder.h
	@brief ���Ͽ��� Ư�� ������ ã�� Ŭ������ ����
*/

#ifndef _SIGNATURE_FINDER_H_
#define _SIGNATURE_FINDER_H_

#include "BinaryFile.h"
#include "FileMap.h"

/**
	@class SignatureFinder
	@brief Ư�� ���Ͽ��� Ư�� ����Ʈ�迭�� ã�� Ŭ����, ��ŵ�� ���� �� ������ ��ŵ�� ������ ������ �� �ִ�
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