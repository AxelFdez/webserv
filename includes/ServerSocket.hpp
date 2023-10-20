#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "ServerConfig.hpp"

class ServerSocket
{
	public :
		ServerSocket(ServerConfig config);

	private :

};