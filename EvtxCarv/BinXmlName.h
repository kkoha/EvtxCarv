/**
	@file BinXmlName.h
	@brief ���̳ʸ� XML������ �̸��� ǥ���ϴ� Ŭ������ ����
*/

#ifndef _BIN_XML_NAME_H_
#define _BIN_XML_NAME_H_

#include <string>
#include "DataType.h"

/**
	@class BinXmlName
	@brief ���̳ʸ� XML�� �̸�(������Ʈ/�Ӽ�)�� ����ȭ�� Ŭ����
	
	�̸��� �浹�ϴ� ��츦 ����ϱ� ���Ͽ� �̸��� ���ǵ� ���ڵ� ��ȣ�� ������ �ִ�
*/
class BinXmlName
{
	public:
		BinXmlName();
		BinXmlName(uint64_t inRecordId, const std::wstring& inNameString);

		void setRecordId(uint64_t inRecordId);
		void setNameString(const std::wstring& inNameString);

		uint64_t getRecordId();
		const std::wstring getNameString();

	private:
		uint64_t recordId_;  ///< �̸��� ���ǵ� ���ڵ� ��ȣ
		std::wstring nameString_;  ///< �̸��� ���ڿ�
};

#endif /* _BIN_XML_NAME_H_ */