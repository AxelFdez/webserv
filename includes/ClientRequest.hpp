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

class ClientInfo
{
	public :
		ClientInfo() {}
		~ClientInfo() {}
		void		setRequest(std::vector<char> request)
		{
			_request = request;
		}
		void		setBelongOgServer(int ref)
		{
			_belongOfServer = ref;
		}
		void setClientIP( std::string IP) {
			_clientIP = IP;
		}
		std::vector<char>	getRequest()
		{
			return _request;
		}
		int			getBelongOfServer()
		{
			return _belongOfServer;
		}
		std::string getClientIP() {
			return _clientIP;
		}

	private :
		std::vector<char>	_request;
		int					_belongOfServer;
		std::string 		_clientIP;
};

class ClientRequest
{
	public :
		ClientRequest(std::vector<std::vector<int> > serverSockets, HandleConfigFile config);
		~ClientRequest();

		void		manageRequest();
		void		readRequest();
		void		sendResponse();
		void		acceptNewClient();
		void		pollFunc();
		void		listenning();


	private :
		ClientRequest();
		HandleConfigFile			 _config;
		std::vector<int>			_socketsByServer;
		int							_totalServerSockets;
		std::vector<struct pollfd>	_pollSockets;
		std::map<int, ClientInfo>	_clients;

		
		//std::map<int, std::string>	_client;
};

void		displayRequest(const std::string &, int);
