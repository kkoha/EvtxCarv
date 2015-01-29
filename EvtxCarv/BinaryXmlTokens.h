/**
	@file BinaryXmlTokens.h
	@brief evtx ���̳ʸ� XML�� ��ū���� ������ ������ ����
*/

#ifndef _BINARY_XML_TOKENS_H_
#define _BINARY_XML_TOKENS_H_

#include "DataType.h"

#pragma pack(1)

/**
	@class BXML_FRAGMENT_HEADER
	@brief ���̳ʸ� XML Fragment Header
*/
struct BXML_FRAGMENT_HEADER {
	uint8_t token;  ///< ��ū
	uint8_t majorVersion;  ///< ������ ����
	uint8_t minorVersion;  ///< ���̳� ����
	uint8_t flags;  ///< �÷���
};

/**
	@class BXML_ELEMENT_START
	@brief ���̳ʸ� XML ������Ʈ�� ������ ��Ÿ���� ��ū
*/
struct BXML_ELEMENT_START {
	uint8_t token;  ///< ��ū
	uint16_t dependencyIdentifier;
	uint32_t dataSize; ///< �������� ũ��
	uint32_t elementNameOffset;  ///< ������Ʈ�� �̸��� ����ִ� ������
};

/**
	@class BXML_ATTRIBUTE_LIST
	@brief ���̳ʸ� XML ������Ʈ�� �Ӽ����� ����Ʈ
*/
struct BXML_ATTRIBUTE_LIST {
	uint32_t dataSize;  ///< ����Ʈ�� ũ��
};

/**
	@class BXML_ATTRIBUTE
	@brief ���̳ʸ� XML ������Ʈ�� �Ӽ� �ϳ�
*/
struct BXML_ATTRIBUTE {
	uint8_t token;  ///< ��ū
	uint32_t nameOffset;  ///< �̸��� ������
};

/**
	@class BXML_NAME
	@brief ���̳ʸ� XML ������Ʈ/�Ӽ��� �̸��� ǥ���ϴ� Ŭ����
*/
struct BXML_NAME {
	uint32_t unknown;
	uint16_t nameHash;  ///< �̸��� �ؽ�
	uint16_t numChars;  ///< ���� ��
};

/**
	@class BXML_VALUE_TEXT
	@brief ���̳ʸ� XML ���ڿ�
*/
struct BXML_VALUE_TEXT {
	uint8_t token; ///< ��ū
	uint8_t valueType;  ///< Ÿ��
};

/**
	@class BXML_NORMAL_SUBSTITUTION
	@brief ���̳ʸ� XML ���� ���߿� �ٸ������� ġȯ�� �κ�
*/
struct BXML_NORMAL_SUBSTITUTION {
	uint8_t token;  ///< ��ū
	uint16_t identifier;  ///< ���̵�
	uint8_t valueType;  ///< Ÿ��
};

/**
	@class BXML_OPTIONAL_SUBSTITUTION
	@brief ���̳ʸ� XML ���� ���߿� �ٸ������� ġȯ�� �κ�(null�� ���� �� �������� �ɼ��� ����), ���� ���������� Normal�� ���� �����
*/
struct BXML_OPTIONAL_SUBSTITUTION {
	uint8_t token;  ///< ��ū
	uint16_t identifier;  ///< ���̵�
	uint8_t valueType;  ///< Ÿ��
};

/**
	@class BXML_CHARACTER_ENTITY_REF
	@brief ���̳ʸ� XML ���� ���� �ϳ��� ���� ����
*/
struct BXML_CHARACTER_ENTITY_REF {
	uint8_t token;
	uint16_t entityValue;
};

/**
	@class BXML_ENTITY_REF
	@brief �� �� ����
*/
struct BXML_ENTITY_REF {
	uint8_t token;
	uint32_t nameOffset;
};

/**
	@class BXML_CDATA_SECTION
	@brief ���̳ʸ� XML CDATA ������
*/
struct BXML_CDATA_SECTION {
	uint8_t token;
};

/**
	@class BXML_TEMPLATE_INSTANCE
	@brief ���̳ʸ� XML ���ø��� ����(�Ϻκи� ���� �� ����) �� �ν��Ͻ�
*/
struct BXML_TEMPLATE_INSTANCE {
	uint8_t token;  ///< ��ū
};

/**
	@class BXML_TEMPLATE_DEFINITION
	@brief ���̳ʸ� XML ���ø��� ����
*/
struct BXML_TEMPLATE_DEFINITION {
	uint8_t unknown1;
	uint32_t templateIdentifier;  ///< ���ø� ���̵�
	uint32_t dataOffset;  ///< ���ø��� ���ǰ� �ִ� ������(���� ���ڵ忡 ���� ��� �����Ͱ� ���⼭ ����)
	uint32_t unknown3;
	uint32_t guid1;  ///< guid�� �Ϻ�1(=���ø� ���̵�)
	uint32_t guid2;  ///< guid�� �Ϻ�2
	uint64_t guid3;  ///< guid�� �Ϻ�3
	uint32_t dataSize;  ///< ���ø� ������ ������
	uint32_t fragment;  ///< ( = BXML_FRAGMENT_HEADER)
};

/**
	@class BXML_TEMPLATE_INSTANCE_DATA
	@brief ���̳ʸ� XML ���ø��� �ν��Ͻ�
*/
struct BXML_TEMPLATE_INSTANCE_DATA {
	uint32_t cntTemplateValues;  ///< �ν��Ͻ� ������ ����
};

/**
	@class BXML_TEMPLATE_VALUE_DESCRIPTOR
	@brief ���̳ʸ� XML ���ø� �ν��Ͻ� ���� ������ ǥ��
*/
struct BXML_TEMPLATE_VALUE_DESCRIPTOR {
	uint16_t valueSize;  ///< ���� ũ��
	uint8_t valueType;  ///< ���� Ÿ��
	uint8_t unknown;
};

/**
	@class BXML_UNICODE_TEXT_STRING
	@brief ���̳ʸ� XML���� �����ڵ� ���ڿ�
*/
struct BXML_UNICODE_TEXT_STRING {
	uint16_t uknown;
	uint16_t numChars; ///< ���� ��
};

/**
	@class BXML_PI_TARGET
	@brief �� �� ����
*/
struct BXML_PI_TARGET {
	uint8_t token;  ///< ��ū
	uint32_t targetNameOffset;
};

/**
	@class BXML_PI_DATA
	@brief �� �� ����
*/
struct BXML_PI_DATA {
	uint8_t token;
};

#pragma pack()

#endif /* _BINARY_XML_TOKENS_H_ */