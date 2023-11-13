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
	_path = "/Users/axelfernandez/ecole42/cursus42/webserv/srcs/tools" + _path; // recuperer le path du fichier de config
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
	if (extension == ".php")
	{
		if (_method != "GET" && _method != "POST")
		{
			_errorCode = 405;
			_responseBody = generateErrorPage(405);
			return;
		}
		_errorCode = 200;
		executeCGI();
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


void	GenerateBody::executeCGI()
{
	int stdinPipe[2];
	int stdoutPipe[2];
	pipe(stdinPipe);
	pipe(stdoutPipe);
	pid_t child = fork();
	if (child < 0)
	{
		_errorCode = 500;
		_responseBody = generateErrorPage(500);
		perror("fork error");
		return;
	}
	else if (child == 0)
	{
		dup2(stdinPipe[0], STDIN_FILENO);
		close(stdinPipe[0]);
		close(stdinPipe[1]);
		dup2(stdoutPipe[1], STDOUT_FILENO);
		close(stdoutPipe[1]);
		close(stdoutPipe[0]);
		chdir(getDirectory().c_str());
		char *cmd[2];
		cmd[0] = const_cast<char *>("/opt/homebrew/bin/php-cgi");
		cmd[1] = NULL;
		std::vector<char*> env;
		std::vector<std::string> tmp = envCGI(_uri, _method, _request);
		for (size_t i = 0; i < tmp.size(); i++)
			env.push_back(const_cast<char*>(tmp[i].c_str()));
		env.push_back(NULL);
		execve(cmd[0], cmd, env.data());
		perror("execve");
	}
	write(stdinPipe[1], _requestBody.c_str(), _requestBody.length());
	close(stdinPipe[1]);
	close(stdinPipe[0]);
	close(stdoutPipe[1]);
	waitpid(child, NULL, 0);
	if (!fillResponseFrom(stdoutPipe[0]))
	{
		_errorCode = 500;
		_responseBody = generateErrorPage(500);
		return;
	}
	close(stdoutPipe[0]);
	size_t pos = _responseBody.find(_lineEnding + _lineEnding);
	if (pos != std::string::npos)
	{
		_cgiHeader.assign(_responseBody.begin(), _responseBody.begin() + pos);
		_responseBody = _responseBody.assign(_responseBody.begin() + pos + 4, _responseBody.end());
	}
	size_t status = _cgiHeader.find("Status: ");
	if (status != std::string::npos)
	{
		_errorCode =  atoi(_cgiHeader.substr(status + 8, 3).c_str());
	}
	// std::cout << "headr = \n" << _cgiHeader << std::endl;
	// std::cout << "body = \n" << _body << std::endl;
}

bool	GenerateBody::fillResponseFrom(int stdoutPipe)
{
	const size_t bufferSize = 256;
   	std::vector<char> buffer(bufferSize);
	ssize_t bytesRead;
    while ((bytesRead = read(stdoutPipe, buffer.data(), bufferSize)) > 0)
        _responseBody.append(buffer.begin(), buffer.begin() + bytesRead);
    if (bytesRead < 0)
	{
        perror("Erreur de lecture du pipe");
		return (false);
	}
	return (true);
}

std::string	GenerateBody::getDirectory()
{
	size_t end = _path.find_last_of("/");
	if (end == std::string::npos || end == _path.size())
		return (_path);
	else
		return(_path.substr(0, end));
}

std::vector<std::string> envCGI(std::string uri, std::string method, std::map<std::string, std::string> request)
{
	std::vector<std::string> env;
		env.push_back("REDIRECT_STATUS=CGI");
		env.push_back("REQUEST_METHOD=" + method);
		if (method == "POST")
		{
        	env.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
        	env.push_back("CONTENT_LENGTH=" + std::to_string(request["Body"].length()));
    	}
		env.push_back(("SCRIPT_FILENAME=") + uri.substr(1, (uri.find('?', 0) - 1)));
		if (uri.find('?', 0) != std::string::npos && method == "GET")
		{
			env.push_back(("QUERY_STRING=") + uri.substr(uri.find('?', 0) + 1, uri.size() - uri.find('?', 0)));
		}
		if (!request["Cookie"].empty())
		{
			env.push_back(("HTTP_COOKIE=") + request["Cookie"]);
		}
	return env;
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