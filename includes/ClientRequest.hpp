#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <map>
#include <poll.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "Server.hpp"
#include "MakeResponse.hpp"
#include "ClientInfo.hpp"

class ClientInfo;

class ClientRequest
{
	public:

		ClientRequest(std::vector<std::vector<int> > serverSockets, HandleConfigFile config);
		~ClientRequest();

		void		manageRequest();
		void		readRequest();
		void		sendResponse();
		void		acceptNewClient();
		void		pollFunc();
		void		listenning();

		std::string vectorCharToString( std::vector<char> vec );
		bool		doubleLineEndingFound(std::vector<char>);

	private:

		ClientRequest();
		ClientRequest( ClientRequest const& src );
		ClientRequest & operator=( ClientRequest const& rhs );

		HandleConfigFile			 _config;
		std::vector<int>			_socketsByServer;
		int							_totalServerSockets;
		std::vector<struct pollfd>	_pollSockets;
		std::map<int, ClientInfo>	_clients;
};

void		displayRequest(const std::string &, int);
