#include "../includes/HandleConfigFile.hpp"
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
	Server *server;
	try
	{
		server = new Server(configFile);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	delete server;
	//system("leaks webserv");
	return (0);
}
