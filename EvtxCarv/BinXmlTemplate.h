/**
	@file BinXmlTemplate.h
	@brief �����͸� XML�� ���ø��� ����ȭ�� Ŭ������ ����
*/

#ifndef _BINARY_XML_TEMPLATE_H_
#define _BINARY_XML_TEMPLATE_H_

#include <string>
#include <vector>
#include "DataType.h"

/**
	@class BinXmlTemplate
	@brief ���̳ʸ� XML�� ���ø��� ����ȭ�� Ŭ����
	
	���ø��� �浹�ϴ� ��츦 ����ϱ� ���Ͽ� ���ø��� ���ǵ� ���ڵ� ��ȣ�� ������ �ִ�
*/
class BinXmlTemplate
{
	public:
		BinXmlTemplate();
		BinXmlTemplate(uint64_t inRecordId, const std::wstring& inTemplateDefinition);

		void setRecordId(uint64_t inRecordId);
		void setTemplateDefinition(const std::wstring& inTemplateDefinition);

		std::wstring createTemplateInstance(const std::vector<std::wstring>& inTemplateArgs);

		uint64_t getRecordId();
		const std::wstring getTemplateDefinition();

	private:
		uint64_t recordId_;  ///< ���ø��� ���ǵ� ���ڵ� ID
		std::wstring templateDefinition_;  ///< ���ø��� ���� ���ڿ�
};

#endif /* _BINARY_XML_TEMPLATE_H_ */