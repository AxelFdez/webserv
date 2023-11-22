#include "../includes/MakeResponse.hpp"

MakeResponse::MakeResponse(std::vector<char> binaryRequest, int serverNo, HandleConfigFile &config) : _binaryRequest(binaryRequest), _serverNo(serverNo), _config(config)
{
	for (int i = 0; i < binaryRequest.size(); i++)
		_request.push_back(binaryRequest[i]);
	_request.push_back('\0');
	//displayRequest(_request, 0);
	_lineEnding = detectLineEnding(_request);
	handleRequest();
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
	_mappedRequest.insert(std::pair<std::string, std::string>("Request", value));
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

void MakeResponse::access_logs( std::string IP ) {

	std::map<std::string, std::string> map;
	map["IP:"] = IP;
	getLogsInfoToMap( map );
	std::string serverName = _config.getServerValues( _serverNo, "server_name" );
	if ( !serverName.empty() ) {
		map["Server-Name:"] = _config.getServerValues( _serverNo, "server_name" );
	}
	std::string token[9] = { "IP:", "Host:", "Server-Name:", "Date:", "Request:", "Response-Code:", "Content-Length:", "Referer:", "User-Agent:" };
	std::map<std::string, std::string>::iterator it;

	for ( size_t i = 0; i < 9; i++ ) {

		it = map.find( token[i] );
		if ( it != map.end() )
			std::cerr << B_GRAY << it->first << " " << B_GREEN << it->second << RESET << std::endl;
		else
			std::cerr << B_GRAY << token[i] << " " << "-" << RESET << std::endl;
	}
}

void MakeResponse::getLogsInfoToMap( std::map<std::string, std::string> & map ) {

	std::map<std::string, std::string>::iterator itMap;
	std::string toFind[4] = {"Request", "User-Agent", "Host", "Referer"};
	for ( size_t i = 0; i < 4; i++ ) {

		itMap = _mappedRequest.find(toFind[i]);
		if ( itMap != _mappedRequest.end() ) {
			if ( toFind[i] == "Host" ) {
				if ( itMap->second.find(':') != std::string::npos ) {
					itMap->second = itMap->second.substr( 0, itMap->second.find(':') );
				}
			}
			map[toFind[i] + ':'] = itMap->second;
		}
	}
	std::vector<std::string> vec;
	std::string line;
	for (std::string::const_iterator it = _responseHeader.begin(); it != _responseHeader.end(); ++it) {
		char c = *it;
        if (c == '\n') {
            vec.push_back(line);
            line.clear();
        }
		else {
            line.push_back(c);
        }
    }
    if (!line.empty()) {
        vec.push_back(line);
    }
	std::vector<std::string>::iterator itVec = vec.begin();
	*itVec = (*itVec).substr((*itVec).find(' '));
	std::istringstream iss(*itVec);
	iss >> *itVec;
	map["Response-Code:"] = *itVec;
	for ( ; itVec != vec.end(); itVec++ ) {

		if ( (*itVec).find(':') ) {

			std::string key = (*itVec).substr(0, (*itVec).find_first_of(':') +1 );
			std::string value = (*itVec).substr((*itVec).find_first_of(':') +2 );
			map[key] = value;
		}
	}
}
