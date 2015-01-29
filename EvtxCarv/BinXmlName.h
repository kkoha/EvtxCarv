/**
	@file BinXmlName.h
	@brief 바이너리 XML에서의 이름을 표현하는 클래스의 정의
*/

#ifndef _BIN_XML_NAME_H_
#define _BIN_XML_NAME_H_

#include <string>
#include "DataType.h"

/**
	@class BinXmlName
	@brief 바이너리 XML의 이름(엘리먼트/속성)을 구조화한 클래스
	
	이름이 충돌하는 경우를 대비하기 위하여 이름이 정의된 레코드 번호를 가지고 있다
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
		uint64_t recordId_;  ///< 이름이 정의된 레코드 번호
		std::wstring nameString_;  ///< 이름의 문자열
};

#endif /* _BIN_XML_NAME_H_ */