#pragma once

#include "Server.hpp"
#include "ErrorCode.hpp"
#include "CGI.hpp"

class GenerateBody
{
	public :
		GenerateBody(std::vector<char> binaryRequest, std::map<std::string, std::string> request, std::string lineEnding, int serverNo, HandleConfigFile &config);
		~GenerateBody();

		const std::string	&getBody() const;
		const int			&getCode() const;
		const std::string	&getPath() const;
		const std::string	&getResponseHeader() const;

	private:

		GenerateBody( GenerateBody const& src );
		GenerateBody & operator=( GenerateBody const& rhs );

		std::vector<char>	_binaryRequest;
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
		std::string 	_root;
		std::string 	_requestBody;
		std::string 	_responseBody;
		std::string 	_responseHeader;


		void		handleRequest();
		bool		requestValid();
		bool 		checkAuthorizedMethods();
		bool		checkRedirection(std::string ressource_path);
		bool		defineRoot();
		bool		isPathAccess();
		bool		checkDirectoryListing();
		bool		deleteMethod();
		bool		isCgiRequired(std::string extension);
		bool		uploadAsked();
		std::string	isolateExtension();
		bool		isFile(const char* path);
		bool		isDir(const char* path);
		std::string getRessource(const std::string &path);
		std::string generateErrorPage(int errorCode);
		std::string generateListingDirectoryPage(const std::string& path, const std::string& indent, bool isRoot);
};

