#pragma once

#include "Server.hpp"
#include "ErrorCode.hpp"
#include "CGI.hpp"

class GenerateBody
{
	public :
		GenerateBody(std::map<std::string, std::string> request, std::string lineEnding, int serverNo, HandleConfigFile &config);
		~GenerateBody();

		void				handleRequest();
		bool				requestErrors();
		std::string			isolateExtension();
		const std::string	&getBody() const;
		const int			&getCode() const;
		const std::string	&getPath() const;
		const std::string	&getResponseHeader() const;

	private :
		std::map<std::string, std::string> _request;
		std::string		_lineEnding;
		int				_serverNo;
		HandleConfigFile _config;
		int				_errorCode;
		std::string		_requestHost;
		std::string		_method;
		std::string 	_uri;
		std::string 	_protocol;
		std::string 	_path;
		std::string 	_requestBody;
		std::string 	_responseBody;
		std::string 	_responseHeader;
};

std::string getRessource(const std::string &path);
std::string generateErrorPage(int errorCode, int serverNo, HandleConfigFile &config);
std::string generateListingDirectoryPage(const std::string& path, const std::string& indent, bool isRoot);
