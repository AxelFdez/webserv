#include "../includes/ClientRequest.hpp"

volatile sig_atomic_t signalReceived = 0;

void signalHandler(int signal) {
    signalReceived = 1;
}

ClientRequest::ClientRequest(std::vector<std::vector<int> > serverSockets, HandleConfigFile config) : _config(config)
{
	_totalServerSockets = 0;
	for (size_t i = 0; i < serverSockets.size(); i++)
	{
		int nbSockets = 0;
		for (size_t j = 0; j < serverSockets[i].size(); j++)
		{
			pollfd pfd;
			pfd.fd = serverSockets[i][j];
			pfd.events = POLLIN;
			_pollSockets.push_back(pfd);
			nbSockets = j;
			_totalServerSockets++;
		}
		_socketsByServer.push_back(nbSockets);
	}
}

ClientRequest::~ClientRequest()
{
	for (size_t i = 0; i < _pollSockets.size(); i++)
	{
		close(_pollSockets[i].fd);
	}
	_pollSockets.clear();
	_clients.clear();
}

void ClientRequest::manageRequest()
{
	std::cout << "Server listenning..." << std::endl;
	signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
	while (!signalReceived)
	{
		pollFunc();
		listenning();
		acceptNewClient();
		readRequest();
		sendResponse();
	}
}

void ClientRequest::pollFunc()
{
	int ret = poll(_pollSockets.data(), _pollSockets.size(), 5000);
	if (ret < 0)
	{
		perror("poll");
		return;
	}
	else if (ret == 0)
	{
		for (size_t i = _totalServerSockets; i < _pollSockets.size(); i++)
		{
			close(_pollSockets[i].fd);
		}
		_pollSockets.erase(_pollSockets.begin() + _totalServerSockets, _pollSockets.end());
		_clients.clear();
	}
}

void ClientRequest::listenning()
{
	for (int i = 0; i < _totalServerSockets; i++)
	{
		if (listen(_pollSockets[i].fd, 50) < 0)
		{
			std::cout << "error : listen failed" << std::endl;
			throw std::exception();
		}
	}
}

