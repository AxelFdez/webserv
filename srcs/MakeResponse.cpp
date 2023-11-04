#include "../includes/MakeResponse.hpp"

MakeResponse::MakeResponse(std::string request) : _request(request)
{
	handleRequest();
	//std::cout << "NEW REQUEST\n" << std::endl;
}

MakeResponse::~MakeResponse() {}

void	MakeResponse::handleRequest()
{
	mappedRequestFunc();
	driveRequest();
	//displayMappedRequest();
}

void	MakeResponse::mappedRequestFunc()
{
	std::istringstream stream(_request);
	std::string value;
	getline(stream, value);
	_mappedRequest.insert(std::pair<std::string, std::string>("request", value));
	while (getline(stream, value, ':'))
	{
		std::string key = value;
		getline(stream, value);
		value.erase(value.begin());
		if (std::all_of(value.begin(), value.end(), ::isspace))
			continue;
		_mappedRequest.insert(std::pair<std::string, std::string>(key, value));
	}

}

void		MakeResponse::displayMappedRequest()
{
	std::map<std::string, std::string>::iterator it;
	std::cout << "--------- REQUEST ---------" << std::endl;
	for (it = _mappedRequest.begin(); it != _mappedRequest.end(); it++)
	{
		std::cout << it->first << " : " << it->second << std::endl;
	}
	std::cout << "------- END REQUEST -------" << std::endl;
}

void	MakeResponse::driveRequest()
{
	std::istringstream split(_mappedRequest["request"]);
	std::string token;
	getline(split, token, ' ');
	std::string method = token;
	getline(split, token, ' ');
	std::string pathData = token;
	getline(split, token);
	std::string protocol = token;
	// std::string path = pathData.substr(0, pathData.find('?', 0));
	// std::cout << "path : " << path << std::endl;
	if (requestError(method, pathData, protocol) == false && _mappedRequest.find("host") == _mappedRequest.end())
	{
		_body = generateErrorPage(400);
		GenerateHeader header(pathData, 400);
		_header = header.getHeader();
		return;
	}
	GenerateBody body(method, pathData);
	_body = body.getBody();
	GenerateHeader header(body.getPath(), body.getCode());
	_header = header.getHeader();
}

std::string	MakeResponse::getResponse()
{
	return (_header + "\n" + _body);
}

// int MakeResponse::extensionPath(const std::string & path)
// {
// 	std::istringstream file(*(path.begin() + 1));
// 	std::string token;
// 	getline(file, token, '/');
// 	return 0;
// }

bool		MakeResponse::requestError(std::string method, std::string path,
		std::string protocol)
{
	if (method != "GET" || method != "POST" || method != "DELETE")
		return false;
	if (protocol != "HTTP/1.1")
		return false;
	if (*path.begin() != '/')
		return false;
	return true;
}
