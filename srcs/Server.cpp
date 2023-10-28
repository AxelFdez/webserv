#include "../includes/Server.hpp"


Server::Server(const std::string &configFile) : _config(configFile)
{
	launchServer();
	handleClients();
}

Server::~Server()
{
	close(_socket);
}

void Server::createSocket()
{
	std::cout << "Create socket..." << std::endl;
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (!_socket)
	{
		std::cout << "error : socket not create" << std::endl;
		throw std::exception();
	}
	std::cout << "Socket created." << std::endl;
}

void Server::linkAddPort()
{
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = (INADDR_ANY); //_config.address
	_address.sin_port = htons(8080); //_config.port

	std::cout << "link to address and port(s)..." << std::endl;
	if (bind(_socket, (struct sockaddr *)&_address, sizeof(_address)) < 0)
	{
		std::cout << "error : bind failed" << std::endl;
		throw std::exception();
	}
	std::cout << "Bind success." << std::endl;
}

// void Server::listenning()
// {
// 	if (listen(_socket, 10) < 0)
// 	{
// 		std::cout << "error : listen failed" << std::endl;
// 		throw std::exception();
// 	}
// 	std::cout << "Server listenning..." << std::endl;
// }

void Server::launchServer()
{
	createSocket();
	linkAddPort();
	// listenning();
}

void Server::handleClients()
{
	ClientRequest clientRequest(_socket);
	clientRequest.manageRequest();
}