#pragma once

#include "Server.hpp"

class ErrorCode
{
	public:

		ErrorCode();
		~ErrorCode();
		const std::string &getMessage(int code);

	private:
		ErrorCode( ErrorCode const& src );
		ErrorCode & operator=( ErrorCode const& rhs );
		
		std::map<int, std::string> _error;
};