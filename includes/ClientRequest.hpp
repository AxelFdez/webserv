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

// class ClientInfo
// {
// 	public :
// 		ClientInfo() {}
// 		~ClientInfo() {}
// 		void		setRequest(std::vector<char> request)
// 		{
// 			_request = request;
// 		}
// 		void		setBelongOgServer(int ref)
// 		{
// 			_belongOfServer = ref;
// 		}
// 		void setClientIP( std::string IP) {
// 			_clientIP = IP;
// 		}
// 		std::vector<char>	getRequest()
// 		{
// 			return _request;
// 		}
// 		int			getBelongOfServer()
// 		{
// 			return _belongOfServer;
// 		}
// 		std::string getClientIP() {
// 			return _clientIP;
// 		}
// 		void		setBodySize(int value)
// 		{
// 			_bodySize = value;
// 		}
// 		int 		getBodySize()
// 		{
// 			return _bodySize;
// 		}
// 		void		setResponse(std::string response)
// 		{
// 			_response = response;
// 		}
// 		std::string getResponse()
// 		{
// 			return _response;
// 		}
// 		void		setBytesSent(int bytes)
// 		{
// 			_bytesSent = bytes;
// 		}
// 		int			getBytesSent()
// 		{
// 			return _bytesSent;
// 		}
// 		void		setReady(bool value)
// 		{
// 			_ready = value;
// 		}
// 		bool		getReady()
// 		{
// 			return _ready;
// 		}
// 	private :
// 		std::vector<char>	_request;
// 		int					_belongOfServer;
// 		std::string 		_clientIP;
// 		int					_bodySize;
// 		std::string			_response;
// 		int					_bytesSent;
// 		bool				_ready;
// };

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
