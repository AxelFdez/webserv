/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chris <chris@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 10:12:55 by chris             #+#    #+#             */
/*   Updated: 2023/10/30 10:15:12 by chris            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server( const char* address, int port ) {

    socketServer( address, port );
    bindSocketServer();
    listening();
}

Server::~Server() {

    close( _serverSocket );
}

void Server::socketServer( const char* address, int port ) {

    _serverSocket = socket( AF_INET, SOCK_STREAM, 0 );

    _addrServer.sin_addr.s_addr = inet_addr( address );
    _addrServer.sin_family = AF_INET;
    _addrServer.sin_port = htons( port );
}

void Server::bindSocketServer() {

    bind( _serverSocket, (const struct sockaddr *)&_addrServer, sizeof( _addrServer ) );
    std::cout << "bind : " << _serverSocket << std::endl;
}

std::string Server::readFileToString( std::string const& fileToRead ) {

    std::ifstream file( fileToRead );
    std::string fileContent;
    if (file) {
        std::string line;
        while (std::getline(file, line)) {
            fileContent += line + "\n";
        }
        file.close();
        return fileContent;
    }
    return NULL;
}

std::string Server::addHeader( std::string & htmlContent ) {

    std::string header =    "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n"
                            "Content-Length: " + std::to_string(htmlContent.size()) + "\r\n"
                            "\r\n";
    return header + htmlContent;

}

void Server::listening() {

//     while (true) {
        
//         listen( _serverSocket, 5 );
//         std::cout << B_ORANGE "listen" RESET << std::endl;
//         // Ajoutez le socket du serveur à surveiller avec POLLIN
//         struct pollfd serverPollfd;
//         serverPollfd.fd = _serverSocket;
//         serverPollfd.events = POLLIN;
//         _pollfds.push_back(serverPollfd);

//         // Utilisez poll pour surveiller les sockets
//         int ready = poll(&_pollfds[0], _pollfds.size(), -1); // Attendez indéfiniment

//         if (ready == -1) {
//             perror("Erreur lors de l'appel à poll");
//             continue;
//         }

//         for (size_t i = 0; i < _pollfds.size(); i++) {
//             if (_pollfds[i].revents & POLLIN) {
//                 if (_pollfds[i].fd == _serverSocket) {
//                     // Nouvelle connexion entrante, acceptez-la
//                     socklen_t csize = sizeof(_addrClient);
//                     int clientSocket = accept(_serverSocket, (struct sockaddr *)&_addrClient, &csize);
//                     std::cout << B_ORANGE "Nouvelle connexion" RESET << std::endl;

//                     // Ajoutez le nouveau socket client à surveiller avec POLLIN
//                     struct pollfd clientPollfd;
//                     clientPollfd.fd = clientSocket;
//                     clientPollfd.events = POLLIN;
//                     _pollfds.push_back(clientPollfd);
//                 } else {
//                     // Lecture de données depuis un client existant
//                     char buffer[4096];
//                     int bytesRead = recv(_pollfds[i].fd, buffer, sizeof(buffer), 0);

//                     if (bytesRead > 0) {
//                         std::string requestBuffer(buffer, bytesRead);
//                         std::cout << B_ORANGE "requestBuffer:\n" RESET << _requestBuffer << std::endl;

//                     if (_requestBuffer.find("GET /page1.html") != std::string::npos) {
//                         puts("demande de la page 1");
//                         _htmlContent = readFileToString("./website/page1.html");
//                         _response = addHeader( _htmlContent );
//                         send( _clientSocket, _response.c_str(), _response.size(), 0);
//                         std::cout << B_ORANGE "response:\n" RESET << _response << std::endl;
//                     }

//                     else if (_requestBuffer.find("GET /") != std::string::npos) {
//                         puts("demande de index.html");
//                         // Renvoie la page index.html
//                         _htmlContent = readFileToString("./website/index.html");
//                         _response = addHeader( _htmlContent );
//                         send( _clientSocket, _response.c_str(), _response.size(), 0);
//                         std::cout << B_ORANGE "response:\n" RESET << _response << std::endl;
//                     }
//                     } else {
//                         // Gestion de la déconnexion du client
//                         close(_pollfds[i].fd);
//                         _pollfds.erase(_pollfds.begin() + i);
//                     }
//                 }
//             }
//         }
//     }
// }
    while ( true ) {


        listen( _serverSocket, 5 );
        std::cout << B_ORANGE "listen" RESET << std::endl;

        socklen_t csize = sizeof( _addrClient );

        _clientSocket = accept( _serverSocket, (struct sockaddr *)&_addrClient, &csize );
        std::cout << B_ORANGE "accept" RESET << std::endl;

        std::cout << B_ORANGE "client : " RESET << _clientSocket << std::endl;

        char buffer[4096];
        int bytesRead = recv(_clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            _requestBuffer = std::string(buffer, bytesRead);
        }
        std::cout << B_ORANGE "requestBuffer:\n" RESET << _requestBuffer << std::endl;
        

        if (_requestBuffer.find("GET /page1.html") != std::string::npos) {
            puts("demande de la page 1");
            _htmlContent = readFileToString("./website/page1.html");
            _response = addHeader( _htmlContent );
            send( _clientSocket, _response.c_str(), _response.size(), 0);
            std::cout << B_ORANGE "response:\n" RESET << _response << std::endl;
        }
       
        else /*(_requestBuffer.find("GET /") != std::string::npos)*/ {
            puts("demande de index.html");
            // Renvoie la page index.html
            _htmlContent = readFileToString("./website/index.html");
            _response = addHeader( _htmlContent );
            send( _clientSocket, _response.c_str(), _response.size(), 0);
            std::cout << B_ORANGE "response:\n" RESET << _response << std::endl;
        }
        

        _response.clear();
        _requestBuffer.clear();
        _htmlContent.clear();
        

        close( _clientSocket );

    }
}
