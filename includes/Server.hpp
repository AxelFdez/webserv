#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <dirent.h>
#include <csignal>
#include <cstring>
#include "HandleConfigFile.hpp"
#include "ClientRequest.hpp"
#include "utils.hpp"


class Config;
class ClientRequest;
class HandleConfigFile;

class Server
{
	public:

		Server(const std::string &configFile, std::string cgi_path);
		~Server();

		void launchServer();
		void handleClients();
		//void listenning();


	private:

		Server( Server const& src );
		Server & operator=( Server const& rhs );

		HandleConfigFile				 _config;
		std::vector<std::vector<int> >	_socket;
		struct sockaddr_in				_address;

		void createSocket();
		void linkAddPort();
		in_addr_t convIpToLong(const std::string& ipAddress);

};

