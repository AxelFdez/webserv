#include "../includes/GenerateBody.hpp"

GenerateBody::GenerateBody(std::vector<char> binaryRequest, std::map<std::string, std::string> request, std::string lineEnding, int serverNo, HandleConfigFile &config) : _binaryRequest(binaryRequest), _request(request), _lineEnding(lineEnding), _serverNo(serverNo), _config(config)
{
	_requestBody = request["Body"];
	_requestHost = request["Host"].substr(0, request["Host"].find(':'));
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
	if (!requestValid())
	{
		return;
	}
	_path = _uri.substr(0, _uri.find('?', 0));
	if (!checkAuthorizedMethods())
	{
		return;
	}

	if (!checkRedirection())
	{
		return;
	}

	defineRoot();
	std::cout << "_path = "<< _path << std::endl;
	if (!isPathAccess())
	{
		return;
	}
	if (deleteMethod())
	{
		return;
	}
	if (checkDirectoryListing())
	{
		return;
	}

	std::string extension = isolateExtension();
	if (isCgiRequired(extension))
	{
		return;
	}

	if (uploadAsked())
	{
		return;
	}
	_responseBody = getRessource(_path);
	_errorCode = 200;
}

bool GenerateBody::checkRedirection()
{
	if (!_config.getLocationValues(_serverNo, _path, "redirect").empty() \
		&& _path != "/" + _config.getLocationValues(_serverNo, _path, "redirect")[0])
	{
		_errorCode = 301;
		_responseBody = generateErrorPage(_errorCode);
		_responseHeader = "Location: " + _config.getLocationValues(_serverNo, _path, "redirect")[0] + "\nContent-Length: " + std::to_string(_responseBody.size());
		return false;
	}
	return true;
}

void GenerateBody::defineRoot()
{
	std::string ressource_path = _config.getLocationValues(_serverNo, _path, "root")[0]; //+ _path.substr(_path.find_last_of("/")); // coller toute la fin de l'uri apres le root.
	//std::cout << "path = " << _path << std::endl;
	//std::cout << "ressource_path = " << ressource_path << std::endl;
	size_t pos = ressource_path.find(_path);
	//std::cout << "pos = " << pos << std::endl;
	if (pos != std::string::npos)
		ressource_path.replace(pos, _path.length(), _path);
	else
		ressource_path += _path;
	if (isFile(ressource_path.c_str()))
	{
		_path = ressource_path;
	}
	else if (isDir(ressource_path.c_str()))
	{
		if (ressource_path[ressource_path.size() - 1] != '/')
			ressource_path += "/";
		if (_config.getLocationValues(_serverNo, _path, "index").size() == 0)
			_path = ressource_path;
		for (size_t i = 0; i < _config.getLocationValues(_serverNo, _path, "index").size(); i++)
		{
			if (access((ressource_path +_config.getLocationValues(_serverNo, _path, "index")[i]).c_str(), F_OK) == 0)
			{
				if (_config.getLocationValues(_serverNo, _path, "index")[i][0] == '/')
					_path = ressource_path + _config.getLocationValues(_serverNo, _path, "index")[i].erase(0, 1);
				else
					_path = ressource_path + _config.getLocationValues(_serverNo, _path, "index")[i];
				break;
			}
			if (i == _config.getLocationValues(_serverNo, _path, "index").size() - 1)
			{
				_path = ressource_path;
			}
		}
	}
}

bool GenerateBody::checkDirectoryListing()
{
	if (isDir(_path.c_str()))
	{
		if (_config.getLocationValues(_serverNo, _path, "directory_listing")[0] == "on")
		{
			_responseBody = generateListingDirectoryPage(_path, "", true);
			_errorCode = 200;
			_responseHeader = "Content_Type: text/html\nContent-Length: " + std::to_string(_responseBody.size());
			return true;
		}
	}
	return false;
}

bool GenerateBody::isPathAccess()
{
	if (access(_path.c_str(), F_OK) == -1)
	{
		_errorCode = 404;
		_responseBody = generateErrorPage(_errorCode);
		_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
		return false;
	}
	else if (access(_path.c_str(), R_OK) == -1)
	{

		_errorCode = 403;
		_responseBody = generateErrorPage(_errorCode);
		_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
		return false;
	}
	return true;
}

bool GenerateBody::deleteMethod()
{
	if (_method == "DELETE")
	{
		if(isDir(_path.c_str()) && remove( _path.c_str() ) != 0)
		{
			perror( "Error deleting file" );
			_errorCode = 403;
			_responseBody = generateErrorPage(_errorCode);
			_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
			return true;
		}
		_errorCode = 204;
		return true;
	}
	return false;
}

bool GenerateBody::isCgiRequired(std::string extension)
{
	if (extension == ".php" || extension == ".py")
	{
		if (_method != "GET" && _method != "POST")
		{
			_errorCode = 405;
			_responseBody = generateErrorPage(405);
			_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
			return true;
		}
		CGI cgi(_path, _uri, _method, _request, _lineEnding, extension);
		_errorCode = cgi.getErrorCode();
		if (_errorCode >= 500)
		{
			_responseBody = generateErrorPage(_errorCode);
			_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
			return true;
		}
		_responseBody = cgi.getResponseBody();
		_responseHeader = cgi.getCgiHeader();
		return true;
	}
	return false;
}

