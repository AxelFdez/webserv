#include "../includes/ClientRequest.hpp"

ClientRequest::ClientRequest(int serverSocket)
{
	pollfd pfd;
	pfd.fd = serverSocket;
	pfd.events = POLLIN;
	_sockets.push_back(pfd);
}

ClientRequest::~ClientRequest()
{
	close(_sockets[0].fd);
	_sockets.clear();
	_client.clear();
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
	int ret = poll(_sockets.data(), _sockets.size(), 5000);
	if (ret < 0)
	{
		perror("poll");
		return;
	}
	else if (ret == 0)
	{
		for (size_t i = 1; i < _sockets.size(); i++)
		{
			close(_sockets[i].fd);
		}
		_sockets.erase(_sockets.begin() + 1, _sockets.end());
		_client.clear();
	}
}

void ClientRequest::listenning()
{
	if (listen(_sockets[0].fd, 50) < 0)
	{
		std::cout << "error : listen failed" << std::endl;
		throw std::exception();
	}
}


void ClientRequest::acceptNewClient()
{
	struct	sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int newClient;
	if (_sockets[0].revents & POLLIN)
	{
		newClient = accept(_sockets[0].fd, (struct sockaddr *)&clientAddr, &clientLen);
	}
	else
		return;
	if (newClient < 0)
	{
		perror("Could not accept this client");
		return;
	}
	pollfd pfd;
	pfd.fd = newClient;
	pfd.events = POLLIN | POLLOUT;
	_sockets.push_back(pfd);
	_client[pfd.fd];
}

void ClientRequest::readRequest()
{

	for (size_t i = 1; i < _sockets.size(); i++)
	{
		if (_sockets[i].revents & POLLIN)
		{
			char request[1024];
			ssize_t bytes = recv(_sockets[i].fd, request, sizeof(request), 0);
			if (bytes < 0)
				perror("Error data reception");
			else if (bytes == 0)
				perror("Client closed the connection");
			else
			{
				request[bytes] = '\0';
				_client[_sockets[i].fd].append(request);
				continue;
			}
			close(_sockets[i].fd);
			_client.count(_sockets[i].fd);
			_sockets.erase(_sockets.begin() + i);
		}
	}
}

void ClientRequest::sendResponse()
{
	for (size_t i = 1; i < _sockets.size(); i++)
	{
		if ((_sockets[i].revents & POLLOUT) && !(_sockets[i].revents & POLLIN) && !_client[_sockets[i].fd].empty())
		{
			displayRequest(_client[_sockets[i].fd], 0);
			MakeResponse response(_client[_sockets[i].fd]);
			displayRequest(response.getResponse(), 1);
			fcntl(_sockets[i].fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
			ssize_t bytesSent = 0;
			while (bytesSent < response.getResponse().size())
			{
				std::string partialResponse= response.getResponse().substr(bytesSent);
				ssize_t bytes = send(_sockets[i].fd, partialResponse.c_str(), partialResponse.size(), 0);
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
			close(_sockets[i].fd);
			_client.erase(_sockets[i].fd);
			_sockets.erase(_sockets.begin() + i);
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