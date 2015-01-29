/**
	@file BinXmlNameCollection.h
	@brief 바이너리 XML 의 이름들을 모아서 관리하는 클래스의 정의
*/

#ifndef _BIN_XML_NAME_COLLECTION_H_
#define _BIN_XML_NAME_COLLECTION_H_

#include <string>
#include <map>
#include <list>
#include "BinXmlName.h"

/**
	@class BinXmlNameCollection
	@brief 바이너리 XML 의 이름들을 모아서 관리하는 클래스
*/
class BinXmlNameCollection
{
	public:
		static BinXmlNameCollection* getInstance();

		void insertName(uint64_t inRecordId, uint32_t inNameOffset, const std::wstring& inNameString);
		bool getName(uint64_t inRecordId, uint32_t inNameOffset, std::wstring& outNameString);

	private:
		BinXmlNameCollection();

		std::map<uint32_t, std::map<uint64_t, BinXmlName> > nameContainer_;  ///< 이름의 오프셋, 레코드 번호 기반으로 모여있는 이름들
};

#endif /* _BIN_XML_NAME_COLLECTION_H_ */