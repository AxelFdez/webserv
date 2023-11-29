
#pragma once

#include <vector>
#include <iostream>

class ClientInfo
{
	public:

		ClientInfo() {}
		~ClientInfo() {}
        //ClientInfo( ClientInfo const& src );
        //ClientInfo & operator=( ClientInfo const& rhs );

		void		        setRequest( std::vector<char> request );
		void		        setBelongOgServer( int ref );
		void                setClientIP( std::string IP );
		void		        setBodySize( int value );
		void		        setResponse( std::string response );
		void		        setBytesSent( size_t bytes );
		void		        setReady( bool value );

		std::vector<char>	getRequest();
		int			        getBelongOfServer();
		int 		        getBodySize();
		size_t		        getBytesSent();
		std::string         getClientIP();
		std::string         getResponse();
		bool		        getReady();

	private:


		std::vector<char>	_request;
		int					_belongOfServer;
		int					_bodySize;
		size_t					_bytesSent;
		std::string 		_clientIP;
		std::string			_response;
		bool				_ready;
};