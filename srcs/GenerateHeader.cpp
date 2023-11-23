#include "../includes/GenerateHeader.hpp"

GenerateHeader::GenerateHeader(std::string path, int code, std::string cgiHeader) : _path(path), _code(code), _cgiHeader(cgiHeader)
{
	if (_code >= 400)
		errorHeaderFormat();
	else
		headerFormat();
}

GenerateHeader::~GenerateHeader() {}

void GenerateHeader::headerFormat()
{
	ErrorCode errorMessage;
	struct stat	fileStat;
	stat(_path.c_str(), &fileStat);
	char lastModifDate[80];
    strftime(lastModifDate, sizeof(lastModifDate), "%a, %d %b %Y %H:%M:%S GMT", localtime(&fileStat.st_mtime));
	std::time_t now = std::time(NULL);
	char creationDate[80];
    strftime(creationDate, sizeof(creationDate), "%a, %d %b %Y %H:%M:%S GMT", localtime(&now));

	_header = "HTTP/1.1 " + std::to_string(_code) + " " + errorMessage.getMessage(_code) + "\n" \
	+ "Server: webserv/chmassa-axfernan\n" \
	+ "Date: " + creationDate + "\n" \
	+ "Last-Modified: " + lastModifDate + "\n";
	if (_cgiHeader.empty())
	{
		_header.append("Content-Type: " + getContentType(_path) + "\n");
		_header.append("Content-Length: " + std::to_string(fileStat.st_size));
	}
	else
	{
		_header.append(_cgiHeader);
	}
}

void GenerateHeader::errorHeaderFormat()
{
	ErrorCode errorMessage;
	std::time_t now = std::time(NULL);
	char creationDate[80];
    strftime(creationDate, sizeof(creationDate), "%a, %d %b %Y %H:%M:%S GMT", localtime(&now));

	_header = "HTTP/1.1 " + std::to_string(_code) + " " + errorMessage.getMessage(_code) + "\n" \
	+ "Server: webserv/chmassa-axfernan\n" \
	+ "Date: " + creationDate + "\n" \
	+ "Content-Type: text/html\n"\
	+ "Connection: close";
}


const std::string &GenerateHeader::getHeader()
{
	return _header;
}

std::string getContentType(const std::string& filePath)
{
    std::map<std::string, std::string> contentTypes;
    contentTypes[".html"] = "text/html; charset=UTF-8";
    contentTypes[".txt"] = "text/plain";
    contentTypes[".css"] = "text/css";
    contentTypes[".js"] = "application/javascript";
    contentTypes[".json"] = "application/json";
    contentTypes[".xml"] = "application/xml";
    contentTypes[".png"] = "image/png";
    contentTypes[".jpg"] = "image/jpeg";
    contentTypes[".jpeg"] = "image/jpeg";
    contentTypes[".gif"] = "image/gif";
    contentTypes[".php"] = "text/html; charset=UTF-8";
	contentTypes[".py"] = "text/html; charset=UTF-8";
	contentTypes[".sh"] = "text/html; charset=UTF-8";

    std::size_t lastIndex = filePath.find_last_of(".");
    if (lastIndex != std::string::npos) {
        std::string extension = filePath.substr(lastIndex);
        if (contentTypes.count(extension) > 0) {
            return contentTypes[extension];
        }
    }
    return "text/plain";
}