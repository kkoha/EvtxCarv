/**
	@file BinXmlTemplateCollection.cpp
	@brief ���̳ʸ� XML�� ���ø��� ��Ƽ� �����ϴ� Ŭ������ ����
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
	@brief ���ø� �ݷ��ǿ� ���ø��� ����ִ´�

	@param inRecordId ���ø��� ���ǵǾ��ִ� ���ڵ� ���̵�
	@param inTemplateId ���ø��� ���̵�
	@param inTemplateSubstitutionMap ���ø� Substitution���� ����
	@param inTemplateDefinition ���ø��� ����
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
	@brief ���ø� �ݷ��ǿ��� Ư�� ���ø��� ã�´�
	
	@param inRecordId ���ø��� ���� �� ���ڵ� ���̵�
	@param inTemplateId ���ø� ���̵�
	@param inTemplateInstances ���ø� �ν��Ͻ����� ����
	@param outTemplate ã�� ���ø�

	@return ���ø��� ã�Ҵ��� ����
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
	@brief ���ø� Substitution�� �ν��Ͻ� ���� ������ �����ϴ��� Ȯ��
	
	@param inTemplateSubstitutionMap ���ø� Substitution ������
	@param inTemplateInstances ���ø� �ν��Ͻ� ������

	@return ���� ����
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