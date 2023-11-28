#include "../includes/Server.hpp"

Server::Server(const std::string &configFile, std::string cgi_path ) : _config(const_cast<char *>(configFile.c_str()), cgi_path )
{
	launchServer();
	handleClients();

}

Server::~Server()
{
	for ( size_t i = 0; i < _socket.size(); i++ )
	{
		for ( size_t j = 0; j < _socket[i].size(); j++ )
		{
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
    		    perror("setsockopt error");
    		    close(_socket[i][j]);
    		    throw std::exception();
    		}

    		struct timeval timeout;
    		timeout.tv_sec = 10;
    		timeout.tv_usec = 0;

    		if (setsockopt(_socket[i][j], SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
    		    perror("timeout configuration error");
    		    close(_socket[i][j]);
    		    throw std::exception();
    		}
		}
	}
}

void Server::linkAddPort()
{
	std::vector<std::vector<int> > ports = _config.getPorts();
	for ( size_t i = 0; i < ports.size(); i++ ) {

		for ( size_t j = 0; j < ports[i].size(); j++ ) {
			_address.sin_family = AF_INET;
			_address.sin_addr.s_addr = htonl(convIpToLong( _config.getServerValues(i, "host").c_str())); //_config.address
			_address.sin_port = htons(ports[i][j]);

			std::cout << "link to : " + _config.getServerValues(i, "host") + "\ndomain(" + _config.getServerValues(i, "server_name") + ")\nat port : " << ports[i][j] << std::endl;
			if (bind(_socket[i][j], (struct sockaddr *)&_address, sizeof(_address)) < 0)
			{
				std::cout << "error : bind failed" << std::endl;
				continue;
			}
			std::cout << "Bind success." << std::endl;
		}
	}
}

void Server::launchServer()
{
	createSocket();
	linkAddPort();
}

void Server::handleClients()
{
	ClientRequest clientRequest(_socket, _config);
	clientRequest.manageRequest();
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