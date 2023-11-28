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
	//Server *server;
	try
	{
		Server server(configFile);
	}
	// catch(const std::invalid_argument& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	//system("lsof -c webserv");
	// 	return (1);
	// }
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	//delete server;
	//system("leaks webserv");
	//system("lsof -c webserv");
	return (0);
}


// question sur le body size
