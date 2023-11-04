#include "../includes/Config.hpp"
#include "../includes/Server.hpp"

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
		// std::cout << getHtmlPage("/Users/axelfernandez/ecole42/cursus42/webserv/srcs/tools/index.html");
		// exit(1);
		Server server(configFile);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}


	return (0);
}





/*

-- parsing du fichier et recupération des variables pour la création du network ==> class ConfigFile(argv[1])
-- creation du socket ==> class serverSocket()
-- mis en place de la boucle d'ecoute => poll()
-- parsing de la requete du navigateur
-- creation de la reponse + envoi


-- signal() pour exit le server


*/