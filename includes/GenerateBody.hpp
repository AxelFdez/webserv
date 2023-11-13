#pragma once

#include "Server.hpp"
#include "ErrorCode.hpp"
#include "CGI.hpp"

class GenerateBody
{
	public :
		GenerateBody(std::map<std::string, std::string> request, std::string body);
		~GenerateBody();

		void				handleRequest();
		bool				requestErrors();
		std::string			isolateExtension();
		const std::string	&getBody() const;
		const int			&getCode() const;
		const std::string	&getPath() const;
		const std::string	&getCgiHeader() const;

	private :
		std::map<std::string, std::string> _request;
		int 		_errorCode;
		std::string _method;
		std::string _uri;
		std::string _protocol;
		std::string _path;
		std::string _requestBody;
		std::string _responseBody;
		std::string _cgiHeader;
		std::string	_lineEnding;
};

std::string getRessource(const std::string &path);
std::string generateErrorPage(int errorCode);
//std::vector<std::string> envCGI(std::string uri, std::string method, std::map<std::string, std::string>);
