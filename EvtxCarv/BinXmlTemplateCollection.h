/**
	@file BinXmlTemplateCollection.h
	@brief ���̳ʸ� XML�� ���ø��� ��Ƽ� �����ϴ� Ŭ������ ����
*/

#ifndef _BINARY_XML_TEMPLATE_COLLECTION_H_
#define _BINARY_XML_TEMPLATE_COLLECTION_H_

#include <string>
#include <map>
#include <list>
#include "BinXmlTemplate.h"
#include "DataType.h"

/**
	@class BinXmlTemplateCollection
	@brief ���̳ʸ� XML �� ���ø����� ��Ƽ� �����ϴ� Ŭ����
*/
class BinXmlTemplateCollection
{
	public:
		static BinXmlTemplateCollection* getInstance();

		void insertTemplate(uint64_t inRecordId, uint32_t inTemplateId, TemplateSubstitutionMap& inTemplateSubstitutionMap, const std::wstring& inTemplateDefinition);
		bool getTemplate(uint64_t inRecordId, uint32_t inTemplateId, TemplateInstanceInfos& inTemplateInstances, BinXmlTemplate& outTemplate);

	private:
		BinXmlTemplateCollection();
		bool isValidTemplateInstance(TemplateSubstitutionMap& inTemplateSubstitutionMap, TemplateInstanceInfos& inTemplateInstances);

		std::map<uint32_t, TemplateList> templateContainer_; ///< ���ø� ���̵�/Substitution����/���ڵ� ��ȣ�� ������� ����Ǿ��ִ� ���ø���
};

#endif /* _BINARY_XML_TEMPLATE_COLLECTION_H_ */