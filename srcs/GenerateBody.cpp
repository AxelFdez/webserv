#include "../includes/GenerateBody.hpp"

GenerateBody::GenerateBody(std::map<std::string, std::string> request, std::string lineEnding) : _request(request), _lineEnding(lineEnding)
{
	_requestBody = request["Body"];
	std::istringstream split(request["RequestLine"]);
	getline(split, _method, ' ');
	getline(split, _uri, ' ');
	getline(split, _protocol, _lineEnding[0]);
	if (_method == "GET")
		_requestBody.clear();
	handleRequest();
}

GenerateBody::~GenerateBody() {}

void GenerateBody::handleRequest()
{
	if (requestErrors() == false)
	{
		return;
	}
	_path = _uri.substr(0, _uri.find('?', 0));
	if (_path == "/")
		_path = "/index.html";
	_path = "/Users/chris/Desktop/webServe42/srcs/tools" + _path; // recuperer le path du fichier de config
	//std::cout << "path = " << _path << std::endl;
	if (access(_path.c_str(), F_OK) == -1)
	{
		_errorCode = 404;
		_responseBody = generateErrorPage(_errorCode);
		return;
	}
	else if (access(_path.c_str(), R_OK) == -1)
	{
		
		_errorCode = 403;
		_responseBody = generateErrorPage(_errorCode);
		return;
	}
	if (_method == "DELETE")
	{
		if(remove( _path.c_str() ) != 0)
		{
			perror( "Error deleting file" );
			_errorCode = 403;
			_responseBody = generateErrorPage(_errorCode);
		}
		_errorCode = 204;
		return;
	}
	std::string extension = isolateExtension();
	if (extension.empty())
	{
		_errorCode = 200;
		_responseBody = "";
		return;
	}
	if (extension == ".php" || extension == ".py")
	{
		if (_method != "GET" && _method != "POST")
		{
			_errorCode = 405;
			_responseBody = generateErrorPage(405);
			return;
		}
		CGI cgi(_path, _uri, _method, _request, _lineEnding, extension);
		_responseBody = cgi.getResponseBody();
		_errorCode = cgi.getErrorCode();
		_cgiHeader = cgi.getCgiHeader();
	}
	else
	{
		_responseBody = getRessource(_path);
		_errorCode = 200;
	}
}

bool	GenerateBody::requestErrors()
{
	if (_method != "GET" && _method != "POST" && _method != "DELETE"
		&& _protocol != "HTTP/1.1" && *_uri.begin() != '/')
		{
			_errorCode = 400;
			_responseBody = generateErrorPage(400);
			return false;
		}
	// else if (_requestBody.size() > BODY_SIZE_MAX)
	// {
	// 	_errorCode = 413;
	// 	_responseBody = generateErrorPage(413);
	// 	return false;
	// }
	return true;
}

std::string GenerateBody::isolateExtension()
{
	std::size_t lastIndex = _path.find_last_of(".");
	std::string extension;
	if (lastIndex != std::string::npos)
	{
		extension = _path.substr(lastIndex);
		return extension;
	}
	return "";
}

const std::string &GenerateBody::getBody() const
{
	return _responseBody;
}

const int			&GenerateBody::getCode() const
{
	return _errorCode;
}

const std::string	&GenerateBody::getPath() const
{
	return _path;
}

const std::string	&GenerateBody::getCgiHeader() const
{
	return _cgiHeader;
}

std::string getRessource(const std::string &path)
{
	std::ifstream ifs(path.c_str(), std::ios::binary);
    if (!ifs)
        return "";

    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

std::string generateErrorPage(int errorCode) {
	ErrorCode errorMessage;
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html lang=\"en\">\n";
    html << "<head>\n";
    html << "<meta charset=\"UTF-8\">\n";
    html << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "<title>Error " << errorCode << "</title>\n";
    html << "<style>\n";
	html << "html { color-scheme: light dark; }";
    html << "body { font-family: Arial, sans-serif; text-align: center; padding: 50px; }\n";
    html << "h1 { color: #d33; }\n";
    html << "</style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "<h1>Error " << errorCode << "</h1>\n";
    html << "<h2>" << errorMessage.getMessage(errorCode) << "</h2>\n";
    html << "<p>" << "webserv/chmassa-axfernan" << "</p>\n";
    html << "</body>\n";
    html << "</html>\n";
    return html.str();
}
