#include "../includes/MakeResponse.hpp"

MakeResponse::MakeResponse(std::string request) : _request(request)
{
	handleRequest();
}

MakeResponse::~MakeResponse() {}

void		MakeResponse::handleRequest()
{
	mappedRequestFunc();
	displayMappedRequest();
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


std::string	MakeResponse::returnResponse() {}
