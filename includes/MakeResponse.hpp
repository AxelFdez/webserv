#pragma once

#include "Server.hpp"
#include "GenerateBody.hpp"
#include "GenerateHeader.hpp"

class GenerateBody;
class GenerateHeader;

class MakeResponse
{
	public :
		MakeResponse(std::string request);
		~MakeResponse();
		std::string	getResponse();
		void		handleRequest();
		void		mappedRequestFunc();
		void		displayMappedRequest();
		void		driveRequest();
		bool		requestError(std::string, std::string,
		std::string);
		int			extensionPath(const std::string &);

	private :
		std::string	_body;
		std::string	_header;
		std::string _request;
		std::map<std::string, std::string> _mappedRequest;
};