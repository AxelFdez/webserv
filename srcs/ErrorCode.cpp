#include "../includes/ErrorCode.hpp"

ErrorCode::ErrorCode()
{
	_error[200] = "OK";
	_error[201] = "Created";
	_error[202] = "Accepted";
	_error[204] = "No Content";
	_error[301] = "Moved Permanently";
	_error[302] = "Found";
	_error[303] = "See Other";
	_error[304] = "Not Modified";
	_error[307] = "Temporary Redirect";
	_error[308] = "Permanent Redirect";
	_error[400] = "Bad Request";
	_error[401] = "Unauthorized";
	_error[403] = "Forbidden";
	_error[404] = "Not Found";
	_error[405] = "Method Not Allowed";
	_error[406] = "Not Acceptable";
	_error[408] = "Request Timeout";
	_error[409] = "Conflict";
	_error[410] = "Gone";
	_error[411] = "Length Required";
	_error[412] = "Precondition Failed";
	_error[413] = "Payload Too Large";
	_error[414] = "URI Too Long";
	_error[415] = "Unsupported Media Type";
	_error[429] = "Too Many Requests";
	_error[500] = "Internal Server Error";
	_error[501] = "Not Implemented";
	_error[502] = "Bad Gateway";
	_error[503] = "Service Unavailable";
	_error[504] = "Gateway Timeout";
	_error[505] = "HTTP Version Not Supported";
}

ErrorCode::~ErrorCode() {}

const std::string &ErrorCode::getMessage(int code)
{
	return _error[code];
}