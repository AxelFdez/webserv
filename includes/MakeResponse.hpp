#pragma once

#include "Server.hpp"
#include "GenerateBody.hpp"
#include "GenerateHeader.hpp"

class GenerateBody;
class GenerateHeader;

class MakeResponse {		
		
	public:

		MakeResponse(std::vector<char> request, int serverNo, HandleConfigFile &config);
		~MakeResponse();
		std::string	getResponse();
		void		handleRequest();
		void		mappedRequest();
		void		displayMappedRequest();
		void		generateResponse();

		void access_logs( std::string IP );
	
	private:

		MakeResponse( MakeResponse const& src );
		MakeResponse & operator=( MakeResponse const& rhs );

		HandleConfigFile _config;
		std::string	_request;
		std::vector<char> 	_binaryRequest;
		int				_serverNo;
		std::map<std::string, std::string> _mappedRequest;
		std::string 	_requestBody;
		std::string 	_responseHeader;
		std::string		_responseBody;
		std::string		_lineEnding;
		void getLogsInfoToMap( std::map<std::string, std::string> & map);

};

std::string	detectLineEnding(const std::string&);
