#pragma once

#include "Server.hpp"

class MakeResponse
{
	public :
		MakeResponse(std::string request);
		~MakeResponse();
		std::string	returnResponse();
		void		handleRequest();
		void		mappedRequestFunc();
		void		displayMappedRequest();

	private :
		std::string _request;
		std::map<std::string, std::string> _mappedRequest;
};