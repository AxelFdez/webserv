#include "../includes/ServerConfig.hpp"
#include "../includes/ServerSocket.hpp"

int main (int argc, char **argv)
{
	if (argc > 2)
		return (1);

	std::string configFile;
	if (argc == 1)
		configFile = "file_to_default_conf";
	else
		configFile = argv[1];
	try
	{
		ServerConfig serverConfig(configFile);
		ServerSocket serverSocket(serverConfig);
	}
	catch(const std::exception& e)
		std::cerr << e.what() << '\n';

	/*
	serverSocket.bind();
	serverSocket.listening();

	while (1)
	{
		connectionClientSocket = accept()
		read (connectionClientSocket, buffer, length); lis le contenu de la requete
		write (connectionClientSocket, response, lengthBuffer); envoi la réponse au client.
		close (connectionClientSocket);

	}

	*/

	return (0);
}





/*

-- parsing du fichier et recupération des variables pour la création du network ==> class ConfigFile(argv[1])
-- creation du socket ==> class serverSocket()

*/