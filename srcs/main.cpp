#include "../includes/HandleConfigFile.hpp"
#include "../includes/Server.hpp"

static int	ft_strncmp(const char *s1, const char *s2, size_t n) {

	size_t	i = 0;

	if (n <= 0)
		return (0);
	while ((((unsigned char *)s1)[i] != '\0'
		&& ((unsigned char *)s2)[i] != '\0')
			&& (((unsigned char*)s1)[i] == ((unsigned char*)s2)[i])
			&& (i < n - 1))
		i++;
	return (((unsigned char *)s1)[i] - ((unsigned char *)s2)[i]);
}

static std::string find_php_cgi( char** envp ) {

	std::string paths, line;
	std::vector<std::string> vecPaths;
	for ( int i = 0; envp[i] != NULL; i++ ) {

		if ( ft_strncmp(envp[i], "PATH=", 5 ) == 0 ) {
			paths = envp[i] + 5;
			for ( size_t j = 0; j < paths.length(); j++ ) {

				if ( paths[j] == ':' ) {

					vecPaths.push_back(line + "/php-cgi");
					line.clear();
					continue;
				}
				line.push_back(paths[j]);
			}
			if ( !line.empty() ) {
				vecPaths.push_back(line + "/php-cgi");
			}
		}
	}
	for ( size_t i = 0; i < vecPaths.size(); i++ ) {

		if ( access( vecPaths[i].c_str(), X_OK ) == 0 ) {
			return vecPaths[i];
		}
	}
	return "";
}

int main (int argc, char **argv, char **envp)
{
	if (argc > 2)
		return (1);

	// std::cout << find_php_cgi( envp ) << std::endl;

	std::string configFile;
	if (argc == 1)
		configFile = "file_to_default_conf";
	else
		configFile = argv[1];
	//Server *server;
	try
	{
		Server server(configFile, find_php_cgi( envp ));
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
