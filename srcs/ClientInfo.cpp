
#include "../includes/ClientInfo.hpp"

void		ClientInfo::setRequest( std::vector<char> request ) {
	_request = request;
}

void		ClientInfo::setBelongOgServer( int ref ) {
	_belongOfServer = ref;
}

void ClientInfo::setClientIP( std::string IP ) {
	_clientIP = IP;
}

std::vector<char>	ClientInfo::getRequest() {
	return _request;
}

int	ClientInfo::getBelongOfServer() {
	return _belongOfServer;
}

std::string ClientInfo::getClientIP() {
	return _clientIP;
}

void    ClientInfo::setBodySize( int value ) {
	_bodySize = value;
}

int ClientInfo::getBodySize() {
	return _bodySize;
}

void    ClientInfo::setResponse( std::string response ) {
	_response = response;
}

std::string ClientInfo::getResponse() {
	return _response;
}

void    ClientInfo::setBytesSent( int bytes ) {
	_bytesSent = bytes;
}

int ClientInfo::getBytesSent() {
	return _bytesSent;
}

void    ClientInfo::setReady( bool value ) {
	_ready = value;
}

bool    ClientInfo::getReady() {
	return _ready;
}