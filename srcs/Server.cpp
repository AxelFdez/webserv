#include "../includes/Server.hpp"


// Server::Server(const std::string &configFile) : _config(const_cast<char *>(configFile.c_str()))
// {
// 	launchServer();
// 	handleClients();
// }

// Server::~Server()
// {
// 	close(_socket);
// }

// void Server::createSocket()
// {
// 	std::cout << "Create socket..." << std::endl;
// 	_socket = socket(AF_INET, SOCK_STREAM, 0);
// 	if (!_socket)
// 	{
// 		std::cout << "error : socket not create" << std::endl;
// 		throw std::exception();
// 	}
// 	std::cout << "Socket created." << std::endl;
// 	int optval = 1;
//     if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
//         perror("Erreur lors de la configuration de setsockopt");
//         close(_socket);
//         exit(EXIT_FAILURE);
//     }

//     // Définition du délai d'attente (timeout)
//     struct timeval timeout;
//     timeout.tv_sec = 10;  // 10 secondes
//     timeout.tv_usec = 0;

//     if (setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
//         perror("Erreur lors de la configuration du délai d'attente");
//         close(_socket);
//         exit(EXIT_FAILURE);
//     }
// }

// void Server::linkAddPort()
// {
//     std::cout << "addr = " << _config.getServerValues(0, "host").c_str() << std::endl;
// 	_address.sin_family = AF_INET;
// 	_address.sin_addr.s_addr = inet_addr(_config.getServerValues(0, "host").c_str()); //_config.address
// 	_address.sin_port = htons(8080); //_config.port

// 	std::cout << "link to address and port(s)..." << std::endl;
// 	if (bind(_socket, (struct sockaddr *)&_address, sizeof(_address)) < 0)
// 	{
// 		std::cout << "error : bind failed" << std::endl;
// 		throw std::exception();
// 	}
// 	std::cout << "Bind success." << std::endl;
// }

// void Server::launchServer()
// {
// 	createSocket();
// 	linkAddPort();
// 	// listenning();
// }

// void Server::handleClients()
// {
// 	ClientRequest clientRequest(_socket);
// 	clientRequest.manageRequest();
// }

Server::Server(const std::string &configFile) : _config(const_cast<char *>(configFile.c_str()))
{
	launchServer();
	handleClients();
}

Server::~Server()
{
	for ( size_t i = 0; i < _socket.size(); i++ ) {

		for ( size_t j = 0; j < _socket[i].size(); j++ ) {

			close( _socket[i][j] );
		}
	}
}

void Server::createSocket()
{

	std::vector<std::vector<int> > ports = _config.getPorts();

	for ( size_t i = 0; i < ports.size(); i++ ) {

		_socket.push_back( std::vector<int>() );
		for ( size_t j = 0; j < ports[i].size(); j++ ) {

			std::cout << "Create socket..." << std::endl;
			_socket[i].push_back( socket(AF_INET, SOCK_STREAM, 0) );
			if (!_socket[i][j])
			{
				std::cout << "error : socket not create" << std::endl;
				throw std::exception();
			}
			std::cout << "Socket created." << std::endl;
			int optval = 1;
    		if (setsockopt(_socket[i][j], SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
    		    perror("Erreur lors de la configuration de setsockopt");
    		    close(_socket[i][j]);
    		    exit(EXIT_FAILURE);
    		}

    		// Définition du délai d'attente (timeout)
    		struct timeval timeout;
    		timeout.tv_sec = 10;  // 10 secondes
    		timeout.tv_usec = 0;

    		if (setsockopt(_socket[i][j], SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
    		    perror("Erreur lors de la configuration du délai d'attente");
    		    close(_socket[i][j]);
    		    exit(EXIT_FAILURE);
    		}
		}
	}


	for ( size_t i = 0; i < _socket.size(); i++ ) {

		for ( size_t j = 0; j < _socket[i].size(); j++ ) {

			std::cout << "i = " << i << " j = " << j << " " << _socket[i][j] << std::endl;

		}

	}
}

void Server::linkAddPort()
{
	std::vector<std::vector<int> > ports = _config.getPorts();
	for ( size_t i = 0; i < ports.size(); i++ ) {

		for ( size_t j = 0; j < ports[i].size(); j++ ) {

    		std::cout << "addr = " << _config.getServerValues(i, "host").c_str() << std::endl;
			_address.sin_family = AF_INET;
			_address.sin_addr.s_addr = htonl(convIpToLong( _config.getServerValues(i, "host").c_str())); //_config.address
			// _address.sin_addr.s_addr = inet_addr(_config.getServerValues(i, "host").c_str()); //_config.address
			_address.sin_port = htons(ports[i][j]); //_config.port

			std::cout << "link to address and port(s)..." << std::endl;
			if (bind(_socket[i][j], (struct sockaddr *)&_address, sizeof(_address)) < 0)
			{
				std::cout << "error : bind failed" << std::endl;
				perror("");
				throw std::exception();
			}
			std::cout << "Bind success." << std::endl;
		}
	}
}

void Server::launchServer()
{
	createSocket();
	linkAddPort();
	// exit(0);
	// listenning();
}

void Server::handleClients()
{
	// ClientRequest clientRequest(_socket);
	// clientRequest.manageRequest();
}

in_addr_t Server::convIpToLong(const std::string& ipAddress) {

    std::istringstream iss(ipAddress);
    std::string octet;
    in_addr_t result = 0;

    for (int i = 0; i < 4; ++i) {
        if (!getline(iss, octet, '.') || !isdigit(octet[0])) {
            throw std::invalid_argument("Adresse IP non valide");
        }
        int octetValue = std::stoi(octet);
        if (octetValue < 0 || octetValue > 255) {
            throw std::invalid_argument("Adresse IP non valide");
        }
        result = (result << 8) | octetValue;
    }
    return result;
}