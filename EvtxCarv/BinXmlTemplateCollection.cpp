/**
	@file BinXmlTemplateCollection.cpp
	@brief 바이너리 XML을 템플릿을 모아서 관리하는 클래스의 구현
*/

#include "BinXmlTemplateCollection.h"
#include "BinaryXmlDef.h"
#include <tchar.h>
#include <stdio.h>

BinXmlTemplateCollection::BinXmlTemplateCollection()
{

}

BinXmlTemplateCollection* BinXmlTemplateCollection::getInstance()
{
	static BinXmlTemplateCollection instance;

	return &instance;
}

/**
	@brief 템플릿 콜렉션에 템플릿을 집어넣는다

	@param inRecordId 템플릿이 정의되어있던 레코드 아이디
	@param inTemplateId 템플릿의 아이디
	@param inTemplateSubstitutionMap 템플릿 Substitution들의 정보
	@param inTemplateDefinition 템플릿의 정의
*/
void BinXmlTemplateCollection::insertTemplate(uint64_t inRecordId, uint32_t inTemplateId, TemplateSubstitutionMap& inTemplateSubstitutionMap, const std::wstring& inTemplateDefinition)
{
	std::map<uint32_t, TemplateList >::iterator foundTemplate = templateContainer_.find(inTemplateId);

	if (foundTemplate == templateContainer_.end())
	{
		TemplateList newTemplateList;
		std::map<uint64_t, BinXmlTemplate> newTemplateMap;
		newTemplateMap.insert(std::make_pair(0, BinXmlTemplate(0, inTemplateDefinition)));

		newTemplateList.push_back(std::make_pair(inTemplateSubstitutionMap, newTemplateMap));

		templateContainer_[inTemplateId] = newTemplateList;
	}
	else
	{
		uint32_t i = 0;

		for (i = 0; i < foundTemplate->second.size(); ++i)
		{
			if (foundTemplate->second[i].first == inTemplateSubstitutionMap)
			{
				break;
			}
		}

		if (i == foundTemplate->second.size())
		{
			std::map<uint64_t, BinXmlTemplate> newTemplateMap;
			newTemplateMap.insert(std::make_pair(0, BinXmlTemplate(0, inTemplateDefinition)));

			foundTemplate->second.push_back(std::make_pair(inTemplateSubstitutionMap, newTemplateMap));
		}
		else
		{
			BinXmlTemplate recordTemplate;

			std::map<uint64_t, BinXmlTemplate>::iterator templateIterator = foundTemplate->second[i].second.begin();
			std::map<uint64_t, BinXmlTemplate>::iterator nextTemplateIterator = templateIterator;
			std::map<uint64_t, BinXmlTemplate>::iterator templateIteratorEnd = foundTemplate->second[i].second.end();

			for (; templateIterator != templateIteratorEnd; ++templateIterator)
			{
				nextTemplateIterator = templateIterator;
				++nextTemplateIterator;

				if (nextTemplateIterator == templateIteratorEnd) { break; }

				if (nextTemplateIterator->first > inRecordId) { break; }
			}

			recordTemplate = templateIterator->second;

			if (recordTemplate.getTemplateDefinition() != inTemplateDefinition)
			{
				foundTemplate->second[i].second.insert(std::make_pair(inRecordId, BinXmlTemplate(inRecordId, inTemplateDefinition)));
			}
		}
	}
}

/**
	@brief 템플릿 콜렉션에서 특정 템플릿을 찾는다
	
	@param inRecordId 템플릿이 적용 될 레코드 아이디
	@param inTemplateId 템플릿 아이디
	@param inTemplateInstances 템플릿 인스턴스들의 정보
	@param outTemplate 찾은 템플릿

	@return 템플릿을 찾았는지 여부
*/
bool BinXmlTemplateCollection::getTemplate(uint64_t inRecordId, uint32_t inTemplateId, TemplateInstanceInfos& inTemplateInstances, BinXmlTemplate& outTemplate)
{
	if (templateContainer_.find(inTemplateId) == templateContainer_.end()) { return false; }
	
	uint32_t i = 0;

	std::map<uint32_t, TemplateList>::iterator foundTemplate = templateContainer_.find(inTemplateId);

	for (i = 0; i < foundTemplate->second.size(); ++i)
	{
		if (isValidTemplateInstance(foundTemplate->second[i].first, inTemplateInstances))
		{
			break;
		}
	}

	if (i == foundTemplate->second.size()) { return false; }

	std::map<uint64_t, BinXmlTemplate>::iterator templateIterator = foundTemplate->second[i].second.begin();
	std::map<uint64_t, BinXmlTemplate>::iterator nextTemplateIterator = templateIterator;
	std::map<uint64_t, BinXmlTemplate>::iterator templateIteratorEnd = foundTemplate->second[i].second.end();

	for (; templateIterator != templateIteratorEnd; ++templateIterator)
	{
		nextTemplateIterator = templateIterator;
		++nextTemplateIterator;

		if (nextTemplateIterator == templateIteratorEnd) { break; }

		if (nextTemplateIterator->first > inRecordId) { break; }
	}

	outTemplate = templateIterator->second;

	return true;
}

/**
	@brief 템플릿 Substitution과 인스턴스 값의 정보가 부합하는지 확인
	
	@param inTemplateSubstitutionMap 템플릿 Substitution 정보들
	@param inTemplateInstances 템플릿 인스턴스 정보들

	@return 부합 여부
*/
bool BinXmlTemplateCollection::isValidTemplateInstance(TemplateSubstitutionMap& inTemplateSubstitutionMap, TemplateInstanceInfos& inTemplateInstances)
{
	TemplateSubstitutionMap::iterator subStitution;

	for (subStitution = inTemplateSubstitutionMap.begin(); subStitution != inTemplateSubstitutionMap.end(); ++subStitution)
	{
		if (subStitution->first >= inTemplateInstances.size()) return false;

		if (subStitution->second.valueType != inTemplateInstances[subStitution->first].valueType)
		{
			if (!(subStitution->second.isOptional && inTemplateInstances[subStitution->first].valueType == BIN_XML_VALUE_NULL))
			{
				return false;
			}
		}
	}

	return true;
}