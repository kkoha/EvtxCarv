/**
	@file RegistryReader.cpp
	@brief 레지스트리의 내용을 읽기위한 클래스의 구현
*/

#include "RegistryReader.h"
#include <tchar.h>

/**
	@brief 레지스트리에서 키값의 내용을 읽어온다(32/64비트 상관없이)

	@param rootKey 루트키(HKEY_LOCAL_MACHINE)
	@param inRegPath 레지스트리 키값이 있는 경로
	@param inKeyName 레지스트리 키값의 이름
	@param outString 읽어온 결과값

	@return 성공 여부
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
	@brief 레지스트리에서 키값의 내용을 읽어온다(32비트)

	@param rootKey 루트키(HKEY_LOCAL_MACHINE)
	@param inRegPath 레지스트리 키값이 있는 경로
	@param inKeyName 레지스트리 키값의 이름
	@param outString 읽어온 결과값

	@return 성공 여부
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
	@brief 레지스트리에서 키값의 내용을 읽어온다(64비트)

	@param rootKey 루트키(HKEY_LOCAL_MACHINE)
	@param inRegPath 레지스트리 키값이 있는 경로
	@param inKeyName 레지스트리 키값의 이름
	@param outString 읽어온 결과값

	@return 성공 여부
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