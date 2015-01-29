/**
	@file RegistryReader.cpp
	@brief ������Ʈ���� ������ �б����� Ŭ������ ����
*/

#include "RegistryReader.h"
#include <tchar.h>

/**
	@brief ������Ʈ������ Ű���� ������ �о�´�(32/64��Ʈ �������)

	@param rootKey ��ƮŰ(HKEY_LOCAL_MACHINE)
	@param inRegPath ������Ʈ�� Ű���� �ִ� ���
	@param inKeyName ������Ʈ�� Ű���� �̸�
	@param outString �о�� �����

	@return ���� ����
*/
bool RegistryReader::readString(HKEY rootKey, const std::wstring& inRegPath, const std::wstring& inKeyName, std::wstring& outString)
{
	outString = _T("");

	if (!readString32(rootKey, inRegPath, inKeyName, outString))
	{
		readString64(rootKey, inRegPath, inKeyName, outString);
	}

	return outString != _T("");
}

/**
	@brief ������Ʈ������ Ű���� ������ �о�´�(32��Ʈ)

	@param rootKey ��ƮŰ(HKEY_LOCAL_MACHINE)
	@param inRegPath ������Ʈ�� Ű���� �ִ� ���
	@param inKeyName ������Ʈ�� Ű���� �̸�
	@param outString �о�� �����

	@return ���� ����
*/
bool RegistryReader::readString32(HKEY rootKey, const std::wstring& inRegPath, const std::wstring& inKeyName, std::wstring& outString)
{
	HKEY regKey = 0;
	DWORD stringLength = 0;
	DWORD valueType = 0;
	TCHAR stringBuffer[4096] = {0,};
	TCHAR expandBuffer[4096] = {0,};

	outString = _T("");

	if (RegOpenKeyEx(rootKey, inRegPath.c_str(), 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &regKey) != ERROR_SUCCESS)
	{
		return false;
	}

	RegQueryValueEx(regKey, inKeyName.c_str(), NULL, &valueType, NULL, &stringLength);

	if (valueType == REG_SZ || valueType == REG_EXPAND_SZ)
	{
		RegQueryValueEx(regKey, inKeyName.c_str(), NULL, &valueType, (LPBYTE)stringBuffer, &stringLength);
		outString = stringBuffer;
		ExpandEnvironmentStrings(stringBuffer, expandBuffer, 4096);
		outString = expandBuffer;
	}

	RegCloseKey(regKey);

	return outString != _T("");
}

/**
	@brief ������Ʈ������ Ű���� ������ �о�´�(64��Ʈ)

	@param rootKey ��ƮŰ(HKEY_LOCAL_MACHINE)
	@param inRegPath ������Ʈ�� Ű���� �ִ� ���
	@param inKeyName ������Ʈ�� Ű���� �̸�
	@param outString �о�� �����

	@return ���� ����
*/
bool RegistryReader::readString64(HKEY rootKey, const std::wstring& inRegPath, const std::wstring& inKeyName, std::wstring& outString)
{
	HKEY regKey = 0;
	DWORD stringLength = 0;
	DWORD valueType = 0;
	TCHAR stringBuffer[4096] = {0,};
	TCHAR expandBuffer[4096] = {0,};

	outString = _T("");

	if (RegOpenKeyEx(rootKey, inRegPath.c_str(), 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &regKey) != ERROR_SUCCESS)
	{
		return false;
	}

	RegQueryValueEx(regKey, inKeyName.c_str(), NULL, &valueType, NULL, &stringLength);

	if (valueType == REG_SZ || valueType == REG_EXPAND_SZ)
	{
		RegQueryValueEx(regKey, inKeyName.c_str(), NULL, &valueType, (LPBYTE)stringBuffer, &stringLength);
		outString = stringBuffer;
		ExpandEnvironmentStrings(stringBuffer, expandBuffer, 4096);
		outString = expandBuffer;
	}

	RegCloseKey(regKey);

	return outString != _T("");
}