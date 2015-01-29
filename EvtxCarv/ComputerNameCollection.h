/**
	@file ComputerNameCollection.h
	@brief ��ǻ���� �̸��� ��Ƴ��� Ŭ������ ����
*/

#ifndef _COMPUTER_NAME_COLLECTION_H_
#define _COMPUTER_NAME_COLLECTION_H_

#include <string>
#include <set>

/**
	@class ComputerNameCollection
	@brief ��ǻ���� �̸��� ��Ƴ��� Ŭ����
*/
class ComputerNameCollection
{
	public:
		static ComputerNameCollection* getInstance();

		void insertComputerName(const std::wstring& inComputerName);
		std::wstring getComputerName(const std::wstring& inString);

	private:
		std::set<std::wstring> computerNames_;  ///< ��ǻ�� �̸����� ����
};

#endif /* _COMPUTER_NAME_COLLECTION_H_ */