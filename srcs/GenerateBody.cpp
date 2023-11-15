#include "../includes/GenerateBody.hpp"

GenerateBody::GenerateBody(std::map<std::string, std::string> request, std::string lineEnding, int serverNo, HandleConfigFile &config) : _request(request), _lineEnding(lineEnding), _serverNo(serverNo), _config(config)
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
	std::string ressource_path = _config.getLocationValues(_serverNo, _path, "root")[0];
	// std::cerr << "ressource_path = " << ressource_path << std::endl;
	_path = ressource_path + _path;
	if (_path[_path.size() - 1] == '/')
	{
		for (size_t i = 0; i < _config.getLocationValues(_serverNo, _path, "index").size(); i++)
		{
			if (access((ressource_path + "/" + _config.getLocationValues(_serverNo, _path, "index")[i]).c_str(), F_OK) == 0)
			{
				_path = ressource_path + "/" + _config.getLocationValues(_serverNo, _path, "index")[i];
				break;
			}
			if (i == _config.getLocationValues(_serverNo, _path, "index").size() - 1)
				_path = ressource_path + _path;
		}
	}

	if (_path[_path.size() - 1] == '/')
	{
		if (_config.getLocationValues(_serverNo, _path, "directory_listing")[0] == "on")
		{
			_responseBody = generateListingDirectoryPage(_path, "", true);
			_errorCode = 200;
			_responseHeader = "Content_Type: text/html\nContent-Length: " + std::to_string(_responseBody.size());
			return;
		}
		else
		{
			_errorCode = 403;
			_responseBody = generateErrorPage(_errorCode, _serverNo, _config);
			_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
			return;
		}
	}
	//std::cerr <<  "path = " << _path << std::endl;
	if (access(_path.c_str(), F_OK) == -1)
	{
		_errorCode = 404;
		_responseBody = generateErrorPage(_errorCode, _serverNo, _config);
		_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
		return;
	}
	else if (access(_path.c_str(), R_OK) == -1)
	{

		_errorCode = 403;
		_responseBody = generateErrorPage(_errorCode, _serverNo, _config);
		_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
		return;
	}
	for (size_t i = 0; i < _config.getLocationValues(_serverNo, _path, "methods").size(); i++)
	{
		if (_method == _config.getLocationValues(_serverNo, _path, "methods")[i])
		{
			break;
		}
		if (i == _config.getLocationValues(_serverNo, _path, "methods").size() - 1)
		{
			_errorCode = 405;
			_responseBody = generateErrorPage(_errorCode, _serverNo, _config);
			_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
			return;
		}
	}
	if (_method == "DELETE")
	{
		if(remove( _path.c_str() ) != 0)
		{
			perror( "Error deleting file" );
			_errorCode = 403;
			_responseBody = generateErrorPage(_errorCode, _serverNo, _config);
			_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
		}
		_errorCode = 204;
		return;
	}
	std::string extension = isolateExtension();
	// if (extension.empty())
	// {
	// 	_errorCode = 200;
	// 	_responseBody = "";
	// 	return;
	// }
	if (extension == ".php" || extension == ".py")
	{
		if (_method != "GET" && _method != "POST")
		{
			_errorCode = 405;
			_responseBody = generateErrorPage(405, _serverNo, _config);
			_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
			return;
		}
		CGI cgi(_path, _uri, _method, _request, _lineEnding, extension);
		_errorCode = cgi.getErrorCode();
		if (_errorCode >= 500)
		{
			_responseBody = generateErrorPage(_errorCode, _serverNo, _config);
			_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
			return;
		}
		_responseBody = cgi.getResponseBody();
		_responseHeader = cgi.getCgiHeader();
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
		_responseBody = generateErrorPage(400, _serverNo, _config);
		return false;
	}
	else if (_requestBody.size() > _config.getBodySizeMax(_serverNo))
	{
		_errorCode = 413;
		_responseBody = generateErrorPage(413, _serverNo, _config);
		return false;
	}
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

const std::string	&GenerateBody::getResponseHeader() const
{
	return _responseHeader;
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

std::string generateErrorPage(int errorCode, int serverNo, HandleConfigFile &config)
{

	std::string path = config.getErrorPage(serverNo, errorCode);
	if (!path.empty())
	{
		std::string extension = path.substr(path.find_last_of("."));
		if (access(path.c_str(), F_OK) == 0 && access(path.c_str(), R_OK) == 0
			&& (extension == ".html" || extension == ".htm"))
			return getRessource(path);
	}
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

std::string generateListingDirectoryPage(const std::string& path, const std::string& indent = "", bool isRoot = true)
{
	DIR *dir;
	struct dirent *entry;
	std::ostringstream html;

	if (isRoot)
	{
		html << "<html>\n<head><title>Directory Listing</title></head>\n<body>\n";
		html << "<h2>Listing " << path << "</h2>\n";
	}
	dir = opendir(path.c_str());
	if (dir == NULL) {
		perror("opendir");
		if (isRoot)
			html << "</body>\n</html>\n";
		return html.str();
	}
	html << indent << "<ul>\n";
	while ((entry = readdir(dir)) != NULL)
	{
		std::string entryName(entry->d_name);
		if (entryName == "." || entryName == "..") continue;
		html << indent << "  <li>" << (entry->d_type == DT_DIR ? "<b>" + entryName + "/</b>" : entryName);
		if (entry->d_type == DT_DIR) {
			html << generateListingDirectoryPage(path + "/" + entryName, indent + "    ", false);
			html << indent << "  </li>\n";
		} else {
			html << "</li>\n";
		}
	}
	html << indent << "</ul>\n";
	closedir(dir);
	if (isRoot)
		html << "</body>\n</html>\n";
	return html.str();
}