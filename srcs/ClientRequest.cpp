#include "../includes/ClientRequest.hpp"

ClientRequest::ClientRequest(int serverSocket)
{
	pollfd pfd;
	pfd.fd = serverSocket;
	pfd.events = POLLIN;
	// _sockets.push_back(pfd);
	//_client.push_back(newData);
	_sockets.push_back(pfd);
	//_client.insert(std::make_pair(pfd, std::map<std::string, std::string>()));
	//_client.insert({pfd, {{"NULL", "NULL"}}});
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
		//std::cout << "nb client = " << _sockets.size() - 1 << std::endl;
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
}

void ClientRequest::listenning()
{
	if (listen(_sockets[0].fd, 10) < 0)
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
		newClient = accept(_sockets[0].fd, (struct sockaddr *)&clientAddr, &clientLen);
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
}

void ClientRequest::readRequest()
{

	for (size_t i = 1; i < _sockets.size(); i++)
	{
		if (_sockets[i].revents & POLLIN)
		{
			char request[1024];
			ssize_t bytes_received = recv(_sockets[i].fd, request, sizeof(request), 0);
			if (bytes_received < 0)
			{
				perror("Erreur lors de la réception des données");
				close(_sockets[i].fd);
				_sockets.erase(_sockets.begin() + i);
			}
			else if (bytes_received == 0)
			{
				printf("La connexion a été fermée par l'autre extrémité\n");
				close(_sockets[i].fd);
				_sockets.erase(_sockets.begin() + i);
			}
			else
			{
				request[bytes_received] = '\0';
				//printf("%s\n", request);
				//_client.insert(std::pair<int, std::string>(_sockets[i].fd, request));
				// make response here
				MakeResponse makeResponse(request);
			}

		}
	}
	//return std::string(request);
}


void ClientRequest::sendResponse()
{
	for (size_t i = 1; i < _sockets.size(); i++)
	{
		if (_sockets[i].revents & POLLOUT)
		{
			fcntl(_sockets[i].fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
			std::ifstream ifs;
			std::string line;
			ifs.open("/Users/axelfernandez/ecole42/cursus42/webserv/srcs/tools/index.html", std::fstream::in);
			if (!ifs.is_open())
			{
				perror("file cannot be openned");
				throw std::exception();
			}
			std::string res;
			while(getline(ifs, line))
			{
				res.append(line);
			}
			std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(res.size()) + "\r\n\r\n";
			res.insert(0, header);
			send(_sockets[i].fd, res.c_str(), res.size(), 0);
			_client.erase(_sockets[i].fd);
			close(_sockets[i].fd);
			_sockets.erase(_sockets.begin() + i);
		}
	}
}
