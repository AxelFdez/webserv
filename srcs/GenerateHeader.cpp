#include "../includes/GenerateHeader.hpp"

GenerateHeader::GenerateHeader(std::string path, int code) : _path(path), _code(code)
{
	if (_code >= 400)
		errorHeaderFormat();
	else
		headerFormat();
}

GenerateHeader::~GenerateHeader() {}

void GenerateHeader::headerFormat()
{
	struct stat	fileStat;
	stat(_path.c_str(), &fileStat);
	char lastModifDate[80];
    strftime(lastModifDate, sizeof(lastModifDate), "%a, %d %b %Y %H:%M:%S GMT", localtime(&fileStat.st_mtime));
	std::time_t now = std::time(NULL);
	char creationDate[80];
    strftime(creationDate, sizeof(creationDate), "%a, %d %b %Y %H:%M:%S GMT", localtime(&now));

	_header = "HTTP/1.1 " + std::to_string(_code) + "\n" \
	+ "Server: webserv/chmassa-axfernan\n" \
	+ "Date: " + creationDate + "\n" \
	+ "Content-Type: " + getContentType(_path) + "\n" \
	+ "Content-Length: " + std::to_string(fileStat.st_size) +"\n" \
	+ "Last-Modified: " + lastModifDate + "\n";
}

void GenerateHeader::errorHeaderFormat()
{
	std::time_t now = std::time(NULL);
	char creationDate[80];
    strftime(creationDate, sizeof(creationDate), "%a, %d %b %Y %H:%M:%S GMT", localtime(&now));

	_header = "HTTP/1.1 " + std::to_string(_code) + "\n" \
	+ "Server: webserv/chmassa-axfernan\n" \
	+ "Date: " + creationDate + "\n" \
	+ "Content-Type: text/html\n"\
	+ "Content-Length: " + std::to_string(generateErrorPage(_code).size()) + "\n" \
	+ "Connection: close\n";
}


const std::string &GenerateHeader::getHeader()
{
	return _header;
}

std::string getContentType(const std::string& filePath) {
    std::map<std::string, std::string> contentTypes;
    contentTypes[".html"] = "text/html";
    contentTypes[".txt"] = "text/plain";
    contentTypes[".css"] = "text/css";
    contentTypes[".js"] = "application/javascript";
    contentTypes[".json"] = "application/json";
    contentTypes[".xml"] = "application/xml";
    contentTypes[".png"] = "image/png";
    contentTypes[".jpg"] = "image/jpeg";
    contentTypes[".jpeg"] = "image/jpeg";
    contentTypes[".gif"] = "image/gif";
    contentTypes[".php"] = "text/html";

    // Trouver la dernière occurrence du point pour obtenir l'extension
    std::size_t lastIndex = filePath.find_last_of(".");
    if (lastIndex != std::string::npos) {
        std::string extension = filePath.substr(lastIndex);
        if (contentTypes.count(extension) > 0) {
            return contentTypes[extension];
        }
    }
    return "application/octet-stream";
}