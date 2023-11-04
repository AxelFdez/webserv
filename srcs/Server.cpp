#include "../includes/Server.hpp"


Server::Server(const std::string &configFile) : _config(configFile)
{
	launchServer();
	handleClients();
}

Server::~Server()
{
	close(_socket);
}

void Server::createSocket()
{
	std::cout << "Create socket..." << std::endl;
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (!_socket)
	{
		std::cout << "error : socket not create" << std::endl;
		throw std::exception();
	}
	std::cout << "Socket created." << std::endl;
	 int optval = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Erreur lors de la configuration de setsockopt");
        close(_socket);
        exit(EXIT_FAILURE);
    }

    // Définition du délai d'attente (timeout)
    struct timeval timeout;
    timeout.tv_sec = 10;  // 10 secondes
    timeout.tv_usec = 0;

    if (setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Erreur lors de la configuration du délai d'attente");
        close(_socket);
        exit(EXIT_FAILURE);
    }
}

void Server::linkAddPort()
{
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = (INADDR_ANY); //_config.address
	_address.sin_port = htons(8080); //_config.port

	std::cout << "link to address and port(s)..." << std::endl;
	if (bind(_socket, (struct sockaddr *)&_address, sizeof(_address)) < 0)
	{
		std::cout << "error : bind failed" << std::endl;
		throw std::exception();
	}
	std::cout << "Bind success." << std::endl;
}

// void Server::listenning()
// {
// 	if (listen(_socket, 10) < 0)
// 	{
// 		std::cout << "error : listen failed" << std::endl;
// 		throw std::exception();
// 	}
// 	std::cout << "Server listenning..." << std::endl;
// }

void Server::launchServer()
{
	createSocket();
	linkAddPort();
	// listenning();
}

void Server::handleClients()
{
	ClientRequest clientRequest(_socket);
	clientRequest.manageRequest();
}

std::string getHtmlPage(const std::string &path)
{
	std::ifstream ifs(path.c_str(), std::ios::binary);
    if (!ifs)
        return "";

    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

std::string generateErrorPage(int errorCode) {
	std::string message;
	if (errorCode == 404)
		message = "Not Found";
	else if (errorCode == 403)
		message = "Forbidden";
	else if (errorCode == 400)
		message = "Bad Request";
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html lang=\"en\">\n";
    html << "<head>\n";
    html << "<meta charset=\"UTF-8\">\n";
    html << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "<title>Error " << errorCode << "</title>\n";
    html << "<style>\n";
	html << "html { color-scheme: light dark; }";
    html << "body { font-family: Arial, sans-serif; text-align: center; padding: 50px; }\n";
    html << "h1 { color: #d33; }\n";
    html << "</style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "<h1>Error " << errorCode << "</h1>\n";
    html << "<h2>" << message << "</h2>\n";
    html << "<p>" << "webserv/chmassa-axfernan" << "</p>\n";
    html << "</body>\n";
    html << "</html>\n";
    return html.str();
}