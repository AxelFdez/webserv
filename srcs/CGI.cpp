#include "../includes/CGI.hpp"

CGI::CGI(std::string path, std::string uri, std::string method, std::map<std::string, std::string> request, std::string lineEnding, std::string extension, HandleConfigFile &config)
// CGI::CGI(std::string path, std::string uri, std::string method, std::map<std::string, std::string> request, std::string lineEnding, std::string extension)
{
	_path = path;
	_uri = uri;
	_method = method;
	_request = request;
	_requestBody = request["Body"];
	//std:: cerr << "requestBody: " << _requestBody << std::endl;
	_lineEnding = lineEnding;
	_extension = extension;
	_cgi_path = config.getCGI_PATH();
	executeCGI();
}

CGI::~CGI() {}

void	CGI::executeCGI()
{

	int stdinPipe[2];
	int stdoutPipe[2];
	pipe(stdinPipe);
	pipe(stdoutPipe);
	pid_t child = fork();
	if (child < 0)
	{
		_errorCode = 500;
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
		if (_extension == ".php")
			cmd[0] = const_cast<char *>(_cgi_path.c_str());
			// cmd[0] = const_cast<char *>("/Users/chris/.brew/bin/php-cgi");
			//cmd[0] = const_cast<char *>("/opt/homebrew/bin/php-cgi");
		else if (_extension == ".py" || _extension == ".sh")
			cmd[0] = const_cast<char *>(_path.c_str());
		cmd[1] = NULL;
		std::vector<char*> env;
		std::vector<std::string> tmp = envCGI();

		for (size_t i = 0; i < tmp.size(); i++)
			env.push_back(const_cast<char*>(tmp[i].c_str()));
		env.push_back(NULL);
		//for ( size_t i = 0; env[i]; i++ ) {
		//	std::cerr << env[i] << std::endl;
		//}
		execve(cmd[0], cmd, env.data());
		perror("execve");
	}
	write(stdinPipe[1], _requestBody.c_str(), _requestBody.length());
	close(stdinPipe[1]);

	close(stdinPipe[0]);
	close(stdoutPipe[1]);
	int ret = 0;
	sleep(2);
	kill(child, SIGTERM);
	waitpid(child, &ret, 0);
	if (!fillResponseFrom(stdoutPipe[0]) && ret != 0)
	{
		_errorCode = 500;
		close(stdoutPipe[0]);
		return;
	}
	close(stdoutPipe[0]);
	size_t pos = _responseBody.find(_lineEnding + _lineEnding);
	if (pos != std::string::npos)
	{
		_cgiHeader.assign(_responseBody.begin(), _responseBody.begin() + pos);
		_responseBody = _responseBody.assign(_responseBody.begin() + pos + 4, _responseBody.end());
	}
	else
	{
		_errorCode = 500;
		return;
	}
	size_t status = _cgiHeader.find("Status: ");
	if (status != std::string::npos)
	{
		_errorCode =  atoi(_cgiHeader.substr(status + 8, 3).c_str());
	}
	else
		_errorCode = 200;
}

bool	CGI::fillResponseFrom(int stdoutPipe)
{
	const size_t bufferSize = 256;
   	std::vector<char> buffer(bufferSize);
	ssize_t bytesRead;
    while ((bytesRead = read(stdoutPipe, buffer.data(), bufferSize)) > 0)
        _responseBody.append(buffer.begin(), buffer.begin() + bytesRead);
    if (bytesRead < 0)
	{
        perror("read pipe error");
		return (false);
	}
	return (true);
}

std::string	CGI::getDirectory()
{
	size_t end = _path.find_last_of("/");
	if (end == std::string::npos || end == _path.size())
		return (_path);
	else
		return(_path.substr(0, end));
}

std::vector<std::string> CGI::envCGI()
{
	std::vector<std::string> env;
	env.push_back("REDIRECT_STATUS=CGI");
	env.push_back("REQUEST_METHOD=" + _method);
	if (_method == "POST")
	{
    	env.push_back("CONTENT_TYPE=" + _request["Content-Type"].substr(0, _request["Content-Type"].size() - 1));
    	env.push_back("CONTENT_LENGTH=" + std::to_string(_request["Body"].length() - 1));
    }
	env.push_back(("SCRIPT_FILENAME=") + _uri.substr(1, (_uri.find('?', 0) - 1)));
	if (_uri.find('?', 0) != std::string::npos && _method == "GET")
	{
		env.push_back(("QUERY_STRING=") + _uri.substr(_uri.find('?', 0) + 1, _uri.size() - _uri.find('?', 0)));
	}
	if (!_request["Cookie"].empty())
	{
		env.push_back(("HTTP_COOKIE=") + _request["Cookie"]);
	}
	//for (int i = 0; i < env.size(); i++)
	//	std::cerr << "env[" << i << "]: " << env[i] << std::endl;
	return env;
}

int	CGI::getErrorCode() const
{
	return _errorCode;
}

const std::string	&CGI::getResponseBody() const
{
	return _responseBody;
}

const std::string	&CGI::getCgiHeader() const
{
	return _cgiHeader;
}
