#include "../includes/MakeResponse.hpp"

MakeResponse::MakeResponse(std::vector<char> binaryRequest, int serverNo, HandleConfigFile &config) : _binaryRequest(binaryRequest), _serverNo(serverNo), _config(config)
{
	for (int i = 0; i < binaryRequest.size(); i++)
		_request.push_back(binaryRequest[i]);
	_request.push_back('\0');
	displayRequest(_request, 0);
	_lineEnding = detectLineEnding(_request);
	handleRequest();
	// std::cout << "REQUEST : " << _request << std::endl;
	//std::cout << "NEW REQUEST\n" << std::endl;
}

MakeResponse::~MakeResponse() {}

void	MakeResponse::handleRequest()
{
	mappedRequest();
	generateResponse();
	//displayMappedRequest();
}

void	MakeResponse::mappedRequest()
{
	std::istringstream stream(_request);
	std::string line;
	std::string key, value;
	getline(stream, value);
	_mappedRequest.insert(std::pair<std::string, std::string>("RequestLine", value));
	while (getline(stream, line) && line != _lineEnding.substr(0, 1) && line != "")
	{
		std::istringstream lineStream(line);
		if(getline(lineStream, key, ':'))
			if (getline(lineStream, value))
			{
				_mappedRequest[key] = value.substr(value.find_first_not_of(" "));
			}
	}
	line.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	if (line.empty())
		_mappedRequest["Body"];
	else
		_mappedRequest.insert(std::pair<std::string, std::string>("Body", line));
}

void	MakeResponse::generateResponse()
{
	GenerateBody body(_binaryRequest, _mappedRequest, _lineEnding, _serverNo, _config);
	_responseBody = body.getBody();
	GenerateHeader header(body.getPath(), body.getCode(), body.getResponseHeader());
	_responseHeader = header.getHeader();
}

std::string	MakeResponse::getResponse()
{
	return (_responseHeader + _lineEnding + _lineEnding + _responseBody);
}

void		MakeResponse::displayMappedRequest()
{
	std::map<std::string, std::string>::iterator it;
	std::cout << "--------- MAPPED REQUEST ---------" << std::endl;
	for (it = _mappedRequest.begin(); it != _mappedRequest.end(); it++)
	{
		std::cout << it->first << " : " << it->second << std::endl;
	}
	std::cout << "------- END MAPPED REQUEST -------" << std::endl;
}

std::string	detectLineEnding(const std::string& request)
{
	if (request.find("\r\n") != std::string::npos)
		return "\r\n";
	else if (request.find("\n") != std::string::npos)
		return "\n";
	return "";
}
