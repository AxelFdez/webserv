#include "../includes/GenerateBody.hpp"

GenerateBody::GenerateBody(std::string method, std::string _pathData) : \
	_method(method), _uri(_pathData) {

		handleMethod();
}

GenerateBody::~GenerateBody() {}

void GenerateBody::handleMethod()
{
	if (_method == "GET")
		GetMethod();
	// else if (_method == "POST")
	// 	PostMethod();
	// else if (_method == "DELETE")
	// 	DeleteMethod();
}

const std::string &GenerateBody::getBody() const
{
	return _response;
}

const int			&GenerateBody::getCode() const
{
	return _code;
}

const std::string	&GenerateBody::getPath() const
{
	return _path;
}

void	GenerateBody::GetMethod()
{
	//std::cout << "_path before = " << _uri << std::endl;
	_path = _uri.substr(0, _uri.find('?', 0));
	//std::cout << "_path after = " << _path << std::endl;
	if (_path == "/")
		_path = "/index.html";
	_path = "/Users/axelfernandez/ecole42/cursus42/webserv/srcs/tools" + _path;
	//std::cout << "full _path = " << _path << std::endl;

	// int test_Path = open(_path.c_str(), O_RDONLY);
	// if (test_Path == -1)
	// {
	// 	_response = getHtmlPage("../tools/403.html");
	// }
	if (access(_path.c_str(), F_OK) == -1)
	{
		_code = 404;
		_response = generateErrorPage(_code);
	}
	else if (access(_path.c_str(), R_OK) == -1 )
	{
		_code = 403;
		_response = generateErrorPage(_code);
	}
	else
	{
		// stat(_path.c_str(), &_fileStat);
		_response = getHtmlPage(_path);
		_code = 200;
	}

}