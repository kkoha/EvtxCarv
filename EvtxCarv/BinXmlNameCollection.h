/**
	@file BinXmlNameCollection.h
	@brief ���̳ʸ� XML �� �̸����� ��Ƽ� �����ϴ� Ŭ������ ����
*/

#ifndef _BIN_XML_NAME_COLLECTION_H_
#define _BIN_XML_NAME_COLLECTION_H_

#include <string>
#include <map>
#include <list>
#include "BinXmlName.h"

/**
	@class BinXmlNameCollection
	@brief ���̳ʸ� XML �� �̸����� ��Ƽ� �����ϴ� Ŭ����
*/
class BinXmlNameCollection
{
	public:
		static BinXmlNameCollection* getInstance();

		void insertName(uint64_t inRecordId, uint32_t inNameOffset, const std::wstring& inNameString);
		bool getName(uint64_t inRecordId, uint32_t inNameOffset, std::wstring& outNameString);

	private:
		BinXmlNameCollection();

		std::map<uint32_t, std::map<uint64_t, BinXmlName> > nameContainer_;  ///< �̸��� ������, ���ڵ� ��ȣ ������� ���ִ� �̸���
};

#endif /* _BIN_XML_NAME_COLLECTION_H_ */