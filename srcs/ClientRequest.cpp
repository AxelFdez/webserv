#include "../includes/ClientRequest.hpp"

ClientRequest::ClientRequest(std::vector<std::vector<int> > serverSockets)
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
	while (1)
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
	for (size_t i = 0; i < _totalServerSockets; i++)
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
	for (size_t i = 0; i < _totalServerSockets; i++)
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
		_clients[pfd.fd];
		_clients[pfd.fd].setBelongOgServer(server);
	}
}

void ClientRequest::readRequest()
{

	for (size_t i = _totalServerSockets; i < _pollSockets.size(); i++)
	{
		if (_pollSockets[i].revents & POLLIN)
		{
			char request[1024];
			ssize_t bytes = recv(_pollSockets[i].fd, request, sizeof(request), 0);
			if (bytes < 0)
				perror("Error data reception");
			else if (bytes == 0)
				perror("Client closed the connection");
			else
			{
				request[bytes] = '\0';
				if (_clients[_pollSockets[i].fd].getRequest().empty())
					_clients[_pollSockets[i].fd].setRequest(request);
				else
				{
					std::string tmp = _clients[_pollSockets[i].fd].getRequest();
					_clients[_pollSockets[i].fd].setRequest(tmp + request);
				}
				continue;
			}
			close(_pollSockets[i].fd);
			_clients.count(_pollSockets[i].fd);
			_pollSockets.erase(_pollSockets.begin() + i);
		}
	}
}

void ClientRequest::sendResponse()
{
	for (size_t i = _totalServerSockets; i < _pollSockets.size(); i++)
	{
		if ((_pollSockets[i].revents & POLLOUT) && !(_pollSockets[i].revents & POLLIN) && !_clients[_pollSockets[i].fd].getRequest().empty())
		{
			displayRequest(_clients[_pollSockets[i].fd].getRequest(), 0);
			MakeResponse response(_clients[_pollSockets[i].fd].getRequest(), _clients[_pollSockets[i].fd].getBelongOfServer());
			//MakeResponse response(_clients[_pollSockets[i].fd].getRequest());
			displayRequest(response.getResponse(), 1);
			fcntl(_pollSockets[i].fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
			ssize_t bytesSent = 0;
			while (bytesSent < response.getResponse().size())
			{
				std::string partialResponse= response.getResponse().substr(bytesSent);
				ssize_t bytes = send(_pollSockets[i].fd, partialResponse.c_str(), partialResponse.size(), 0);
				if (bytes < 0)
				{
					perror("send error");
					break;
				}
				else if (bytes == 0)
				{
					perror("client closed the connection");
					break;
				}
				bytesSent += bytes;
			}
			close(_pollSockets[i].fd);
			_clients.erase(_pollSockets[i].fd);
			_pollSockets.erase(_pollSockets.begin() + i);
			i--;
		}
	}
}

void ClientRequest::displayRequest(const std::string  &request, int modifier) const
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