bool GenerateBody::uploadAsked()
{
	if (_request["Content-Type"].find("multipart/form-data") != std::string::npos)
	{
		if (_method == "POST" && _config.getLocationValues(_serverNo, _path, "download")[0] == "on")
		{
			if (!_request["Body"][0])
			{
				_errorCode = 400;
				std::cout << ("empty body") << std::endl;
				_responseBody = generateErrorPage(_errorCode);
				_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
				return true;
			}
			std::string filename = _request["Body"].substr(_request["Body"].find("filename=") + 10);
			filename = filename.substr(0, filename.find("\""));
			std::vector<char> binaryData;
			char *str = strstr(_binaryRequest.data(), "\r\n\r\n") + 4;
			str = strstr(str, "\r\n\r\n") + 4;
			size_t sizeBody;
			if (_request["Body"].find("Content-Type: text/plain") != std::string::npos)
			{
				char *end = strstr(str, "\r\n");
				sizeBody = end - str;
			}
			else
			{
				sizeBody = _binaryRequest.size() - (str - _binaryRequest.data());
			}
			if (str)
			{
				for (size_t i = 0; i < sizeBody; i++)
				{
					binaryData.push_back(str[i]);
				}
			}
			if (!_config.getLocationValues(_serverNo, _path, "download_folder")[0].empty() \
				&& access(_config.getLocationValues(_serverNo, _path, "download_folder")[0].c_str(), F_OK) == 0 \
				&& access(_config.getLocationValues(_serverNo, _path, "download_folder")[0].c_str(), W_OK) == 0 \
				&& isDir(_config.getLocationValues(_serverNo, _path, "download_folder")[0].c_str()))
			{
				std::ofstream file;
				if (_config.getLocationValues(_serverNo, _path, "download_folder")[0][_config.getLocationValues(_serverNo, _path, "download_folder")[0].size() - 1] != '/')
					file.open(_config.getLocationValues(_serverNo, _path, "download_folder")[0] + "/" + filename, std::ios::binary);
				else
					file.open(_config.getLocationValues(_serverNo, _path, "download_folder")[0] + filename, std::ios::binary);
				if (file.is_open())
				{
					for (size_t i = 0; i < binaryData.size(); i++)
					{
						file << binaryData[i];
					}
					file.close();
				}
				else
				{
					_errorCode = 500;
					_responseBody = generateErrorPage(_errorCode);
					_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
					return true;
				}
			}
			else
			{
				_errorCode = 403;
				_responseBody = generateErrorPage(_errorCode);
				_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
				return true;
			}
			_errorCode = 201;
			_responseBody = generateErrorPage(_errorCode);
			_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
			return true;
		}
		else
		{
			_errorCode = 403;
			_responseBody = generateErrorPage(_errorCode);
			_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
			return true;
		}
	}
	return false;
}

bool	GenerateBody::requestValid()
{
	if (_method != "GET" && _method != "POST" && _method != "DELETE"
		&& _protocol != "HTTP/1.1" && *_uri.begin() != '/' && _requestHost.empty())
	{
		_errorCode = 400;
		_responseBody = generateErrorPage(400);
		return false;
	}
	else if (_requestHost != _config.getServerValues(_serverNo, "host") \
		&& _requestHost != _config.getServerValues(_serverNo, "server_name"))
	{
		_errorCode = 404;
		_responseBody = generateErrorPage(404);
		return false;
	}
	else if (_requestBody.size() > static_cast<size_t>(_config.getBodySizeMax(_serverNo)))
	{
		_errorCode = 413;
		_responseBody = generateErrorPage(413);
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

std::string GenerateBody::getRessource(const std::string &path)
{
	std::ifstream ifs(path.c_str(), std::ios::binary);
    if (!ifs)
        return "";

    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

std::string GenerateBody::generateErrorPage(int errorCode)
{

	std::string path = _config.getErrorPage(_serverNo, errorCode);
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
	html << "<title>Code " << errorCode << "</title>\n";
	html << "<style>\n";
	html << "html { color-scheme: light dark; }";
	html << "body { font-family: Arial, sans-serif; text-align: center; padding: 50px; }\n";
	html << "h1 { color: #d33; }\n";
	html << "</style>\n";
	html << "</head>\n";
	html << "<body>\n";
	html << "<h1>Code " << errorCode << "</h1>\n";
	html << "<h2>" << errorMessage.getMessage(errorCode) << "</h2>\n";
	html << "<p>" << "webserv/chmassa-axfernan" << "</p>\n";
	html << "</body>\n";
	html << "</html>\n";
	return html.str();
}

std::string GenerateBody::generateListingDirectoryPage(const std::string& path, const std::string& indent = "", bool isRoot = true)
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

bool GenerateBody::isFile(const char* path) {
    struct stat fileInfo;

    if (stat(path, &fileInfo) != 0) {

        return false;
    }

    return S_ISREG(fileInfo.st_mode);
}

bool GenerateBody::isDir(const char* path) {
    struct stat dirInfo;
    if (stat(path, &dirInfo) != 0) {
        return false;
    }
    return S_ISDIR(dirInfo.st_mode);
}


bool GenerateBody::checkAuthorizedMethods()
{
	for(size_t i = 0; i < _config.getLocationValues(_serverNo, _path, "methods").size(); i++)
	{
		if (_method == _config.getLocationValues(_serverNo, _path, "methods")[i])
		{
			break;
		}
		if (i == _config.getLocationValues(_serverNo, _path, "methods").size() - 1)
		{
			_errorCode = 405;
			_responseBody = generateErrorPage(405);
			_responseHeader = "Content-Length: " + std::to_string(_responseBody.size());
			return false;
		}
	}
	return true;
}