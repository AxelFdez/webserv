#pragma once

#include "Server.hpp"

class GenerateHeader
{
	public :
		GenerateHeader(std::string path, int code);
		~GenerateHeader();
		const std::string &getHeader();

		void	headerFormat();
		void	errorHeaderFormat();

	private :
		std::string _path;
		std::string _header;
		int			_code;

};