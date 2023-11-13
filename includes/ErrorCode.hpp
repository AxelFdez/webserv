#pragma once

#include "Server.hpp"

class ErrorCode
{
	public :
		ErrorCode();
		~ErrorCode();
		const std::string &getMessage(int code);
	private :
		std::map<int, std::string> _error;
};