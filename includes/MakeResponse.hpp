#pragma once

#include "Server.hpp"
#include "GenerateBody.hpp"
#include "GenerateHeader.hpp"

class GenerateBody;
class GenerateHeader;

class MakeResponse
{
	public :
		MakeResponse(std::string request, int serverNo, HandleConfigFile &config);
		~MakeResponse();
		std::string	getResponse();
		void		handleRequest();
		void		mappedRequest();
		void		displayMappedRequest();
		void		generateResponse();

	private :
		HandleConfigFile _config;
		std::string 	_request;
		int				_serverNo;
		std::map<std::string, std::string> _mappedRequest;
		std::string 	_requestBody;
		std::string 	_responseHeader;
		std::string		_responseBody;
		std::string		_lineEnding;
};

std::string	detectLineEnding(const std::string&);