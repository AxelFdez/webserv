/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chris <chris@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:12:46 by chris             #+#    #+#             */
/*   Updated: 2023/10/30 09:23:12 by chris            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <poll.h>
#include <unistd.h>
#include <string>
#include "policeColor.hpp"

class Server {

    private:
        Server();

        struct sockaddr_in  _addrServer;
        struct sockaddr_in  _addrClient;
        int                 _serverSocket;
        int                 _clientSocket;
        
        std::vector<struct pollfd> _pollfds;
        
        std::string _requestBuffer;
        std::string _htmlContent;
        std::string _response;
    
        void        socketServer( const char* address, int port );
        void        bindSocketServer();
        void        listening();

        std::string readFileToString( std::string const& fileToRead );
        std::string addHeader( std::string & htmlContent );

    public:
        Server( const char* address, int port );
        ~Server();

};

