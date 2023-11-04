#pragma once

#include "Server.hpp"

class GenerateBody
{
	public :
		GenerateBody(std::string method, std::string pathData);
		~GenerateBody();

		void				handleMethod();
		void				GetMethod();
		//void				PostMethod();
		//void				DeleteMethod();
		const std::string	&getBody() const;
		const int			&getCode() const;
		const std::string	&getPath() const;
		// const struct stat	&getFileStat() const;

	private :
		int 		_code;
		std::string _method;
		std::string _uri;
		std::string _path;
		std::string _response;
};

std::string getContentType(const std::string& filePath);