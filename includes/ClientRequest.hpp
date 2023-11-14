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

#include "Server.hpp"
#include "MakeResponse.hpp"

class ClientInfo
{
	public :
		ClientInfo() {}
		~ClientInfo() {}
		void		setRequest(std::string request)
		{
			_request = request;
		}
		void		setBelongOgServer(int ref)
		{
			_belongOfServer = ref;
		}
		std::string	getRequest()
		{
			return _request;
		}
		int			getBelongOfServer()
		{
			return _belongOfServer;
		}

	private :
		std::string _request;
		int			_belongOfServer;
};

class ClientRequest
{
	public :
		ClientRequest(std::vector<std::vector<int> > serverSockets);
		~ClientRequest();

		void		manageRequest();
		void		readRequest();
		void		sendResponse();
		void		acceptNewClient();
		void		pollFunc();
		void		listenning();

		void		displayRequest(const std::string &, int) const;

	private :
		std::vector<int>			_socketsByServer;
		int							_totalServerSockets;
		std::vector<struct pollfd>	_pollSockets;
		std::map<int, ClientInfo>	_clients;
		//std::map<int, std::string>	_client;
};

