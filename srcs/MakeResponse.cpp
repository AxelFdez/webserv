#include "../includes/MakeResponse.hpp"

MakeResponse::MakeResponse(std::vector<char> binaryRequest, int serverNo, HandleConfigFile &config) : _binaryRequest(binaryRequest), _serverNo(serverNo), _config(config)
{
	for (int i = 0; i < binaryRequest.size(); i++)
		_request.push_back(binaryRequest[i]);
	_request.push_back('\0');
	// displayRequest(_request, 0);
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
	// access_logs();
	displayMappedRequest();
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

void MakeResponse::access_logs( std::string IP ) {

	std::map<std::string, std::string> map;
	map["IP:"] = IP;
	request_response_to_map( map, _request, "Request:" );
	request_response_to_map( map, _responseHeader, "Response-Code:" );
	std::string serverName = _config.getServerValues( _serverNo, "server_name" );
	if ( !serverName.empty() ) {
		map["Server-Name:"] = _config.getServerValues( _serverNo, "server_name" );
	}
	
	
	// for ( std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); it++ ) {
	// 	std::cerr << it->first << " " << it->second << std::endl;
	// }
	// std::cerr << std::endl;
	
	std::string token[9] = { "IP:", "Host:", "Server-Name:", "Date:", "Request:", "Response-Code:", "Content-Length:", "Referer:", "User-Agent:" }; 
	std::map<std::string, std::string>::iterator it;

	// std::ostringstream out;
	for ( size_t i = 0; i < 9; i++ ) {
		
		it = map.find( token[i] );
		if ( it != map.end() ) 
			std::cerr << B_GRAY << it->first << " " << B_GREEN << it->second << RESET << std::endl;
		else
			std::cerr << B_GRAY << token[i] << " " << "-" << RESET << std::endl;
	}
	// std::cerr << B_GREEN << out.str() << RESET << std::endl;

}
void request_response_to_map( std::map<std::string, std::string> & map, std::string toMap, std::string type ) {

	
	std::vector<std::string> vec;
	std::string line;
	for (std::string::const_iterator it = toMap.begin(); it != toMap.end(); ++it) {
        
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
	std::vector<std::string>::iterator it = vec.begin();
	map.insert(std::pair<std::string, std::string>( type, *it++ ));
	for ( ; it != vec.end(); it++ ) {

		if ( (*it).find(':') ) {

			std::string key = (*it).substr(0, (*it).find_first_of(':') +1 );
			std::string value = (*it).substr((*it).find_first_of(':') +2 );
			if ( key == "Host:" ) {
				if ( value.find(':') != std::string::npos ) {
					value = value.substr( 0, value.find(':') );
				}
			}
			map[key] = value;
		}
	}
	std::map<std::string, std::string>::iterator itMap = map.find( "Response-Code:" );
	if ( itMap != map.end() ) {
		if ( itMap->second.find(' ') != std::string::npos ) {

			itMap->second = itMap->second.substr( itMap->second.find(' '));
		}
		std::istringstream iss(itMap->second);
		iss >> itMap->second;
	}
}
// void request_response_to_map( std::map<std::string, std::string> & map, std::string toMap, std::string type ) {

// 	std::vector<std::string> vec;
// 	std::string line;
// 	for (std::string::const_iterator it = toMap.begin(); it != toMap.end(); ++it) {
        
// 		char c = *it;
//         if (c == '\n') {
//             vec.push_back(line);
//             line.clear(); 
//         }
// 		else {
//             line.push_back(c);
//         }
//     }
//     if (!line.empty()) {
//         vec.push_back(line);
//     }
// 	std::vector<std::string>::iterator it = vec.begin();
// 	map.insert(std::pair<std::string, std::string>( type, *it++ ));
// 	for ( ; it != vec.end(); it++ ) {

// 		if ( (*it).find(':') ) {

// 			std::string key = (*it).substr(0, (*it).find_first_of(':') +1 );
// 			std::string value = (*it).substr((*it).find_first_of(':') +2 );
// 			if ( key == "Host:" ) {
// 				if ( value.find(':') != std::string::npos ) {
// 					value = value.substr( 0, value.find(':') );
// 				}
// 			}
// 			map.insert(std::pair<std::string, std::string>( key, value ) );
// 		}
// 	}
// 	std::map<std::string, std::string>::iterator itMap = map.find( "Response-Code:" );
// 	if ( itMap != map.end() ) {
// 		if ( itMap->second.find(' ') != std::string::npos ) {

// 			itMap->second = itMap->second.substr( itMap->second.find(' '));
// 		}
// 		std::istringstream iss(itMap->second);
// 		iss >> itMap->second;
// 	}
// }

