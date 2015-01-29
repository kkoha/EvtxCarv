/**
	@file ComputerNameCollection.h
	@brief 컴퓨터의 이름을 모아놓은 클래스의 정의
*/

#ifndef _COMPUTER_NAME_COLLECTION_H_
#define _COMPUTER_NAME_COLLECTION_H_

#include <string>
#include <set>

/**
	@class ComputerNameCollection
	@brief 컴퓨터의 이름을 모아놓은 클래스
*/
class ComputerNameCollection
{
	public:
		static ComputerNameCollection* getInstance();

		void insertComputerName(const std::wstring& inComputerName);
		std::wstring getComputerName(const std::wstring& inString);

	private:
		std::set<std::wstring> computerNames_;  ///< 컴퓨터 이름들의 모음
};

#endif /* _COMPUTER_NAME_COLLECTION_H_ */