void ClientRequest::acceptNewClient()
{
	struct	sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int newClient;
	int ref = -1;
	int server = 0;
	for (int i = 0; i < _totalServerSockets; i++)
	{
		if (ref == _socketsByServer[server])
		{
			ref = -1;
			server++;
		}
		ref++;
		if (_pollSockets[i].revents & POLLIN)
		{
			newClient = accept(_pollSockets[i].fd, (struct sockaddr *)&clientAddr, &clientLen);
		}
		else
			continue;
		if (newClient < 0)
		{
			perror("Could not accept this client");
			return;
		}
		pollfd pfd;
		pfd.fd = newClient;
		pfd.events = POLLIN | POLLOUT;
		_pollSockets.push_back(pfd);
		fcntl(pfd.fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
		_clients[pfd.fd];
		_clients[pfd.fd].setBelongOgServer(server);
		_clients[pfd.fd].setBodySize(0);
		_clients[pfd.fd].setReady(false);
		char clientIP[INET_ADDRSTRLEN];
    	inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
		std::string ip(clientIP);
		_clients[pfd.fd].setClientIP( ip );
	}
}

bool ClientRequest::doubleLineEndingFound(std::vector<char> request)
{
	if (request.size() >= 4)
	{
		std::string tmp = vectorCharToString( request );
		// if (tmp.find("\r\n\r\n", tmp.size() - 5, 4) != std::string::npos)
		if (tmp.find("\r\n\r\n") != std::string::npos)
			return true;
	}
	return false;
}

void ClientRequest::readRequest()
{
	for (size_t i = _totalServerSockets; i < _pollSockets.size(); i++)
	{
		//std::cout << _pollSockets[i].revents << std::endl;
		if (_pollSockets[i].revents & POLLIN \
			&& _clients[_pollSockets[i].fd].getBodySize() <= _config.getBodySizeMax(_clients[_pollSockets[i].fd].getBelongOfServer()))
		{
			int bufferSize = 65536;
			char request[bufferSize];
			ssize_t bytes = recv(_pollSockets[i].fd, request, bufferSize - 1, 0);
			// std::cout << "request = " << request << std::endl;
			if (bytes <= 0)
			{
				perror("Error data reception");
				close(_pollSockets[i].fd);
				_clients.erase(_pollSockets[i].fd);
				_pollSockets.erase(_pollSockets.begin() + i);
				continue;
			}
			else
			{
				std::vector<char> requestBinary(request, request + bytes);

				if (_clients[_pollSockets[i].fd].getRequest().empty())
				{
					_clients[_pollSockets[i].fd].setRequest(requestBinary);
					std::string tmp = vectorCharToString( _clients[_pollSockets[i].fd].getRequest() );
					if (tmp.find("\r\n\r\n") != std::string::npos)
					{
						tmp = tmp.substr(tmp.find("\r\n\r\n") + 3);
						_clients[_pollSockets[i].fd].setBodySize(tmp.size());
					}
					if (doubleLineEndingFound(_clients[_pollSockets[i].fd].getRequest()))
						_clients[_pollSockets[i].fd].setReady(true);
				}
				else
				{
					std::vector<char> tmp = _clients[_pollSockets[i].fd].getRequest();
					std::vector<char> truncRequest;
					truncRequest = tmp;
					truncRequest.insert(truncRequest.end(), requestBinary.begin(), requestBinary.end());
					if (!_clients[_pollSockets[i].fd].getBodySize())
					{
						std::string tmp = vectorCharToString( truncRequest );
						if (tmp.find("\r\n\r\n") != std::string::npos)
						{
							tmp = tmp.substr(tmp.find("\r\n\r\n") + 3);
							_clients[_pollSockets[i].fd].setBodySize(tmp.size());
						}
					}
					else
						_clients[_pollSockets[i].fd].setBodySize(_clients[_pollSockets[i].fd].getBodySize() + bytes);
						// std::cout << _clients[_pollSockets[i].fd].getBodySize() << std::endl;
					_clients[_pollSockets[i].fd].setRequest(truncRequest);
					if (doubleLineEndingFound(_clients[_pollSockets[i].fd].getRequest()) && !_clients[_pollSockets[i].fd].getReady())
						_clients[_pollSockets[i].fd].setReady(true);
					//std::cout << std::boolalpha << _clients[_pollSockets[i].fd].getReady() << std::endl;
					//std::cout << "bodySize = " << _clients[_pollSockets[i].fd].getBodySize() << std::endl;
					//std::cout << "bodySizeMax = " << _config.getBodySizeMax(_clients[_pollSockets[i].fd].getBelongOfServer()) << std::endl;
				}
			}
		}
	}
}

void ClientRequest::sendResponse()
{
	for (size_t i = _totalServerSockets; i < _pollSockets.size(); i++)
	{
		if (((_pollSockets[i].revents & POLLOUT) && !(_pollSockets[i].revents & POLLIN) && !_clients[_pollSockets[i].fd].getRequest().empty()) && _clients[_pollSockets[i].fd].getReady() \
			|| (_pollSockets[i].revents & POLLOUT) && _clients[_pollSockets[i].fd].getBodySize() > _config.getBodySizeMax(_clients[_pollSockets[i].fd].getBelongOfServer()))
		{
			//std::cout <<  std::boolalpha << _clients[_pollSockets[i].fd].getReady() << std::endl;
			if (_clients[_pollSockets[i].fd].getResponse().empty())
			{
				_clients[_pollSockets[i].fd].setBytesSent(0);
				MakeResponse response(_clients[_pollSockets[i].fd].getRequest(), _clients[_pollSockets[i].fd].getBelongOfServer(), _config);
			//	displayRequest(response.getResponse(), 1);
				_clients[_pollSockets[i].fd].setResponse(response.getResponse());
				response.access_logs(_clients[_pollSockets[i].fd].getClientIP());
			}
			std::string partialResponse = _clients[_pollSockets[i].fd].getResponse().substr(_clients[_pollSockets[i].fd].getBytesSent(), 1024);
			//std::cout << "partialResponse = " << partialResponse << std::endl;
			size_t bytes = send(_pollSockets[i].fd, partialResponse.c_str(), partialResponse.size(), 0);
			if (bytes < 0)
			{
				perror("Error data sending");
				close(_pollSockets[i].fd);
				_clients.erase(_pollSockets[i].fd);
				_pollSockets.erase(_pollSockets.begin() + i);
				i--;
				continue;;
			}
			else
			{
				_clients[_pollSockets[i].fd].setBytesSent(_clients[_pollSockets[i].fd].getBytesSent() + bytes);
				if (_clients[_pollSockets[i].fd].getBytesSent() >= _clients[_pollSockets[i].fd].getResponse().size())
				{
					close(_pollSockets[i].fd);
					_clients.erase(_pollSockets[i].fd);
					_pollSockets.erase(_pollSockets.begin() + i);
					i--;
				}
			}
		}
	}
}


std::string ClientRequest::vectorCharToString( std::vector<char> vec ) {

    std::string str;
    if ( vec.size() > 0 ) {

        for ( size_t i = 0; i < vec.size(); i++ ) {
            str.push_back( vec[i] );
        }
    }
    return str;
}

void displayRequest(const std::string  &request, int modifier)
{
	if (!modifier)
		std::cout << "\033[0;41m-----REQUEST------" << std::endl;
	else
		std::cout << "\033[0;42m-----RESPONSE-----" << std::endl;
	std::cout << request << std::endl;
	if (!modifier)
		std::cout <<  "\033[0;41m-----REQUEST------\033[0m" << std::endl;
	else
		std::cout << "\033[0;42m-----RESPONSE-----\033[0m" << std::endl;
}
