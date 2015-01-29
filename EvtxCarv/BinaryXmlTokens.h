/**
	@file BinaryXmlTokens.h
	@brief evtx 바이너리 XML의 토큰들의 구조를 정의한 파일
*/

#ifndef _BINARY_XML_TOKENS_H_
#define _BINARY_XML_TOKENS_H_

#include "DataType.h"

#pragma pack(1)

/**
	@class BXML_FRAGMENT_HEADER
	@brief 바이너리 XML Fragment Header
*/
struct BXML_FRAGMENT_HEADER {
	uint8_t token;  ///< 토큰
	uint8_t majorVersion;  ///< 메이저 버전
	uint8_t minorVersion;  ///< 마이너 버전
	uint8_t flags;  ///< 플래그
};

/**
	@class BXML_ELEMENT_START
	@brief 바이너리 XML 엘리먼트의 시작을 나타내는 토큰
*/
struct BXML_ELEMENT_START {
	uint8_t token;  ///< 토큰
	uint16_t dependencyIdentifier;
	uint32_t dataSize; ///< 데이터의 크기
	uint32_t elementNameOffset;  ///< 엘리먼트의 이름이 담겨있는 오프셋
};

/**
	@class BXML_ATTRIBUTE_LIST
	@brief 바이너리 XML 엘리먼트의 속성들의 리스트
*/
struct BXML_ATTRIBUTE_LIST {
	uint32_t dataSize;  ///< 리스트의 크기
};

/**
	@class BXML_ATTRIBUTE
	@brief 바이너리 XML 엘리먼트의 속성 하나
*/
struct BXML_ATTRIBUTE {
	uint8_t token;  ///< 토큰
	uint32_t nameOffset;  ///< 이름의 오프셋
};

/**
	@class BXML_NAME
	@brief 바이너리 XML 엘리먼트/속성의 이름을 표현하는 클래스
*/
struct BXML_NAME {
	uint32_t unknown;
	uint16_t nameHash;  ///< 이름의 해시
	uint16_t numChars;  ///< 글자 수
};

/**
	@class BXML_VALUE_TEXT
	@brief 바이너리 XML 문자열
*/
struct BXML_VALUE_TEXT {
	uint8_t token; ///< 토큰
	uint8_t valueType;  ///< 타입
};

/**
	@class BXML_NORMAL_SUBSTITUTION
	@brief 바이너리 XML 에서 나중에 다른값으로 치환될 부분
*/
struct BXML_NORMAL_SUBSTITUTION {
	uint8_t token;  ///< 토큰
	uint16_t identifier;  ///< 아이디
	uint8_t valueType;  ///< 타입
};

/**
	@class BXML_OPTIONAL_SUBSTITUTION
	@brief 바이너리 XML 에서 나중에 다른값으로 치환될 부분(null값 가능 및 여러가지 옵션이 있음), 현재 구현에서는 Normal과 같이 사용함
*/
struct BXML_OPTIONAL_SUBSTITUTION {
	uint8_t token;  ///< 토큰
	uint16_t identifier;  ///< 아이디
	uint8_t valueType;  ///< 타입
};

/**
	@class BXML_CHARACTER_ENTITY_REF
	@brief 바이너리 XML 에서 문자 하나에 대한 참조
*/
struct BXML_CHARACTER_ENTITY_REF {
	uint8_t token;
	uint16_t entityValue;
};

/**
	@class BXML_ENTITY_REF
	@brief 알 수 없음
*/
struct BXML_ENTITY_REF {
	uint8_t token;
	uint32_t nameOffset;
};

/**
	@class BXML_CDATA_SECTION
	@brief 바이너리 XML CDATA 데이터
*/
struct BXML_CDATA_SECTION {
	uint8_t token;
};

/**
	@class BXML_TEMPLATE_INSTANCE
	@brief 바이너리 XML 템플릿의 정의(일부분만 있을 수 있음) 와 인스턴스
*/
struct BXML_TEMPLATE_INSTANCE {
	uint8_t token;  ///< 토큰
};

/**
	@class BXML_TEMPLATE_DEFINITION
	@brief 바이너리 XML 템플릿의 정의
*/
struct BXML_TEMPLATE_DEFINITION {
	uint8_t unknown1;
	uint32_t templateIdentifier;  ///< 템플릿 아이디
	uint32_t dataOffset;  ///< 템플릿의 정의가 있는 오프셋(현재 레코드에 없는 경우 데이터가 여기서 끝남)
	uint32_t unknown3;
	uint32_t guid1;  ///< guid의 일부1(=템플릿 아이디)
	uint32_t guid2;  ///< guid의 일부2
	uint64_t guid3;  ///< guid의 일부3
	uint32_t dataSize;  ///< 템플릿 정의의 사이즈
	uint32_t fragment;  ///< ( = BXML_FRAGMENT_HEADER)
};

/**
	@class BXML_TEMPLATE_INSTANCE_DATA
	@brief 바이너리 XML 템플릿의 인스턴스
*/
struct BXML_TEMPLATE_INSTANCE_DATA {
	uint32_t cntTemplateValues;  ///< 인스턴스 값들의 개수
};

/**
	@class BXML_TEMPLATE_VALUE_DESCRIPTOR
	@brief 바이너리 XML 템플릿 인스턴스 값을 정보를 표현
*/
struct BXML_TEMPLATE_VALUE_DESCRIPTOR {
	uint16_t valueSize;  ///< 값의 크기
	uint8_t valueType;  ///< 값의 타입
	uint8_t unknown;
};

/**
	@class BXML_UNICODE_TEXT_STRING
	@brief 바이너리 XML에서 유니코드 문자열
*/
struct BXML_UNICODE_TEXT_STRING {
	uint16_t uknown;
	uint16_t numChars; ///< 글자 수
};

/**
	@class BXML_PI_TARGET
	@brief 알 수 없음
*/
struct BXML_PI_TARGET {
	uint8_t token;  ///< 토큰
	uint32_t targetNameOffset;
};

/**
	@class BXML_PI_DATA
	@brief 알 수 없음
*/
struct BXML_PI_DATA {
	uint8_t token;
};

#pragma pack()

#endif /* _BINARY_XML_TOKENS_H_ */