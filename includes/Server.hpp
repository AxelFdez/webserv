#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include "../includes/Config.hpp"
#include "../includes/ClientRequest.hpp"

class Config;
class ClientRequest;

class Server
{
	public :
		Server(const std::string &configFile);
		~Server();

		void launchServer();
		void handleClients();
		//void listenning();


	private :
		Config				 _config;
		int					_socket;
		struct sockaddr_in	_address;

		void createSocket();
		void linkAddPort();


};