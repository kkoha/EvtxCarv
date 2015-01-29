/**
	@file BinXmlTemplateCollection.h
	@brief 바이너리 XML을 템플릿을 모아서 관리하는 클래스의 정의
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
	@brief 바이너리 XML 의 템플릿들을 모아서 관리하는 클래스
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

		std::map<uint32_t, TemplateList> templateContainer_; ///< 템플릿 아이디/Substitution정보/레코드 번호를 기반으로 저장되어있는 템플릿들
};

#endif /* _BINARY_XML_TEMPLATE_COLLECTION_H_ */