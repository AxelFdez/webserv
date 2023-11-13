#pragma once

#include "Server.hpp"
#include "ErrorCode.hpp"

class GenerateHeader
{
	public :
		GenerateHeader(std::string path, int code, std::string cgiHeader);
		~GenerateHeader();
		const std::string &getHeader();

		void	headerFormat();
		void	errorHeaderFormat();

	private :
		std::string _path;
		std::string _header;
		std::string _cgiHeader;
		int			_code;

};