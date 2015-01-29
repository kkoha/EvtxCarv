/**
	@file BinXmlTemplate.h
	@brief 바이터리 XML의 템플릿을 구조화한 클래스의 정의
*/

#ifndef _BINARY_XML_TEMPLATE_H_
#define _BINARY_XML_TEMPLATE_H_

#include <string>
#include <vector>
#include "DataType.h"

/**
	@class BinXmlTemplate
	@brief 바이너리 XML의 템플릿을 구조화한 클래스
	
	템플릿이 충돌하는 경우를 대비하기 위하여 템플릿이 정의된 레코드 번호를 가지고 있다
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
		uint64_t recordId_;  ///< 템플릿이 정의된 레코드 ID
		std::wstring templateDefinition_;  ///< 템플릿의 정의 문자열
};

#endif /* _BINARY_XML_TEMPLATE_H_ */