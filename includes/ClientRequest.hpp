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

class ClientRequest
{
	public :
		ClientRequest(int serverSocket);
		~ClientRequest();

		void		manageRequest();
		void		readRequest();
		void		sendResponse();
		void		acceptNewClient();
		void		pollFunc();
		void		listenning();

	private :

		std::vector<struct pollfd>	_sockets;
		std::map<int, std::string>	_client;
};