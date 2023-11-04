#include "../includes/main.hpp"

// TAG: Prints the usage of the program
void usage()
{
	std::cerr << RED_BOLD << "Error: Wrong number of arguments" << RESET << std::endl;
	std::cout << std::endl
			  << WHITE_BOLD << "Name:\twebserv" << std::endl;
	std::cout << "Usage:\t./webserv\t\t" << MAGENTA_BOLD << "Runs a webserver using the default config file" << RESET << std::endl;
	std::cout << "\t./webserv <config file>\t" << MAGENTA_BOLD << "Runs a webserver using a specific config file" << RESET << std::endl;
	exit(1);
}

// TAG: Handles the SIGINT and SIGTSTP signals which are sent when the user presses CTRL+C or CTRL+Z
void signalHandler(int signal)
{
	if (signal == SIGINT || signal == SIGTSTP)
		exit(0);
	// TODO (close all the fds and exit)
}

int main(int ac, char **av)
{
	std::signal(SIGINT, signalHandler);
	std::signal(SIGPIPE, SIG_IGN);
	if (ac == 1 || ac == 2)
	{
		try
		{
			std::string config_file;
			ConfParser config_parser;
			Multiplexer multiplexer;

			config_file = (ac == 1 ? "./config/default.conf" : av[1]);
			config_parser.parse(config_file);
			multiplexer.createServers(config_parser.getServers());
			multiplexer.runServers();
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
			return (1);
		}
	}
	else
		usage();
	return (0);
}
