#pragma once

#include "Server.hpp"
#include "GenerateBody.hpp"

class CGI
{
	public :
		CGI(std::string path, std::string uri, std::string method, std::map<std::string, std::string> request, std::string lineEnding, std::string extension);
		~CGI();
		void						executeCGI();
		std::string					getDirectory();
		bool						fillResponseFrom(int stdoutPipe);
		std::vector<std::string>	envCGI();
		int 						getErrorCode() const;
		const std::string 			&getResponseBody() const;
		const std::string 			&getCgiHeader() const;


	private :
		CGI();
		int _errorCode;
		std::string _responseBody;
		std::string _requestBody;
		std::string _path;
		std::string	_uri;
		std::string _method;
		std::string _lineEnding;
		std::string	_cgiHeader;
		std::string	_extension;
		std::map<std::string, std::string> _request;


};